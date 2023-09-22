/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fs_memory.h"

#include "sc_file_system.h"

#include "../sc_segment.h"
#include "sc_fs_memory_builder.h"

#include "glib/gstdio.h"
#include "../sc-base/sc_message.h"

#include "sc_io.h"
#include "sc_dictionary_fs_memory_private.h"

sc_fs_memory_manager * manager;

sc_bool sc_fs_memory_initialize_ext(sc_memory_params const * params)
{
  manager = sc_fs_memory_build();
  manager->version = params->version;
  manager->path = params->repo_path;

  if (manager->path == null_ptr)
  {
    sc_fs_memory_error("Empty repo path to initialize memory");
    return SC_FALSE;
  }

  static sc_char const * segments_postfix = "segments" SC_FS_EXT;
  sc_fs_concat_path(manager->path, segments_postfix, &manager->segments_path);

  if (manager->initialize(&manager->fs_memory, params) != SC_FS_MEMORY_OK)
    return SC_FALSE;

  // clear repository if it needs
  if (params->clear == SC_TRUE)
  {
    sc_fs_memory_info("Clear sc-memory segments");
    if (sc_fs_remove_file(manager->segments_path) == SC_FALSE)
      sc_fs_memory_info("Can't remove segments file: %s", manager->segments_path);
  }

  return SC_TRUE;
}

sc_bool sc_fs_memory_initialize(sc_char const * path, sc_bool clear)
{
  sc_memory_params * params = _sc_dictionary_fs_memory_get_default_params(path, clear);
  sc_bool const status = sc_fs_memory_initialize_ext(params);
  sc_mem_free(params);
  return status;
}

sc_bool sc_fs_memory_shutdown()
{
  sc_bool const result = manager->shutdown(manager->fs_memory) == SC_FS_MEMORY_OK;

  sc_mem_free(manager->segments_path);
  sc_mem_free(manager);

  return result;
}

sc_bool sc_fs_memory_link_string(sc_addr_hash const link_hash, sc_char const * string, sc_uint32 const string_size)
{
  return manager->link_string(manager->fs_memory, link_hash, string, string_size, SC_TRUE) == SC_FS_MEMORY_OK;
}

