/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <glib.h>
#include <glib/gstdio.h>

#include "sc_fs_storage.h"

#ifdef SC_DICTIONARY_FS_STORAGE
#  include "sc_dictionary_fs_storage.h"
#elif SC_ROCKSDB_FS_STORAGE
#  include "sc_rocksdb_fs_storage.h"
#  include "../sc_link_helpers.h"
#endif

#include "sc_file_system.h"

#include "../sc_segment.h"
#include "../../sc_memory_version.h"

sc_char * repo_path = 0;
sc_char segments_path[MAX_PATH_LENGTH];

int _checksum_type()
{
  return G_CHECKSUM_SHA512;
}

sc_uint8 _checksum_get_size()
{
  return (sc_uint8)g_checksum_type_get_length(_checksum_type());
}

// ----------------------------------------------

sc_bool sc_fs_storage_initialize(const sc_char * path, sc_bool clear)
{
  g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/segments.scdb", path);

  repo_path = g_strdup(path);

#ifdef SC_DICTIONARY_FS_STORAGE
  sc_dictionary_fs_storage_initialize(repo_path);
#elif SC_ROCKSDB_FS_STORAGE
  sc_rocksdb_fs_storage_initialize(repo_path);
#endif

  // clear repository if it needs
  if (clear == SC_TRUE)
  {
    g_message("Clear memory");
    if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) && g_remove(segments_path) != 0)
      g_error("Can't remove segments file: %s", segments_path);

    g_message("Clear file memory");
#ifdef SC_ROCKSDB_FS_STORAGE
    if (sc_rocksdb_fs_storage_clear() != SC_RESULT_OK)
    {
      g_critical("Can't clear file memory");
      return SC_FALSE;
    }
#endif
  }

  return SC_TRUE;
}

sc_bool sc_fs_storage_shutdown(sc_segment ** segments, sc_bool save_segments)
{
  if (save_segments == SC_TRUE)
  {
    g_message("Write segments");
    sc_fs_storage_save(segments);
  }

#ifdef SC_DICTIONARY_FS_STORAGE
  sc_dictionary_fs_storage_shutdown();
#elif SC_ROCKSDB_FS_STORAGE
  sc_rocksdb_fs_storage_shutdown();
#endif

  g_free(repo_path);

  return SC_FALSE;
}

void sc_fs_storage_append_sc_link(sc_element * element, sc_addr addr, sc_char * sc_string, sc_uint32 size)
{
#ifdef SC_DICTIONARY_FS_STORAGE
  sc_dictionary_fs_storage_append_sc_link(addr, sc_string, size);
#elif SC_ROCKSDB_FS_STORAGE
  sc_char * current_string;
  sc_uint32 current_size = 0;
  sc_fs_storage_get_sc_string_ext(element, addr, &current_string, &current_size);

  if (current_size != 0 && current_string != null_ptr)
  {
    sc_check_sum * check_sum;
    sc_link_calculate_checksum(current_string, current_size, &check_sum);
    sc_rocksdb_fs_storage_addr_ref_remove(addr, check_sum);
  }

  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, size, &check_sum);
  sc_rocksdb_fs_storage_addr_ref_append(addr, check_sum);

  memcpy(element->content.data, check_sum->data, check_sum->len);
  sc_rocksdb_fs_storage_write_stream(check_sum, sc_string);
#endif
}

sc_addr sc_fs_storage_get_sc_link(const sc_char * sc_string)
{
#ifdef SC_DICTIONARY_FS_STORAGE
  return sc_dictionary_fs_storage_get_sc_link(sc_string);
#elif SC_ROCKSDB_FS_STORAGE
  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, strlen(sc_string), &check_sum);

  sc_addr * addrs;
  sc_uint32 size = 0;
  sc_rocksdb_fs_storage_find(check_sum, &addrs, &size);

  if (size != 0)
    return *addrs;

  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty);
<<<<<<< HEAD

  sc_addr_hash * addr_hash = sc_dictionary_get_data_from_node(links_dictionary->root, sc_string);
  if (addr_hash != null_ptr)
  {
    sc_addr addr;
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(*addr_hash);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(*addr_hash);
    return addr;
  }

=======
>>>>>>> [memory][fs-storage] Add opportunity to switch between file system storages
  return empty;
#endif
}

