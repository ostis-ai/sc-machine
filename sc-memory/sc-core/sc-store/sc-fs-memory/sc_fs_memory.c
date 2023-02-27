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
#include "sc_dictionary_fs_memory_private.h"

sc_fs_memory_manager * manager;

sc_int32 _checksum_type()
{
  return G_CHECKSUM_SHA512;
}

// ----------------------------------------------

sc_bool sc_fs_memory_initialize(const sc_char * path, sc_uint32 const max_searchable_string_size, sc_bool clear)
{
  manager = sc_fs_memory_build();

  static sc_char const * segments_postfix = "segments" SC_FS_EXT;
  sc_fs_initialize_file_path(path, segments_postfix, &manager->segments_path);

  if (manager->initialize(&manager->fs_memory, path, max_searchable_string_size) != SC_FS_MEMORY_OK)
    return SC_FALSE;

  // clear repository if it needs
  if (clear == SC_TRUE)
  {
    sc_fs_memory_info("Clear sc-memory segments");
    if (sc_fs_isfile(manager->segments_path) && g_remove(manager->segments_path) != 0)
    {
      sc_fs_memory_error("Can't remove segments file: %s", manager->segments_path);
      return SC_FALSE;
    }

    sc_fs_memory_info("Clear sc-fs-memory");
    if (manager->clear != null_ptr && manager->clear() != SC_FS_MEMORY_OK)
    {
      sc_fs_memory_error("Can't clear sc-fs-memory");
      return SC_FALSE;
    }
  }

  return SC_TRUE;
}

sc_bool sc_fs_memory_shutdown()
{
  if (manager->shutdown(manager->fs_memory) != SC_FS_MEMORY_OK)
    return SC_FALSE;

  sc_mem_free(manager->segments_path);
  sc_mem_free(manager);

  return SC_TRUE;
}

sc_bool sc_fs_memory_link_string(sc_addr_hash const link_hash, sc_char const * string, sc_uint32 const string_size)
{
  return manager->link_string(manager->fs_memory, link_hash, string, string_size) == SC_FS_MEMORY_OK;
}

sc_bool sc_fs_memory_get_string_by_link_hash(sc_addr_hash const link_hash, sc_char ** string, sc_uint32 * string_size)
{
  return manager->get_string_by_link_hash(manager->fs_memory, link_hash, string, (sc_uint64 *)string_size) ==
         SC_FS_MEMORY_OK;
}

sc_bool sc_fs_memory_get_link_hashes_by_string(sc_char const * string, sc_uint32 const string_size, sc_list ** links)
{
  return manager->get_link_hashes_by_string(manager->fs_memory, string, string_size, links) == SC_FS_MEMORY_OK;
}

sc_bool sc_fs_memory_get_link_hashes_by_substring(
    sc_char const * substring,
    sc_uint32 const substring_size,
    sc_uint32 const max_length_to_search_as_prefix,
    sc_list ** link_hashes)
{
  return manager->get_link_hashes_by_substring(
             manager->fs_memory, substring, substring_size, max_length_to_search_as_prefix, link_hashes) ==
         SC_FS_MEMORY_OK;
}

sc_bool sc_fs_memory_get_strings_by_substring(
    const sc_char * substring,
    const sc_uint32 substring_size,
    sc_uint32 const max_length_to_search_as_prefix,
    sc_list ** strings)
{
  return manager->get_strings_by_substring(
             manager->fs_memory, substring, substring_size, max_length_to_search_as_prefix, strings) == SC_FS_MEMORY_OK;
}

sc_bool sc_fs_memory_unlink_string(sc_addr_hash link_hash)
{
  return manager->unlink_string(manager->fs_memory, link_hash) == SC_FS_MEMORY_OK;
}

