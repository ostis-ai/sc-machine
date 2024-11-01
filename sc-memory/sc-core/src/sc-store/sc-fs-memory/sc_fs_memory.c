/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fs_memory.h"
#include "sc_fs_memory_builder.h"

#include "sc_file_system.h"
#include "sc_dictionary_fs_memory_private.h"

#include "sc-store/sc_segment.h"
#include "sc-store/sc_storage_private.h"

#include "sc_io.h"

sc_fs_memory_manager * manager;

sc_fs_memory_status sc_fs_memory_initialize_ext(sc_memory_params const * params)
{
  manager = sc_fs_memory_build();
  manager->version = params->version;
  manager->path = params->binaries;

  if (manager->path == null_ptr)
  {
    sc_fs_memory_error("Empty repo path to initialize memory");
    return SC_FS_MEMORY_NO;
  }

  static sc_char const * segments_postfix = "segments" SC_FS_EXT;
  sc_fs_concat_path(manager->path, segments_postfix, &manager->segments_path);

  if (manager->initialize(&manager->fs_memory, params) != SC_FS_MEMORY_OK)
    return SC_FS_MEMORY_NO;

  // clear repository if it needs
  if (params->clear == SC_TRUE)
  {
    sc_fs_memory_info("Clear sc-memory segments");
    if (sc_fs_remove_file(manager->segments_path) == SC_FALSE)
      sc_fs_memory_info("Can't remove segments file: %s", manager->segments_path);
  }

  return SC_FS_MEMORY_OK;
}

sc_fs_memory_status sc_fs_memory_initialize(sc_char const * path, sc_bool clear)
{
  sc_memory_params * params = _sc_dictionary_fs_memory_get_default_params(path, clear);
  sc_fs_memory_status const status = sc_fs_memory_initialize_ext(params);
  sc_mem_free(params);
  return status;
}

sc_fs_memory_status sc_fs_memory_shutdown()
{
  sc_fs_memory_status const result = manager->shutdown(manager->fs_memory);
  sc_mem_free(manager->segments_path);
  sc_mem_free(manager);
  return result;
}

sc_fs_memory_status sc_fs_memory_link_string(
    sc_addr_hash const link_hash,
    sc_char const * string,
    sc_uint32 const string_size)
{
  return manager->link_string(manager->fs_memory, link_hash, string, string_size, SC_TRUE);
}

sc_fs_memory_status sc_fs_memory_link_string_ext(
    sc_addr_hash const link_hash,
    sc_char const * string,
    sc_uint32 const string_size,
    sc_bool is_searchable_string)
{
  return manager->link_string(manager->fs_memory, link_hash, string, string_size, is_searchable_string);
}

sc_fs_memory_status sc_fs_memory_get_string_by_link_hash(
    sc_addr_hash const link_hash,
    sc_char ** string,
    sc_uint32 * string_size)
{
  sc_uint64 size;
  sc_fs_memory_status result = manager->get_string_by_link_hash(manager->fs_memory, link_hash, string, &size);
  *string_size = size;
  return result;
}

sc_fs_memory_status sc_fs_memory_get_link_hashes_by_string(
    sc_char const * string,
    sc_uint32 const string_size,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  return manager->get_link_hashes_by_string(manager->fs_memory, string, string_size, data, callback);
}

sc_fs_memory_status sc_fs_memory_get_link_hashes_by_substring(
    sc_char const * substring,
    sc_uint32 const substring_size,
    sc_uint32 const max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  return manager->get_link_hashes_by_substring(
      manager->fs_memory, substring, substring_size, max_length_to_search_as_prefix, data, callback);
}

sc_fs_memory_status sc_fs_memory_get_strings_by_substring(
    sc_char const * substring,
    sc_uint32 const substring_size,
    sc_uint32 const max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  return manager->get_strings_by_substring(
      manager->fs_memory, substring, substring_size, max_length_to_search_as_prefix, data, callback);
}

sc_fs_memory_status sc_fs_memory_unlink_string(sc_addr_hash link_hash)
{
  return manager->unlink_string(manager->fs_memory, link_hash);
}