sc_bool sc_fs_storage_get_sc_links(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size)
{
#ifdef SC_DICTIONARY_FS_STORAGE
  return sc_dictionary_fs_storage_get_sc_links(sc_string, links, size);
#elif SC_ROCKSDB_FS_STORAGE
  sc_check_sum * check_sum;
  sc_link_calculate_checksum(sc_string, strlen(sc_string), &check_sum);

  return sc_rocksdb_fs_storage_find(check_sum, links, size);
#endif
}

void sc_fs_storage_get_sc_string_ext(sc_element * element, sc_addr addr, sc_char ** sc_string, sc_uint32 * size)
{
#ifdef SC_DICTIONARY_FS_STORAGE
  return sc_dictionary_fs_storage_get_sc_string_ext(addr, sc_string, size);
#elif SC_ROCKSDB_FS_STORAGE
  sc_check_sum check_sum;
  memcpy(check_sum.data, element->content.data, SC_CHECKSUM_LEN);
  check_sum.len = SC_CHECKSUM_LEN;

  sc_char * data = sc_rocksdb_fs_storage_read_stream_new(&check_sum);

  if (data != null_ptr)
  {
    *sc_string = data;
    *size = strlen(data);
  }
  else
  {
    *sc_string = null_ptr;
    *size = 0;
  }
#endif
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
    g_free(in_file);

    if (is_valid == SC_FALSE)
      return SC_FALSE;
  }

  g_message("Segments loaded: %u", *segments_num);

#ifdef SC_DICTIONARY_FS_STORAGE
  sc_dictionary_fs_storage_read_strings();
#endif

  return SC_TRUE;
}

sc_bool sc_fs_storage_write_to_path(sc_segment ** segments)
{
  if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
  {
    if (!sc_fs_mkdirs(repo_path))
      g_error("Can't create a directory %s", repo_path);
  }

  // create temporary file
  gchar * tmp_filename = null_ptr;
  GIOChannel * output = sc_fs_open_tmp_file(repo_path, &tmp_filename, "segments");

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

    g_checksum_update(checksum, (sc_uchar *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE);
  }

  header.segments_num = idx;
  gsize bytes = SC_STORAGE_SEG_CHECKSUM_SIZE;
  g_checksum_get_digest(checksum, header.checksum, &bytes);

  sc_uint32 header_size = sizeof(header);
  if (g_io_channel_write_chars(output, (sc_char *)&header_size, sizeof(header_size), &bytes, null_ptr) !=
          G_IO_STATUS_NORMAL ||
      bytes != sizeof(header_size))
    g_error("Can't write header size: %s", tmp_filename);

  if (g_io_channel_write_chars(output, (sc_char *)&header, header_size, &bytes, null_ptr) != G_IO_STATUS_NORMAL ||
      bytes != header_size)
    g_error("Can't write header: %s", tmp_filename);

  for (idx = 0; idx < header.segments_num; ++idx)
  {
    segment = segments[idx];
    g_assert(segment != null_ptr);

    if (g_io_channel_write_chars(output, (sc_char *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes, null_ptr) !=
            G_IO_STATUS_NORMAL ||
        bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
      g_error("Can't write segment %d into %s", idx, tmp_filename);
  }

  // rename main file
  if (g_file_test(tmp_filename, G_FILE_TEST_IS_REGULAR))
  {
    g_io_channel_shutdown(output, SC_TRUE, NULL);
    g_free(output);
    output = null_ptr;

    if (g_rename(tmp_filename, segments_path) != 0)
      g_error("Can't rename %s -> %s", tmp_filename, segments_path);
  }

  if (tmp_filename != null_ptr)
    g_free(tmp_filename);
  if (checksum != null_ptr)
    g_checksum_free(checksum);
  if (output != null_ptr)
  {
    g_io_channel_shutdown(output, SC_TRUE, null_ptr);
    g_free(output);
  }

  return SC_TRUE;
}

sc_bool sc_fs_storage_save(sc_segment ** segments)
{
  sc_bool result = sc_fs_storage_write_to_path(segments);
#ifdef SC_DICTIONARY_FS_STORAGE
  result &= sc_dictionary_fs_storage_write_strings();
#elif SC_ROCKSDB_FS_STORAGE
  result &= sc_rocksdb_fs_storage_save();
#endif
  return result;
}
