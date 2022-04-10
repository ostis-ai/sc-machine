/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <memory.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "sc_fs_storage.h"

#include "sc_segment.h"

#include "sc_config.h"

#include "../sc_memory_version.h"

#define BUFFER_SIZE 256 * 1024

sc_char * repo_path = 0;
sc_char segments_path[MAX_PATH_LENGTH];
sc_char strings_path[MAX_PATH_LENGTH];
sc_char links_path[MAX_PATH_LENGTH];

sc_dictionary * links_dictionary;
sc_dictionary * strings_dictionary;

int _checksum_type()
{
  return G_CHECKSUM_SHA512;
}

sc_uint8 _checksum_get_size()
{
  return (sc_uint8)g_checksum_type_get_length(_checksum_type());
}

void _sc_fm_remove_dir(const sc_char * path)
{
  if (g_file_test(path, G_FILE_TEST_IS_DIR) == SC_FALSE)
    return;

  GDir * directory = g_dir_open(path, 0, 0);
  g_assert(directory != null_ptr);

  // calculate files
  sc_char tmp_path[MAX_PATH_LENGTH];
  sc_char const * file = g_dir_read_name(directory);
  while (file != null_ptr)
  {
    g_snprintf(tmp_path, MAX_PATH_LENGTH, "%s/%s", path, file);

    if (g_file_test(tmp_path, G_FILE_TEST_IS_REGULAR) == SC_TRUE)
    {
      if (g_remove(tmp_path) == -1)
        g_critical("Can't remove file: %s", tmp_path);
    }
    else if (g_file_test(tmp_path, G_FILE_TEST_IS_DIR) == SC_TRUE)
      _sc_fm_remove_dir(tmp_path);

    file = g_dir_read_name(directory);
  }

  g_dir_close(directory);
  g_rmdir(path);
}

// ----------------------------------------------

sc_bool _sc_fs_mkdirs(const sc_char * path)
{
#if SC_PLATFORM_LINUX || SC_PLATFORM_MAC
  int const mode = 0755;
#else
  int const mode = 0;
#endif
  if (g_mkdir_with_parents(repo_path, mode) == -1)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_fs_storage_initialize(const sc_char * path, sc_bool clear)
{
  g_message("Initialize sc-dictionary from path: %s", path);
  sc_bool result = sc_dictionary_initialize(&links_dictionary);
  if (result == SC_FALSE)
    return SC_FALSE;

  result = sc_dictionary_initialize(&strings_dictionary);
  if (result == SC_FALSE)
    return SC_FALSE;

  g_message("Initialize sc-storage from path: %s", path);
  g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/segments.scdb", path);
  g_snprintf(strings_path, MAX_PATH_LENGTH, "%s/strings.scdb", path);
  g_snprintf(links_path, MAX_PATH_LENGTH, "%s/links.scdb", path);

  repo_path = g_strdup(path);

  // clear repository if it needs
  if (clear == SC_TRUE)
  {
    g_message("Clear memory");
    if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) && g_remove(segments_path) != 0)
      g_error("Can't remove segments file: %s", segments_path);
  }

  return SC_TRUE;
}

sc_bool sc_fs_storage_shutdown(sc_segment ** segments, sc_bool save_segments)
{
  if (save_segments == SC_TRUE)
  {
    g_message("Write segments");
    sc_fs_storage_write_to_path(segments);
    g_message("Write sc-dictionary");
    sc_fs_storage_write_strings();
  }

  g_message("Shutdown sc-dictionary");
  if (sc_dictionary_shutdown(links_dictionary) == SC_FALSE)
    g_critical("Can't shutdown sc-dictionary");

  if (sc_dictionary_shutdown(strings_dictionary) == SC_FALSE)
    g_critical("Can't shutdown sc-dictionary");

  g_free(repo_path);

  g_message("Shutdown sc-fs-storage");

  return SC_FALSE;
}

sc_bool sc_hashes_compare(void * hash, void * other)
{
  return *(sc_addr_hash *)hash == *(sc_addr_hash *)other;
}

