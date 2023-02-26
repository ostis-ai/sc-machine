/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fs_memory.h"

#include "sc_file_system.h"

#include "../sc_segment.h"
#include "../../sc_memory_version.h"
#include "sc_fs_memory_builder.h"

#include "glib/gstdio.h"
#include "../sc-base/sc_assert_utils.h"
#include "../sc-base/sc_message.h"
#include "sc_io.h"

sc_fs_memory_manager * manager;
sc_char * repo_path = null_ptr;
sc_char segments_path[MAX_PATH_LENGTH];

sc_int32 _checksum_type()
{
  return G_CHECKSUM_SHA512;
}

sc_uint8 _checksum_get_size()
{
  return (sc_uint8)g_checksum_type_get_length(_checksum_type());
}

// ----------------------------------------------

sc_bool sc_fs_memory_initialize(const sc_char * path, sc_bool clear)
{
  manager = sc_fs_memory_build();

  g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/segments.scdb", path);
  repo_path = g_strdup(path);

  if (manager->initialize(&manager->fs_memory, repo_path))
    return SC_FALSE;

  // clear repository if it needs
  if (clear == SC_TRUE)
  {
    sc_message("Sc-memory: clear memory");
    if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) && g_remove(segments_path) != 0)
    {
      sc_critical("Sc-memory: can't remove segments file: %s", segments_path);
      return SC_FALSE;
    }

    sc_message("Sc-memory: clear file memory");
    if (manager->clear != null_ptr && manager->clear() != SC_RESULT_OK)
    {
      sc_critical("Sc-memory: can't clear file memory");
      return SC_FALSE;
    }
  }

  return SC_TRUE;
}

sc_bool sc_fs_memory_shutdown(sc_segment ** segments, sc_bool save_segments)
{
  if (save_segments == SC_TRUE)
  {
    sc_message("Sc-memory: write segments");
    if (sc_fs_memory_save(segments))
      return SC_FALSE;
  }

  if (manager->shutdown(manager->fs_memory))
    return SC_FALSE;

  sc_mem_free(repo_path);

  sc_mem_free(manager);

  return SC_FALSE;
}

sc_bool sc_fs_memory_link_string(sc_addr_hash const link_hash, sc_char const * string, sc_uint32 const string_size)
{
  return !manager->link_string(manager->fs_memory, link_hash, string, string_size);
}

sc_bool sc_fs_memory_get_string_by_link_hash(sc_addr_hash const link_hash, sc_char ** string, sc_uint32 * string_size)
{
  return !manager->get_string_by_link_hash(manager->fs_memory, link_hash, string, (sc_uint64 *)string_size);
}

sc_bool sc_fs_memory_get_link_hashes_by_string(sc_char const * string, sc_uint32 const string_size, sc_list ** links)
{
  return !manager->get_link_hashes_by_string(manager->fs_memory, string, string_size, links);
}

sc_bool sc_fs_memory_get_link_hashes_by_substring(
    sc_char const * substring,
    sc_uint32 const substring_size,
    sc_list ** link_hashes,
    sc_uint32 max_length_to_search_as_prefix)
{
  return !manager->get_link_hashes_by_substring(manager->fs_memory, substring, substring_size, link_hashes);
}

sc_bool sc_fs_memory_get_strings_by_substring(
    const sc_char * substring,
    const sc_uint32 substring_size,
    sc_list ** strings,
    sc_uint32 max_length_to_search_as_prefix)
{
  return !manager->get_strings_by_substring(manager->fs_memory, substring, substring_size, strings);
}

sc_bool sc_fs_memory_remove_link_string(sc_addr_hash const link_hash)
{
  return !manager->unlink_string(manager->fs_memory, link_hash);
}