// dictionary read, write and save methods
sc_bool _sc_fs_memory_load_sc_memory_segments(sc_segment ** segments, sc_uint32 * segments_num)
{
  sc_fs_memory_info("Load sc-memory segments");
  if (sc_fs_isfile(manager->segments_path) == SC_FALSE)
  {
    sc_fs_memory_info("There are no sc-memory segments in %s", manager->segments_path);
    return SC_TRUE;
  }

  // open segments
  {
    sc_io_channel * segments_channel = sc_io_new_read_channel(manager->segments_path, null_ptr);
    if (segments_channel == null_ptr)
    {
      sc_fs_memory_error("Can't open sc-memory segments from: %s", manager->segments_path);
      return SC_FALSE;
    }

    sc_bool is_valid = SC_TRUE;

    if (sc_io_channel_set_encoding(segments_channel, null_ptr, null_ptr) != SC_FS_IO_STATUS_NORMAL)
    {
      sc_fs_memory_error("Can't setup encoding: %s", manager->segments_path);
      return SC_FALSE;
    }

    gsize bytes_num = 0;
    sc_uint32 header_size = 0;
    if (sc_io_channel_read_chars(
            segments_channel, (sc_char *)&header_size, sizeof(header_size), &bytes_num, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        bytes_num != sizeof(header_size))
    {
      sc_fs_memory_error("Can't read header size");
      return SC_FALSE;
    }

    sc_fs_memory_segments_header header;
    if (header_size != sizeof(header))
    {
      sc_fs_memory_error("Invalid header size %d != %d", header_size, (int)sizeof(header));
      return SC_FALSE;
    }

    if (sc_io_channel_read_chars(segments_channel, (sc_char *)&header, sizeof(header), &bytes_num, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        bytes_num != sizeof(header))
    {
      sc_fs_memory_error("Can't read header of segments: %s", manager->segments_path);
      return SC_FALSE;
    }

    *segments_num = header.segments_num;

    GChecksum * checksum = null_ptr;
    /// TODO: Check version
    checksum = g_checksum_new(_checksum_type());

    g_checksum_reset(checksum);

    // check data
    sc_uint32 i;
    sc_segment * seg = null_ptr;
    for (i = 0; i < *segments_num; ++i)
    {
      seg = sc_segment_new(i);
      segments[i] = seg;

      sc_io_channel_read_chars(
          segments_channel, (sc_char *)seg->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes_num, null_ptr);
      sc_segment_loaded(seg);
      if (bytes_num != SC_SEG_ELEMENTS_SIZE_BYTE)
      {
        sc_fs_memory_error("Error while read data for segment: %d", i);
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
    sc_io_channel_shutdown(segments_channel, SC_FALSE, null_ptr);

    if (is_valid == SC_FALSE)
      return SC_FALSE;
  }

  sc_fs_memory_info("Sc-memory segments loaded: %u", *segments_num);
  return SC_TRUE;
}

sc_bool sc_fs_memory_load(sc_segment ** segments, sc_uint32 * segments_num)
{
  sc_bool const sc_memory_result = _sc_fs_memory_load_sc_memory_segments(segments, segments_num);
  sc_bool const sc_fs_memory_result = manager->load(manager->fs_memory) == SC_FS_MEMORY_OK;
  return sc_memory_result && sc_fs_memory_result;
}

sc_bool _sc_fs_memory_save_sc_memory_segments(sc_segment ** segments)
{
  sc_fs_memory_info("Save sc-memory segments");

  if (!sc_fs_isdir(manager->fs_memory->path))
  {
    if (!sc_fs_mkdirs(manager->fs_memory->path))
    {
      sc_fs_memory_error("Can't create a directory %s", manager->fs_memory->path);
      return SC_FALSE;
    }
  }

  // create temporary file
  sc_char * tmp_filename = null_ptr;
  sc_io_channel * segments_channel = sc_fs_open_tmp_file(manager->fs_memory->path, &tmp_filename, "segments");
  sc_io_channel_set_encoding(segments_channel, null_ptr, null_ptr);

  sc_fs_memory_segments_header header;
  sc_mem_set(&header, 0, sizeof(sc_fs_memory_segments_header));
  header.segments_num = 0;
  header.timestamp = g_get_real_time();

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
  if (sc_io_channel_write_chars(segments_channel, (sc_char *)&header_size, sizeof(header_size), &bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      bytes != sizeof(header_size))
  {
    sc_fs_memory_error("Error while attribute `header_size` writing");
    return SC_FALSE;
  }

  if (sc_io_channel_write_chars(segments_channel, (sc_char *)&header, header_size, &bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      bytes != header_size)
  {
    sc_fs_memory_error("Error while attribute `header` writing");
    return SC_FALSE;
  }

  for (idx = 0; idx < header.segments_num; ++idx)
  {
    segment = segments[idx];

    if (sc_io_channel_write_chars(
            segments_channel, (sc_char *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
    {
      sc_fs_memory_error("Error while attribute `segment->elements` writing");
      return SC_FALSE;
    }
  }

  // rename main file
  if (sc_fs_isfile(tmp_filename))
  {
    if (g_rename(tmp_filename, manager->segments_path) != 0)
    {
      sc_fs_memory_error("Can't rename %s -> %s", tmp_filename, manager->segments_path);
      return SC_FALSE;
    }
  }

  if (tmp_filename != null_ptr)
    sc_mem_free(tmp_filename);
  if (checksum != null_ptr)
    g_checksum_free(checksum);

  sc_io_channel_shutdown(segments_channel, SC_TRUE, null_ptr);

  sc_fs_memory_info("Sc-memory segments saved");
  return SC_TRUE;
}

sc_bool sc_fs_memory_save(sc_segment ** segments)
{
  sc_bool sc_memory_result = _sc_fs_memory_save_sc_memory_segments(segments);
  sc_bool sc_fs_memory_result = manager->save(manager->fs_memory) == SC_FS_MEMORY_OK;
  return sc_memory_result && sc_fs_memory_result;
}