void _sc_fs_storage_append_sc_string_reference(
    sc_addr addr,
    sc_dictionary_node * node,
    sc_char * sc_string,
    sc_uint32 size)
{
  sc_char * hash_str = sc_addr_to_str(addr);
  sc_addr_hash hash_str_len = strlen(hash_str);

  sc_dictionary_node * link_hash_node = sc_dictionary_append_to_node(strings_dictionary->root, hash_str, hash_str_len);

  if (link_hash_node->data_list == null_ptr)
    sc_list_init(&link_hash_node->data_list);

  if (link_hash_node->data_list->size != 0)
    g_free(sc_list_pop_back(link_hash_node->data_list));

  sc_link_content * content = g_new0(sc_link_content, 1);

  content->sc_string = g_new0(sc_char, size + 1);
  content->string_size = size;
  memcpy(content->sc_string, sc_string, size);
  content->node = &(*node);

  sc_list_push_back(link_hash_node->data_list, content, 1);
}

sc_dictionary_node * _sc_fs_storage_append_sc_link_unique(sc_char * sc_string, sc_uint32 size, sc_addr addr)
{
  sc_link_content * old_content = sc_fs_storage_get_link_content(addr);
  if (old_content != null_ptr && old_content->sc_string != null_ptr && old_content->node != null_ptr)
  {
    sc_list * data_list = old_content->node->data_list;
    sc_addr_hash other = sc_addr_to_hash(addr);
    sc_list_remove_if(data_list, &other, 1, sc_hashes_compare);
  }

  sc_addr_hash * hash = g_new0(sc_addr_hash, 1);
  *hash = sc_addr_to_hash(addr);
  return sc_dictionary_append(links_dictionary, sc_string, size, hash);
}

sc_dictionary_node * sc_fs_storage_append_sc_link(sc_addr addr, sc_char * sc_string, sc_uint32 size)
{
  sc_dictionary_node * node = _sc_fs_storage_append_sc_link_unique(sc_string, size, addr);
  _sc_fs_storage_append_sc_string_reference(addr, node, sc_string, size);

  return node;
}

sc_addr * sc_list_to_addr_array(sc_list * list)
{
  sc_addr * addrs = g_new0(sc_addr, list->size);
  sc_addr ** temp = g_new0(sc_addr *, 1);
  *temp = addrs;

  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_addr_hash hash = *(sc_addr_hash *)sc_iterator_get(it)->value;
    sc_addr addr;
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(hash);
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(hash);

    **temp = addr;
    ++*temp;
  }

  return addrs;
}

sc_addr_hash * sc_list_to_hashes_array(sc_list * list)
{
  sc_addr_hash * hashes = g_new0(sc_addr_hash, list->size);
  sc_addr_hash ** temp = g_new0(sc_addr_hash *, 1);
  *temp = hashes;

  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    **temp = *(sc_addr_hash *)sc_iterator_get(it)->value;
    ++*temp;
  }

  return hashes;
}

sc_addr sc_fs_storage_get_sc_link(const sc_char * sc_string)
{
  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty)

  sc_addr_hash * addr_hash = sc_dictionary_get_data_from_node(links_dictionary->root, sc_string);
  if (addr_hash != null_ptr)
  {
    sc_addr addr;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(*addr_hash);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(*addr_hash);
    return addr;
  }

  return empty;
}

sc_bool sc_fs_storage_get_sc_links(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size)
{
  sc_list * list = sc_dictionary_get_datas_from_node(links_dictionary->root, sc_string);

  if (list == null_ptr)
  {
    *links = null_ptr;
    *size = 0;
    return SC_FALSE;
  }

  *links = sc_list_to_addr_array(list);
  *size = list->size;
  *links = memcpy(*links, *links, *size);

  return SC_TRUE;
}