// dictionary read, write and save methods
sc_bool sc_fs_memory_read_from_path(sc_segment ** segments, sc_uint32 * segments_num)
{
  if (g_file_test(repo_path, G_FILE_TEST_IS_DIR) == SC_FALSE)
  {
    sc_fs_mkdirs(repo_path);
  }

  if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) == SC_FALSE)
  {
    sc_message("Sc-memory: there are no segments in %s", segments_path);
    return SC_FALSE;
  }

  // open segments
  {
    sc_io_channel * in_file = sc_io_new_read_channel(segments_path, null_ptr);
    sc_bool is_valid = SC_TRUE;

    if (in_file == null_ptr)
    {
      sc_critical("Sc-memory: Can't open segments from: %s", segments_path);
      return SC_FALSE;
    }

    if (sc_io_channel_set_encoding(in_file, null_ptr, null_ptr) != SC_FS_IO_STATUS_NORMAL)
    {
      sc_critical("Sc-memory: Can't setup encoding: %s", segments_path);
      return SC_FALSE;
    }

    gsize bytes_num = 0;
    sc_uint32 header_size = 0;
    if ((g_io_channel_read_chars(in_file, (sc_char *)&header_size, sizeof(header_size), &bytes_num, null_ptr) !=
         SC_FS_IO_STATUS_NORMAL) ||
        (bytes_num != sizeof(header_size)))
    {
      sc_critical("Sc-memory: Can't read header size");
      return SC_FALSE;
    }

    sc_fs_memory_segments_header header;
    if (header_size != sizeof(header))
    {
      sc_critical("Sc-memory: invalid header size %d != %d", header_size, (int)sizeof(header));
      return SC_FALSE;
    }

    if ((g_io_channel_read_chars(in_file, (sc_char *)&header, sizeof(header), &bytes_num, null_ptr) !=
         SC_FS_IO_STATUS_NORMAL) ||
        (bytes_num != sizeof(header)))
    {
      sc_critical("Sc-memory: can't read header of segments: %s", segments_path);
      return SC_FALSE;
    }

    *segments_num = header.segments_num;

    GChecksum * checksum = null_ptr;
    /// TODO: Check version
    checksum = g_checksum_new(_checksum_type());
    sc_assert(checksum);

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
        sc_critical("Sc-memory: error while read data for segment: %d", i);
        return SC_FALSE;
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
    sc_mem_free(in_file);

    if (is_valid == SC_FALSE)
      return SC_FALSE;
  }

  sc_message("Sc-memory: segments loaded: %u", *segments_num);

  if (manager->load)
  {
    if (manager->load(manager->fs_memory))
      return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_fs_memory_write_to_path(sc_segment ** segments)
{
  if (!sc_fs_isdir(repo_path))
  {
    if (!sc_fs_mkdirs(repo_path))
    {
      sc_critical("Sc-memory: can't create a directory %s", repo_path);
      return SC_FALSE;
    }
  }

  // create temporary file
  gchar * tmp_filename = null_ptr;
  void * output = sc_fs_open_tmp_file(repo_path, &tmp_filename, "segments");

  sc_fs_memory_segments_header header;
  memset(&header, 0, sizeof(sc_fs_memory_segments_header));
  header.segments_num = 0;
  header.timestamp = g_get_real_time();

  sc_io_channel_set_encoding(output, null_ptr, null_ptr);

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
  if (sc_io_channel_write_chars(output, (sc_char *)&header_size, sizeof(header_size), &bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      bytes != sizeof(header_size))
  {
    sc_critical("Sc-memory: error while `header_size` writing");
    return SC_FALSE;
  }

  if (g_io_channel_write_chars(output, (sc_char *)&header, header_size, &bytes, null_ptr) != SC_FS_IO_STATUS_NORMAL ||
      bytes != header_size)
  {
    sc_critical("Sc-memory: error while `header` writing");
    return SC_FALSE;
  }

  for (idx = 0; idx < header.segments_num; ++idx)
  {
    segment = segments[idx];

    if (g_io_channel_write_chars(output, (sc_char *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
    {
      sc_critical("Sc-memory: error while `segment->elements` writing");
      return SC_FALSE;
    }
  }

  // rename main file
  if (g_file_test(tmp_filename, G_FILE_TEST_IS_REGULAR))
  {
    if (g_rename(tmp_filename, segments_path) != 0)
    {
      sc_critical("Sc-memory: can't rename %s -> %s", tmp_filename, segments_path);
      return SC_FALSE;
    }
  }

  if (tmp_filename != null_ptr)
    sc_mem_free(tmp_filename);
  if (checksum != null_ptr)
    g_checksum_free(checksum);

  sc_io_channel_shutdown(output, SC_TRUE, null_ptr);

  return SC_TRUE;
}

sc_bool sc_fs_memory_save(sc_segment ** segments)
{
  sc_bool sc_memory_result = sc_fs_memory_write_to_path(segments);
  sc_bool sc_fs_memory_result = !manager->save(manager->fs_memory);
  return sc_memory_result && sc_fs_memory_result;
}