sc_bool sc_fs_memory_link_string_ext(
    sc_addr_hash const link_hash,
    sc_char const * string,
    sc_uint32 const string_size,
    sc_bool is_searchable_string)
{
  return manager->link_string(manager->fs_memory, link_hash, string, string_size, is_searchable_string) ==
         SC_FS_MEMORY_OK;
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
  if (sc_fs_is_file(manager->segments_path) == SC_FALSE)
  {
    *segments_num = 0;
    sc_fs_memory_info("There are no sc-memory segments in %s", manager->segments_path);
    return SC_TRUE;
  }

  // open segments
  sc_io_channel * segments_channel = sc_io_new_read_channel(manager->segments_path, null_ptr);
  sc_io_channel_set_encoding(segments_channel, null_ptr, null_ptr);

  if (sc_fs_memory_header_read(segments_channel, &manager->header) != SC_FS_MEMORY_OK)
    goto error;
  *segments_num = manager->header.size;

  // backward compatibility with version 0.7.0
  sc_uint64 read_bytes = 0;
  sc_bool is_no_deprecated_segments = *segments_num == 0;
  if (is_no_deprecated_segments)
    sc_fs_memory_info("Load sc-memory segments from %s", manager->segments_path);
  else
    sc_fs_memory_warning("Load deprecated sc-memory segments from %s", manager->segments_path);

  static sc_uint32 const OLD_SC_ELEMENT_SIZE = 36;
  sc_uint32 element_size = is_no_deprecated_segments ? sizeof(sc_element) : OLD_SC_ELEMENT_SIZE;
  if (is_no_deprecated_segments)
  {
    if (sc_io_channel_read_chars(segments_channel, (sc_char *)segments_num, sizeof(sc_uint32), &read_bytes, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        read_bytes != sizeof(sc_uint32))
    {
      *segments_num = 0;
      sc_fs_memory_error("Error while attribute `segments_num` reading");
      goto error;
    }
  }

  sc_version read_version;
  sc_version_from_int(manager->header.version, &read_version);
  if (sc_version_compare(&manager->version, &read_version) == -1)
  {
    sc_char * version = sc_version_string_new(&read_version);
    sc_fs_memory_error("Read sc-memory segments has incompatible version %s", version);
    sc_version_string_free(version);
    goto error;
  }

  for (sc_uint32 i = 0; i < *segments_num; ++i)
  {
    sc_uint32 const num = i;
    sc_segment * seg = sc_segment_new(i);
    segments[i] = seg;

    for (sc_uint32 j = 0; j < SC_SEGMENT_ELEMENTS_COUNT; ++j)
    {
      if (sc_io_channel_read_chars(
              segments_channel, (sc_char *)&seg->elements[j], element_size, &read_bytes, null_ptr) !=
              SC_FS_IO_STATUS_NORMAL ||
          read_bytes != element_size)
      {
        *segments_num = num;
        sc_fs_memory_error("Error while sc-element %d in sc-segment %d reading", j, i);
        goto error;
      }

      // needed for sc-template search
      if (!is_no_deprecated_segments)
      {
        seg->elements[j].input_arcs_count = 1;
        seg->elements[j].output_arcs_count = 1;
      }
    }

    if (is_no_deprecated_segments)
    {
      if (sc_io_channel_read_chars(
              segments_channel, (sc_char *)&seg->last_element_offset, sizeof(sc_uint32), &read_bytes, null_ptr) !=
              SC_FS_IO_STATUS_NORMAL ||
          read_bytes != sizeof(sc_uint32))
      {
        sc_fs_memory_error("Error while sc-segment %d reading", i);
        goto error;
      }
    }

    i = num;
  }

  sc_io_channel_shutdown(segments_channel, SC_FALSE, null_ptr);

  if (is_no_deprecated_segments)
    sc_fs_memory_info("Sc-memory segments loaded");
  else
    sc_fs_memory_warning("Deprecated sc-memory segments loaded");

  sc_message("\tSc-memory segments: %u", *segments_num);
  return SC_TRUE;

error:
{
  sc_io_channel_shutdown(segments_channel, SC_FALSE, null_ptr);
  return SC_FALSE;
}
}

sc_bool sc_fs_memory_load(sc_segment ** segments, sc_uint32 * segments_num)
{
  sc_bool const sc_memory_result = _sc_fs_memory_load_sc_memory_segments(segments, segments_num);
  sc_bool const sc_fs_memory_result = manager->load(manager->fs_memory) == SC_FS_MEMORY_OK;
  return sc_memory_result && sc_fs_memory_result;
}

sc_bool _sc_fs_memory_save_sc_memory_segments(sc_segment ** segments, sc_uint32 segments_num)
{
  sc_fs_memory_info("Save sc-memory segments");

  // create temporary file
  sc_char * tmp_filename;
  sc_io_channel * segments_channel = sc_fs_new_tmp_write_channel(manager->fs_memory->path, &tmp_filename, "segments");
  sc_io_channel_set_encoding(segments_channel, null_ptr, null_ptr);

  manager->header.size = 0;
  manager->header.version = sc_version_to_int(&manager->version);
  manager->header.timestamp = g_get_real_time();
  if (sc_fs_memory_header_write(segments_channel, manager->header) != SC_FS_MEMORY_OK)
    goto error;

  sc_uint64 written_bytes;
  if (sc_io_channel_write_chars(
          segments_channel, (sc_char *)&segments_num, sizeof(sc_uint32), &written_bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      written_bytes != sizeof(sc_uint32))
  {
    sc_fs_memory_error("Error while attribute `segments_num` writing");
    goto error;
  }

  for (sc_uint32 idx = 0; idx < segments_num; ++idx)
  {
    sc_segment const * segment = segments[idx];

    if (segment == null_ptr ||
        sc_io_channel_write_chars(
            segments_channel, (sc_char *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &written_bytes, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        written_bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
    {
      sc_fs_memory_error("Error while attribute `segment->elements` writing");
      goto error;
    }

    if (sc_io_channel_write_chars(
            segments_channel, (sc_char *)&segment->last_element_offset, sizeof(sc_uint32), &written_bytes, null_ptr) !=
            SC_FS_IO_STATUS_NORMAL ||
        written_bytes != sizeof(sc_uint32))
    {
      sc_fs_memory_error("Error while attribute `segment->last_element_offset` writing");
      goto error;
    }
  }

  // rename main file
  if (sc_fs_is_file(tmp_filename))
  {
    if (g_rename(tmp_filename, manager->segments_path) != 0)
    {
      sc_fs_memory_error("Can't rename %s -> %s", tmp_filename, manager->segments_path);
      goto error;
    }
  }

  sc_mem_free(tmp_filename);
  sc_io_channel_shutdown(segments_channel, SC_TRUE, null_ptr);
  sc_fs_memory_info("Sc-memory segments saved");
  return SC_TRUE;

error:
{
  sc_mem_free(tmp_filename);
  sc_io_channel_shutdown(segments_channel, SC_TRUE, null_ptr);
  return SC_FALSE;
}
}

sc_bool sc_fs_memory_save(sc_segment ** segments, sc_uint32 const segments_num)
{
  if (manager->path == null_ptr)
  {
    sc_fs_memory_error("Repo path is empty to save memory");
    return SC_FALSE;
  }

  sc_bool sc_memory_result = _sc_fs_memory_save_sc_memory_segments(segments, segments_num);
  sc_bool sc_fs_memory_result = manager->save(manager->fs_memory) == SC_FS_MEMORY_OK;
  return sc_memory_result && sc_fs_memory_result;
}