sc_char * sc_fs_storage_get_sc_string(sc_addr addr)
{
  if (SC_ADDR_IS_EMPTY(addr))
    return null_ptr;

  sc_link_content * content = sc_dictionary_get_data_from_node(strings_dictionary->root, sc_addr_to_str(addr));

  if (content == null_ptr || content->sc_string == null_ptr)
    return null_ptr;

  sc_uint32 len = content->string_size;
  sc_char * copy = g_new0(sc_char, len + 1);
  return memcpy(copy, content->sc_string, len);
}

sc_dictionary_node * sc_fs_storage_get_node(sc_addr addr)
{
  if (SC_ADDR_IS_EMPTY(addr))
    return null_ptr;

  sc_link_content * content = sc_dictionary_get_data_from_node(strings_dictionary->root, sc_addr_to_str(addr));

  if (content == null_ptr || content->node == null_ptr)
    return null_ptr;

  return content->node;
}

sc_link_content * sc_fs_storage_get_link_content(sc_addr addr)
{
  if (SC_ADDR_IS_EMPTY(addr))
    return null_ptr;

  sc_link_content * content = sc_dictionary_get_data_from_node(strings_dictionary->root, sc_addr_to_str(addr));

  if (content == null_ptr || content->node == null_ptr || content->sc_string == null_ptr)
    return null_ptr;

  return content;
}

void sc_fs_storage_get_sc_string_ext(sc_addr addr, sc_char ** sc_string, sc_uint32 * size)
{
  if (SC_ADDR_IS_EMPTY(addr))
  {
    *sc_string = null_ptr;
    *size = 0;
    return;
  }

  sc_link_content * content = sc_dictionary_get_data_from_node(strings_dictionary->root, sc_addr_to_str(addr));
  if (content == null_ptr)
  {
    *sc_string = null_ptr;
    *size = 0;
    return;
  }

  sc_uint32 len = content->string_size;
  *sc_string = g_new0(sc_char, len + 1);
  memcpy(*sc_string, content->sc_string, len);
  *size = len;
}

// dictionary read, write and save methods
sc_bool sc_fs_storage_read_from_path(sc_segment ** segments, sc_uint32 * segments_num)
{
  if (g_file_test(repo_path, G_FILE_TEST_IS_DIR) == SC_FALSE)
  {
    g_error("%s isn't a directory.", repo_path);
  }

  if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) == SC_FALSE)
  {
    g_message("There are no segments in %s", segments_path);
    return SC_FALSE;
  }

  // open segments
  {
    GIOChannel * in_file = g_io_channel_new_file(segments_path, "r", null_ptr);
    sc_bool is_valid = SC_TRUE;

    g_assert(_checksum_get_size() == SC_STORAGE_SEG_CHECKSUM_SIZE);
    if (in_file == null_ptr)
    {
      g_critical("Can't open segments from: %s", segments_path);
      return SC_FALSE;
    }

    if (g_io_channel_set_encoding(in_file, null_ptr, null_ptr) != G_IO_STATUS_NORMAL)
    {
      g_critical("Can't setup encoding: %s", segments_path);
      return SC_FALSE;
    }

    gsize bytes_num = 0;
    sc_uint32 header_size = 0;
    if ((g_io_channel_read_chars(in_file, (sc_char *)&header_size, sizeof(header_size), &bytes_num, null_ptr) !=
         G_IO_STATUS_NORMAL) ||
        (bytes_num != sizeof(header_size)))
    {
      g_critical("Can't read header size");
      return SC_FALSE;
    }

    sc_fs_storage_segments_header header;
    if (header_size != sizeof(header))
    {
      g_critical("Invalid header size %d != %d", header_size, (int)sizeof(header));
      return SC_FALSE;
    }

    if ((g_io_channel_read_chars(in_file, (sc_char *)&header, sizeof(header), &bytes_num, null_ptr) !=
         G_IO_STATUS_NORMAL) ||
        (bytes_num != sizeof(header)))
    {
      g_critical("Can't read header of segments: %s", segments_path);
      return SC_FALSE;
    }

    *segments_num = header.segments_num;

    GChecksum * checksum = null_ptr;
    /// TODO: Check version
    checksum = g_checksum_new(_checksum_type());
    g_assert(checksum);

    g_checksum_reset(checksum);

    // check data
    sc_uint32 i;
    sc_segment * seg = null_ptr;
    for (i = 0; i < *segments_num; ++i)
    {
      seg = sc_segment_new(i);
      segments[i] = seg;

      g_io_channel_read_chars(in_file, (sc_char *)seg->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes_num, null_ptr);
      sc_segment_loaded(seg);
      if (bytes_num != SC_SEG_ELEMENTS_SIZE_BYTE)
      {
        g_error("Error while read data for segment: %d", i);
      }
      g_checksum_update(checksum, (sc_uchar *)seg->elements, SC_SEG_ELEMENTS_SIZE_BYTE);
    }

    // compare checksum
    sc_uint8 calculated_checksum[SC_STORAGE_SEG_CHECKSUM_SIZE];
    g_checksum_get_digest(checksum, calculated_checksum, &bytes_num);
    if (bytes_num != SC_STORAGE_SEG_CHECKSUM_SIZE)
      is_valid = SC_FALSE;
    else
      is_valid = (memcmp(calculated_checksum, header.checksum, SC_STORAGE_SEG_CHECKSUM_SIZE) == 0) ? SC_TRUE : SC_FALSE;

    if (is_valid == SC_FALSE)
    {
      *segments_num = 0;
      for (i = 0; i < SC_SEGMENT_MAX; ++i)
      {
        if (segments[i])
        {
          sc_segment_free(segments[i]);
          segments[i] = null_ptr;
        }
      }
    }

    g_checksum_free(checksum);
    g_io_channel_shutdown(in_file, SC_FALSE, null_ptr);

    if (is_valid == SC_FALSE)
      return SC_FALSE;
  }

  g_message("Segments loaded: %u", *segments_num);

  return SC_TRUE;
}