// read, write and save methods
sc_fs_memory_status _sc_fs_memory_load_sc_memory_segments(sc_storage * storage)
{
  if (sc_fs_is_file(manager->segments_path) == SC_FALSE)
  {
    storage->segments_count = 0;
    sc_fs_memory_info("There are no sc-memory segments in %s", manager->segments_path);
    return SC_FS_MEMORY_OK;
  }

  // open segments
  sc_io_channel * segments_channel = sc_io_new_read_channel(manager->segments_path, null_ptr);
  sc_io_channel_set_encoding(segments_channel, null_ptr, null_ptr);

  if (sc_fs_memory_header_read(segments_channel, &manager->header) != SC_FS_MEMORY_OK)
    goto error;
  storage->segments_count = manager->header.size;

  // backward compatibility with version 0.7.0
  sc_uint64 read_bytes = 0;
  sc_bool is_no_deprecated_segments = storage->segments_count == 0;
  if (is_no_deprecated_segments)
    sc_fs_memory_info("Load sc-memory segments from %s", manager->segments_path);
  else
    sc_fs_memory_warning("Load deprecated sc-memory segments from %s", manager->segments_path);

  static sc_uint32 const OLD_SC_ELEMENT_SIZE = 36;
  sc_uint32 element_size = is_no_deprecated_segments ? sizeof(sc_element) : OLD_SC_ELEMENT_SIZE;
  if (is_no_deprecated_segments)
  {
    if (sc_io_channel_read_chars(
            segments_channel, (sc_char *)&storage->segments_count, sizeof(sc_addr_seg), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || read_bytes != sizeof(sc_addr_seg))
    {
      storage->segments_count = 0;
      sc_fs_memory_error("Error while attribute `storage->segments_count` reading");
      goto error;
    }

    if (sc_io_channel_read_chars(
            segments_channel,
            (sc_char *)&storage->last_not_engaged_segment_num,
            sizeof(sc_addr_seg),
            &read_bytes,
            null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || read_bytes != sizeof(sc_addr_seg))
    {
      storage->last_not_engaged_segment_num = 0;
      sc_fs_memory_error("Error while attribute `storage->last_not_engaged_segment_num` reading");
      goto error;
    }

    if (sc_io_channel_read_chars(
            segments_channel,
            (sc_char *)&storage->last_released_segment_num,
            sizeof(sc_addr_seg),
            &read_bytes,
            null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || read_bytes != sizeof(sc_addr_seg))
    {
      storage->last_released_segment_num = 0;
      sc_fs_memory_error("Error while attribute `storage->last_released_segment_num` reading");
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

  for (sc_addr_seg i = 0; i < storage->segments_count; ++i)
  {
    sc_addr_seg const num = i;
    sc_segment * seg = sc_segment_new(i + 1);
    storage->segments[i] = seg;

    for (sc_addr_seg j = 0; j < SC_SEGMENT_ELEMENTS_COUNT; ++j)
    {
      if (sc_io_channel_read_chars(segments_channel, (sc_char *)&seg->elements[j], element_size, &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || read_bytes != element_size)
      {
        storage->segments_count = num;
        sc_fs_memory_error("Error while sc-element %d in sc-segment %d reading", j, i);
        goto error;
      }

      // needed for sc-template search
      if (!is_no_deprecated_segments)
      {
        seg->elements[j].incoming_arcs_count = 1;
        seg->elements[j].outgoing_arcs_count = 1;
      }
    }

    if (is_no_deprecated_segments)
    {
      if (sc_io_channel_read_chars(
              segments_channel, (sc_char *)&seg->last_engaged_offset, sizeof(sc_addr_offset), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || read_bytes != sizeof(sc_addr_offset))
      {
        sc_fs_memory_error("Error while sc-segment %d reading", i);
        goto error;
      }

      if (sc_io_channel_read_chars(
              segments_channel, (sc_char *)&seg->last_released_offset, sizeof(sc_addr_offset), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || read_bytes != sizeof(sc_addr_offset))
      {
        sc_fs_memory_error("Error while sc-segment %d reading", i);
        goto error;
      }
    }

    i = num;
  }

  sc_io_channel_shutdown(segments_channel, SC_FALSE, null_ptr);

  sc_message("\tLoaded segments count: %d", storage->segments_count);
  sc_message("\tSc-segments size: %ld", storage->segments_count * sizeof(sc_segment));
  sc_message("\tLast not engaged segment num: %d", storage->last_not_engaged_segment_num);
  sc_message("\tLast released segment num: %d", storage->last_released_segment_num);

  if (is_no_deprecated_segments)
    sc_fs_memory_info("Sc-memory segments loaded");
  else
    sc_fs_memory_warning("Deprecated sc-memory segments loaded");

  return SC_FS_MEMORY_OK;

error:
{
  sc_io_channel_shutdown(segments_channel, SC_FALSE, null_ptr);
  return SC_FS_MEMORY_READ_ERROR;
}
}

sc_fs_memory_status sc_fs_memory_load(sc_storage * storage)
{
  if (_sc_fs_memory_load_sc_memory_segments(storage) != SC_FS_MEMORY_OK)
    return SC_FS_MEMORY_READ_ERROR;
  if (manager->load(manager->fs_memory) != SC_FS_MEMORY_OK)
    return SC_FS_MEMORY_READ_ERROR;

  return SC_FS_MEMORY_OK;
}

sc_fs_memory_status _sc_fs_memory_save_sc_memory_segments(sc_storage * storage)
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
          segments_channel, (sc_char *)&storage->segments_count, sizeof(sc_addr_seg), &written_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || written_bytes != sizeof(sc_addr_seg))
  {
    sc_fs_memory_error("Error while attribute `storage->segments_count` writing");
    goto error;
  }

  if (sc_io_channel_write_chars(
          segments_channel,
          (sc_char *)&storage->last_not_engaged_segment_num,
          sizeof(sc_addr_seg),
          &written_bytes,
          null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || written_bytes != sizeof(sc_addr_seg))
  {
    sc_fs_memory_error("Error while attribute `storage->last_not_engaged_segment_num` writing");
    goto error;
  }

  if (sc_io_channel_write_chars(
          segments_channel,
          (sc_char *)&storage->last_released_segment_num,
          sizeof(sc_addr_seg),
          &written_bytes,
          null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || written_bytes != sizeof(sc_addr_seg))
  {
    sc_fs_memory_error("Error while attribute `storage->last_released_segment_num` writing");
    goto error;
  }

  for (sc_addr_seg idx = 0; idx < storage->segments_count; ++idx)
  {
    sc_segment * segment = storage->segments[idx];
    if (segment == null_ptr)
    {
      sc_fs_memory_error("Error while attribute `segment` writing");
      goto error;
    }

    sc_monitor_acquire_read(&segment->monitor);

    if (sc_io_channel_write_chars(
            segments_channel, (sc_char *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || written_bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
    {
      sc_fs_memory_error("Error while attribute `segment->elements` writing");
      goto segment_save_error;
    }

    if (sc_io_channel_write_chars(
            segments_channel,
            (sc_char *)&segment->last_engaged_offset,
            sizeof(sc_addr_offset),
            &written_bytes,
            null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || written_bytes != sizeof(sc_addr_offset))
    {
      sc_fs_memory_error("Error while attribute `segment->last_engaged_offset` writing");
      goto segment_save_error;
    }

    if (sc_io_channel_write_chars(
            segments_channel,
            (sc_char *)&segment->last_released_offset,
            sizeof(sc_addr_offset),
            &written_bytes,
            null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || written_bytes != sizeof(sc_addr_offset))
    {
      sc_fs_memory_error("Error while attribute `segment->last_released_offset` writing");
      goto segment_save_error;
    }

  segment_save_error:
    sc_monitor_release_read(&segment->monitor);
  }

  // rename main file
  if (sc_fs_is_file(tmp_filename))
  {
    if (sc_fs_rename_file(tmp_filename, manager->segments_path) == SC_FALSE)
    {
      sc_fs_memory_error("Can't rename %s -> %s", tmp_filename, manager->segments_path);
      goto error;
    }
  }

  sc_message("\tLoaded segments count: %d", storage->segments_count);
  sc_message("\tSc-segments size: %ld", storage->segments_count * sizeof(sc_segment));
  sc_message("\tLast not engaged segment num: %d", storage->last_not_engaged_segment_num);
  sc_message("\tLast released segment num: %d", storage->last_released_segment_num);

  sc_mem_free(tmp_filename);
  sc_io_channel_shutdown(segments_channel, SC_TRUE, null_ptr);
  sc_fs_memory_info("Sc-memory segments saved");
  return SC_FS_MEMORY_OK;

error:
{
  sc_mem_free(tmp_filename);
  sc_io_channel_shutdown(segments_channel, SC_TRUE, null_ptr);
  return SC_FS_MEMORY_WRITE_ERROR;
}
}

sc_fs_memory_status sc_fs_memory_save(sc_storage * storage)
{
  if (manager->path == null_ptr)
  {
    sc_fs_memory_error("Repo path is empty to save memory");
    return SC_FS_MEMORY_NO;
  }

  if (_sc_fs_memory_save_sc_memory_segments(storage) != SC_FS_MEMORY_OK)
    return SC_FS_MEMORY_WRITE_ERROR;
  if (manager->save(manager->fs_memory) != SC_FS_MEMORY_OK)
    return SC_FS_MEMORY_WRITE_ERROR;

  return SC_FS_MEMORY_OK;
}