static GIOChannel * _open_tmp_file(sc_char ** tmp_file_name, sc_char * prefix)
{
  *tmp_file_name = g_strdup_printf("%s/%s_%lu", repo_path, prefix, (sc_ulong)g_get_real_time());

  GIOChannel * result = g_io_channel_new_file(*tmp_file_name, "w", null_ptr);
  return result;
}

sc_bool sc_fs_storage_write_to_path(sc_segment ** segments)
{
  if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
  {
    if (!_sc_fs_mkdirs(repo_path))
      g_error("Can't create a directory %s", repo_path);
  }

  // create temporary file
  gchar * tmp_filename = null_ptr;
  GIOChannel * output = _open_tmp_file(&tmp_filename, "segments");

  sc_fs_storage_segments_header header;
  memset(&header, 0, sizeof(sc_fs_storage_segments_header));
  header.segments_num = 0;
  header.timestamp = g_get_real_time();
  header.version = sc_version_to_int(&SC_VERSION);

  g_io_channel_set_encoding(output, null_ptr, null_ptr);

  GChecksum * checksum = g_checksum_new(_checksum_type());
  g_checksum_reset(checksum);

  sc_uint32 idx;
  const sc_segment * segment = null_ptr;
  for (idx = 0; idx < SC_ADDR_SEG_MAX; idx++)
  {
    segment = segments[idx];
    if (segment == null_ptr)
      break;  // stop save, because we allocate segment in order

    g_checksum_update(checksum, (guchar *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE);
  }

  header.segments_num = idx;
  gsize bytes = SC_STORAGE_SEG_CHECKSUM_SIZE;
  g_checksum_get_digest(checksum, header.checksum, &bytes);

  sc_uint32 header_size = sizeof(header);
  if (g_io_channel_write_chars(output, (gchar *)&header_size, sizeof(header_size), &bytes, null_ptr) !=
          G_IO_STATUS_NORMAL ||
      bytes != sizeof(header_size))
    g_error("Can't write header size: %s", tmp_filename);

  if (g_io_channel_write_chars(output, (gchar *)&header, header_size, &bytes, null_ptr) != G_IO_STATUS_NORMAL ||
      bytes != header_size)
    g_error("Can't write header: %s", tmp_filename);

  for (idx = 0; idx < header.segments_num; ++idx)
  {
    segment = segments[idx];
    g_assert(segment != null_ptr);

    if (g_io_channel_write_chars(output, (gchar *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes, null_ptr) !=
            G_IO_STATUS_NORMAL ||
        bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
      g_error("Can't write segment %d into %s", idx, tmp_filename);
  }

  // rename main file
  if (g_file_test(tmp_filename, G_FILE_TEST_IS_REGULAR))
  {
    g_io_channel_shutdown(output, SC_TRUE, NULL);
    output = null_ptr;

    if (g_rename(tmp_filename, segments_path) != 0)
      g_error("Can't rename %s -> %s", tmp_filename, segments_path);
  }

  if (tmp_filename != null_ptr)
    g_free(tmp_filename);
  if (checksum != null_ptr)
    g_checksum_free(checksum);
  if (output != null_ptr)
    g_io_channel_shutdown(output, SC_TRUE, null_ptr);

  return SC_TRUE;
}

void sc_fs_storage_write_nodes(
    void (*callable)(sc_dictionary_node *, void **),
    GIOChannel * strings_dest,
    GIOChannel * links_dest)
{
  GIOChannel ** dest = g_new0(GIOChannel *, 2);
  dest[0] = strings_dest;
  dest[1] = links_dest;

  sc_dictionary_visit_nodes(strings_dictionary, callable, (void **)dest);

  g_free(dest);
}

void sc_fs_storage_write_node(sc_dictionary_node * node, void ** dest)
{
  sc_link_content * content = (sc_link_content *)node->data_list->begin->data->value;

  if (content->node->mask & 0xF0)
    return;

  sc_addr_hash * hashes = sc_list_to_hashes_array(content->node->data_list);
  sc_uint8 hashes_size = content->node->data_list->size;

  content->node->mask |= 0xF0;

  GIOChannel * strings_channel = dest[0];
  g_assert(strings_channel != null_ptr);
  GIOChannel * links_channel = dest[1];
  g_assert(links_channel != null_ptr);

  gsize bytes;
  if (g_io_channel_write_chars(strings_channel, (sc_char *)&hashes_size, sizeof(hashes_size), &bytes, null_ptr) !=
      G_IO_STATUS_NORMAL)
    g_error("Can't write string hashes size %d into %s", hashes_size, strings_path);

  if (g_io_channel_write_chars(
          strings_channel, (sc_char *)hashes, sizeof(sc_addr_hash) * hashes_size, &bytes, null_ptr) !=
      G_IO_STATUS_NORMAL)
    g_error("Can't write string hashes %llu into %s", *hashes, strings_path);

  if (g_io_channel_write_chars(
          strings_channel, (sc_char *)&content->string_size, sizeof(content->string_size), &bytes, null_ptr) !=
      G_IO_STATUS_NORMAL)
    g_error("Can't write sc-string size %d into %s", hashes_size, strings_path);

  if (g_io_channel_write_chars(strings_channel, content->sc_string, content->string_size, &bytes, null_ptr) !=
      G_IO_STATUS_NORMAL)
    g_error("Can't write sc-string %s into %s", content->sc_string, strings_path);

  if (g_io_channel_write_chars(
          links_channel, (sc_char *)hashes, sizeof(sc_addr_hash) * hashes_size, &bytes, null_ptr) != G_IO_STATUS_NORMAL)
    g_error("Can't write string hashes %llu into %s", *hashes, links_path);
}

sc_bool sc_fs_storage_write_strings()
{
  if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
  {
    if (!_sc_fs_mkdirs(repo_path))
      g_error("Can't create a directory %s", repo_path);
  }

  sc_char * strings_filename = null_ptr;
  GIOChannel * output_strings = _open_tmp_file(&strings_filename, "strings");
  g_io_channel_set_encoding(output_strings, null_ptr, null_ptr);

  sc_char * links_filename = null_ptr;
  GIOChannel * output_links = _open_tmp_file(&links_filename, "links");
  g_io_channel_set_encoding(output_links, null_ptr, null_ptr);

  sc_fs_storage_write_nodes(sc_fs_storage_write_node, output_strings, output_links);

  if (g_file_test(strings_filename, G_FILE_TEST_IS_REGULAR))
  {
    g_io_channel_shutdown(output_strings, SC_TRUE, null_ptr);
    output_strings = null_ptr;

    if (g_rename(strings_filename, strings_path) != 0)
      g_error("Can't rename %s -> %s", strings_filename, strings_path);
  }

  if (g_file_test(links_filename, G_FILE_TEST_IS_REGULAR))
  {
    g_io_channel_shutdown(output_links, SC_TRUE, null_ptr);
    output_links = null_ptr;

    if (g_rename(links_filename, links_path) != 0)
      g_error("Can't rename %s -> %s", links_filename, links_path);
  }

  if (strings_filename != null_ptr)
    g_free(strings_filename);
  if (output_strings != null_ptr)
    g_io_channel_shutdown(output_strings, SC_TRUE, null_ptr);

  if (links_filename != null_ptr)
    g_free(links_filename);
  if (output_links != null_ptr)
    g_io_channel_shutdown(output_links, SC_TRUE, null_ptr);

  return SC_TRUE;
}

sc_bool sc_fs_storage_read_strings()
{
  if (g_file_test(repo_path, G_FILE_TEST_IS_DIR) == SC_FALSE)
    g_error("%s isn't a directory.", repo_path);

  if (g_file_test(strings_path, G_FILE_TEST_IS_REGULAR) == SC_FALSE)
  {
    g_message("There are no strings in %s", strings_path);
    return SC_FALSE;
  }

  GIOChannel * in_file = g_io_channel_new_file(strings_path, "r", null_ptr);
  if (in_file == null_ptr)
  {
    g_critical("Can't open strings from: %s", strings_path);
    return SC_FALSE;
  }

  if (g_io_channel_set_encoding(in_file, null_ptr, null_ptr) != G_IO_STATUS_NORMAL)
  {
    g_critical("Can't setup encoding: %s", strings_path);
    return SC_FALSE;
  }

  gsize bytes_num = 0;
  while (SC_TRUE)
  {
    sc_uint8 hashes_size = 0;
    if (g_io_channel_read_chars(in_file, (gchar *)&hashes_size, sizeof(hashes_size), &bytes_num, null_ptr) !=
        G_IO_STATUS_NORMAL)
      break;

    sc_addr_hash * hashes = g_new0(sc_addr_hash, hashes_size);
    if (g_io_channel_read_chars(in_file, (gchar *)hashes, sizeof(sc_addr_hash) * hashes_size, &bytes_num, null_ptr) !=
        G_IO_STATUS_NORMAL)
      break;

    sc_uint32 string_size = 0;
    if (g_io_channel_read_chars(in_file, (gchar *)&string_size, sizeof(string_size), &bytes_num, null_ptr) !=
        G_IO_STATUS_NORMAL)
      break;

    sc_char * string = g_new0(sc_char, string_size);
    if (g_io_channel_read_chars(in_file, (gchar *)string, string_size, &bytes_num, null_ptr) != G_IO_STATUS_NORMAL)
      break;

    sc_uint8 i;
    for (i = 0; i < hashes_size; ++i)
    {
      if (hashes[i] == 0)
        continue;

      sc_addr addr;
      addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(hashes[i]);
      addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(hashes[i]);

      sc_fs_storage_append_sc_link(addr, string, string_size);
    }
  }

  g_io_channel_shutdown(in_file, SC_FALSE, null_ptr);

  g_message("SC-dictionary loaded");

  return SC_TRUE;
}
