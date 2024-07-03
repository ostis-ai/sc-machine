/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifdef SC_DICTIONARY_FS_MEMORY

#  include "sc_dictionary_fs_memory.h"
#  include "sc_dictionary_fs_memory_private.h"

#  include "../sc-base/sc_allocator.h"
#  include "../sc-container/sc-string/sc_string.h"

#  include "sc_file_system.h"
#  include "sc_io.h"

#  define DEFAULT_STRING_INT_SIZE 20
#  define DEFAULT_MAX_SEARCHABLE_STRING_SIZE 1000

typedef struct
{
  sc_list * link_hashes;
  sc_uint64 string_offset;
} sc_link_hash_content;

sc_io_channel * _sc_dictionary_fs_memory_get_strings_channel_by_offset(
    sc_dictionary_fs_memory * memory,
    sc_uint64 strings_offset,
    sc_monitor ** channel_monitor)
{
  *channel_monitor = null_ptr;

  sc_uint64 const idx = strings_offset / memory->max_strings_channel_size;
  if (idx >= memory->max_strings_channels)
  {
    sc_fs_memory_info(
        "Max strings channels is %d. File memory is full. Please extends or swap file memory",
        memory->max_strings_channels);
    return null_ptr;
  }

  sc_monitor_acquire_read(&memory->monitor);
  sc_io_channel * channel = memory->strings_channels[idx];
  sc_monitor_release_read(&memory->monitor);
  if (channel != null_ptr)
  {
    *channel_monitor =
        sc_monitor_table_get_monitor_from_table(&memory->strings_channels_monitors_table, (sc_pointer)idx);
    return channel;
  }

  sc_monitor_acquire_read(&memory->monitor);
  if (idx > 0 && memory->strings_channels[idx - 1] != null_ptr)
    sc_io_channel_flush(memory->strings_channels[idx - 1], null_ptr);
  sc_monitor_release_read(&memory->monitor);

  sc_char strings_channel_number[DEFAULT_STRING_INT_SIZE];
  {
    sc_uint64 strings_channel_number_size;
    sc_int_to_str_int(idx + 1, strings_channel_number, strings_channel_number_size);
    (void)strings_channel_number_size;
  }
  static sc_char const * strings_postfix = "strings";
  sc_char * strings_channel_name;
  {
    sc_str_concat(strings_postfix, strings_channel_number, strings_channel_name);
  }
  sc_char * strings_path;
  sc_fs_concat_path_ext(memory->path, strings_channel_name, SC_FS_EXT, &strings_path);
  sc_mem_free(strings_channel_name);

  sc_bool is_path = sc_fs_is_file(strings_path);

  sc_monitor_acquire_write(&memory->monitor);

  if (is_path == SC_FALSE || memory->clear == SC_TRUE)
    memory->strings_channels[idx] = sc_io_new_write_channel(strings_path, null_ptr);
  else
    memory->strings_channels[idx] = sc_io_new_append_channel(strings_path, null_ptr);
  sc_io_channel_set_encoding(memory->strings_channels[idx], null_ptr, null_ptr);

  sc_monitor_release_write(&memory->monitor);
  *channel_monitor = sc_monitor_table_get_monitor_from_table(&memory->strings_channels_monitors_table, (sc_pointer)idx);

  sc_mem_free(strings_path);

  sc_monitor_acquire_read(&memory->monitor);
  channel = memory->strings_channels[idx];
  sc_monitor_release_read(&memory->monitor);

  return channel;
}

sc_uint64 _sc_dictionary_fs_memory_normalize_offset(sc_dictionary_fs_memory const * memory, sc_uint64 strings_offset)
{
  sc_uint64 const channel_idx = strings_offset / memory->max_strings_channel_size;
  return strings_offset - memory->max_strings_channel_size * channel_idx;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize_ext(
    sc_dictionary_fs_memory ** memory,
    sc_memory_params const * params)
{
  sc_char const * path = params->repo_path;
  sc_fs_memory_info("Initialize");
  if (path == null_ptr)
  {
    sc_fs_memory_info("Path is empty");
    goto error;
  }

  if (sc_fs_is_directory(path) == SC_FALSE)
  {
    if (sc_fs_create_directory(path) == SC_FALSE)
    {
      sc_fs_memory_error("Path `%s` is not correct", path);
      goto error;
    }
  }
  else if (params->clear)
  {
    sc_fs_remove_directory_ext(path, SC_FALSE);
  }

  *memory = sc_mem_new(sc_dictionary_fs_memory, 1);
  {
    {
      sc_str_cpy((*memory)->path, params->repo_path, sc_str_len(path));
      (*memory)->clear = params->clear;
    }
    {
      (*memory)->max_strings_channels = sc_boundary(params->max_strings_channels, 1, SC_MAXINT16);
      (*memory)->max_strings_channel_size = sc_boundary(params->max_strings_channel_size, 1000, SC_MAXUINT32);
      (*memory)->max_searchable_string_size = sc_boundary(params->max_searchable_string_size, 10, 100000);
      (*memory)->term_separators = params->term_separators;
      (*memory)->search_by_substring = params->search_by_substring;
    }
    {
      _sc_uchar_dictionary_initialize(&(*memory)->terms_string_offsets_dictionary);
      static sc_char const * term_string_offsets = "term_string_offsets" SC_FS_EXT;
      sc_fs_concat_path((*memory)->path, term_string_offsets, &(*memory)->terms_string_offsets_path);

      (*memory)->strings_channels = (void **)sc_mem_new(sc_io_channel *, (*memory)->max_strings_channels);
      _sc_monitor_table_init(&(*memory)->strings_channels_monitors_table);
      (*memory)->last_string_offset = 0;
      sc_monitor_init(&(*memory)->monitor);
    }

    _sc_number_dictionary_initialize(&(*memory)->link_hashes_string_offsets_dictionary);
    _sc_number_dictionary_initialize(&(*memory)->string_offsets_link_hashes_dictionary);
    static sc_char const * string_offsets_link_hashes = "string_offsets_link_hashes" SC_FS_EXT;
    sc_fs_concat_path((*memory)->path, string_offsets_link_hashes, &(*memory)->string_offsets_link_hashes_path);
  }
  sc_fs_memory_info("Configuration:");
  sc_message("\tSc-dictionary node size: %zd", sizeof(sc_dictionary_node));
  sc_message("\tSc-dictionary size: %zd", sizeof(sc_dictionary));
  sc_message("\tSc-fs-memory size: %zd", sizeof(sc_dictionary_fs_memory));
  sc_message("\tRepo path: %s", (*memory)->path);
  sc_message("\tClean on initialize: %s", (*memory)->clear ? "On" : "Off");

  sc_fs_memory_info("Index configuration:");
  sc_message("\tMax strings channels: %d", (*memory)->max_strings_channels);
  sc_message("\tMax strings channel size: %d", (*memory)->max_strings_channel_size);
  sc_message("\tMax searchable string size: %d", (*memory)->max_searchable_string_size);
  sc_message("\tTerm separators: \"%s\"", (*memory)->term_separators);

  sc_fs_memory_info("Successfully initialized");

  return SC_FS_MEMORY_OK;

error:
{
  if (memory != null_ptr)
    *memory = null_ptr;
  sc_fs_memory_info("Initialized with errors");
  return SC_FS_MEMORY_WRONG_PATH;
}
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize(
    sc_dictionary_fs_memory ** memory,
    sc_char const * path)
{
  sc_memory_params * params = _sc_dictionary_fs_memory_get_default_params(path, SC_FALSE);
  sc_dictionary_fs_memory_status const status = sc_dictionary_fs_memory_initialize_ext(memory, params);
  sc_mem_free(params);
  return status;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_shutdown(sc_dictionary_fs_memory * memory)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to shutdown");
    return SC_FS_MEMORY_NO;
  }

  sc_fs_memory_info("Shutdown");
  {
    sc_mem_free(memory->path);

    {
      sc_dictionary_destroy(memory->terms_string_offsets_dictionary, _sc_dictionary_fs_memory_node_clear);
      sc_mem_free(memory->terms_string_offsets_path);

      for (sc_uint64 i = 0; i < memory->max_strings_channels && memory->strings_channels[i] != null_ptr; ++i)
      {
        sc_io_channel_shutdown(memory->strings_channels[i], SC_TRUE, null_ptr);
      }
      sc_mem_free(memory->strings_channels);
      _sc_monitor_table_destroy(&memory->strings_channels_monitors_table);
      sc_monitor_destroy(&memory->monitor);
    }

    sc_dictionary_destroy(memory->link_hashes_string_offsets_dictionary, _sc_dictionary_fs_memory_string_node_clear);
    sc_dictionary_destroy(memory->string_offsets_link_hashes_dictionary, _sc_dictionary_fs_memory_link_node_clear);
    sc_mem_free(memory->string_offsets_link_hashes_path);
  }
  sc_mem_free(memory);

  sc_fs_memory_info("Successfully shutdown");
  return SC_FS_MEMORY_OK;
}

void _sc_dictionary_fs_memory_append(
    sc_dictionary * dictionary,
    sc_char const * key,
    sc_uint64 const key_size,
    void * data)
{
  sc_list * list = sc_dictionary_get_by_key(dictionary, key, key_size);
  if (list == null_ptr)
  {
    sc_char * copied_key;
    sc_str_cpy(copied_key, key, key_size);

    sc_list_init(&list);
    sc_dictionary_append(dictionary, copied_key, key_size, list);
    sc_list_push_back(list, copied_key);
  }

  sc_list_push_back(list, data);
}

sc_bool _sc_addr_hash_compare(void * addr_hash, void * other_addr_hash)
{
  return addr_hash == other_addr_hash;
}

void _sc_dictionary_fs_memory_append_link_string_unique(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash const link_hash,
    sc_uint64 const string_offset)
{
  sc_char link_hash_str[DEFAULT_STRING_INT_SIZE];
  sc_uint64 link_hash_str_size;
  sc_bool is_content_new;
  sc_link_hash_content * content;
  {
    sc_int_to_str_int(link_hash, link_hash_str, link_hash_str_size);
    content =
        sc_dictionary_get_by_key(memory->link_hashes_string_offsets_dictionary, link_hash_str, link_hash_str_size);
    is_content_new = (content == null_ptr);
    if (is_content_new)
    {
      content = sc_mem_new(sc_link_hash_content, 1);
      sc_dictionary_append(memory->link_hashes_string_offsets_dictionary, link_hash_str, link_hash_str_size, content);
    }
  }

  sc_char string_offset_str[DEFAULT_STRING_INT_SIZE];
  sc_uint64 string_offset_str_size;
  sc_list * link_hashes;
  {
    sc_int_to_str_int(string_offset, string_offset_str, string_offset_str_size);
    link_hashes = sc_dictionary_get_by_key(
        memory->string_offsets_link_hashes_dictionary, string_offset_str, string_offset_str_size);
    if (link_hashes == null_ptr)
    {
      sc_list_init(&link_hashes);
      sc_dictionary_append(
          memory->string_offsets_link_hashes_dictionary, string_offset_str, string_offset_str_size, link_hashes);
    }
  }

  {
    if (!is_content_new && content->link_hashes != link_hashes)
      sc_list_remove_if(content->link_hashes, (void *)link_hash, _sc_addr_hash_compare);

    if (content->link_hashes != link_hashes)
    {
      content->string_offset = string_offset + 1;
      content->link_hashes = link_hashes;
      sc_list_push_back(content->link_hashes, (void *)link_hash);
    }
  }
}

sc_list * _sc_dictionary_fs_memory_get_string_offsets_by_term(
    sc_dictionary_fs_memory const * memory,
    sc_char const * term)
{
  sc_uint64 const term_size = sc_str_len(term);
  return sc_dictionary_get_by_key(memory->terms_string_offsets_dictionary, term, term_size);
}

sc_dictionary_fs_memory_status _sc_dictionary_node_fs_memory_get_string_offset_by_string(
    sc_dictionary_fs_memory * memory,
    sc_io_channel * strings_channel,
    sc_monitor * channel_monitor,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_list const * string_offsets,
    sc_uint64 * found_string_offset)
{
  sc_iterator * string_offset_it = sc_list_iterator(string_offsets);
  if (!sc_iterator_next(string_offset_it))
  {
    sc_iterator_destroy(string_offset_it);
    return SC_FS_MEMORY_READ_ERROR;
  }

  while (sc_iterator_next(string_offset_it))
  {
    sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(string_offset_it);

    // read string with size from fs-memory
    sc_uint64 read_bytes;
    sc_uint64 const normalized_string_offset = _sc_dictionary_fs_memory_normalize_offset(memory, string_offset);
    sc_io_channel_seek(strings_channel, normalized_string_offset, SC_FS_IO_SEEK_SET, null_ptr);
    {
      sc_uint64 other_string_size;
      if (sc_io_channel_read_chars(
              strings_channel, (sc_char *)&other_string_size, sizeof(sc_uint64), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || sizeof(sc_uint64) != read_bytes)
        goto error;

      if (other_string_size != string_size)
        continue;

      sc_char other_string[other_string_size + 1];
      if (sc_io_channel_read_chars(strings_channel, other_string, other_string_size, &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || other_string_size != read_bytes)
        goto error;
      other_string[other_string_size] = '\0';

      if (sc_str_cmp(string, other_string) == SC_FALSE)
        continue;
    }

    *found_string_offset = string_offset;
    break;
  }

  sc_iterator_destroy(string_offset_it);
  return SC_FS_MEMORY_OK;

error:
  sc_iterator_destroy(string_offset_it);
  return SC_FS_MEMORY_READ_ERROR;
}

sc_uint64 _sc_dictionary_fs_memory_get_string_offset_by_string(
    sc_dictionary_fs_memory * memory,
    sc_io_channel * strings_channel,
    sc_monitor * channel_monitor,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_char const * term)
{
  sc_list * string_offsets = _sc_dictionary_fs_memory_get_string_offsets_by_term(memory, term);

  sc_uint64 string_offset = INVALID_STRING_OFFSET;
  _sc_dictionary_node_fs_memory_get_string_offset_by_string(
      memory, strings_channel, channel_monitor, string, string_size, string_offsets, &string_offset);
  return string_offset;
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_write_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash const link_hash,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_list const * string_terms,
    sc_bool is_searchable_string,
    sc_uint64 * string_offset,
    sc_bool * is_not_exist)
{
  sc_monitor * channel_monitor;
  sc_io_channel * strings_channel =
      _sc_dictionary_fs_memory_get_strings_channel_by_offset(memory, memory->last_string_offset, &channel_monitor);
  *string_offset = INVALID_STRING_OFFSET;
  if (strings_channel == null_ptr)
    return SC_FS_MEMORY_WRITE_ERROR;

  sc_monitor_acquire_write(&memory->monitor);
  sc_monitor_acquire_write(channel_monitor);
  // find string if it exists in fs-memory
  if (is_searchable_string)
  {
    *string_offset = _sc_dictionary_fs_memory_get_string_offset_by_string(
        memory, strings_channel, channel_monitor, string, string_size, string_terms->begin->data);
  }

  *is_not_exist = (*string_offset == INVALID_STRING_OFFSET);
  // save string in fs-memory
  if (*is_not_exist)
  {
    *string_offset = memory->last_string_offset;

    sc_uint64 const normalized_string_offset = _sc_dictionary_fs_memory_normalize_offset(memory, *string_offset);
    sc_io_channel_seek(strings_channel, normalized_string_offset, SC_FS_IO_SEEK_SET, null_ptr);

    sc_uint64 written_bytes = 0;
    if (sc_io_channel_write_chars(strings_channel, &string_size, sizeof(string_size), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(string_size) != written_bytes)
    {
      sc_fs_memory_error("Error while attribute `size` writing");
      goto error;
    }

    memory->last_string_offset += written_bytes;

    if (sc_io_channel_write_chars(strings_channel, string, string_size, &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || string_size != written_bytes)
    {
      sc_fs_memory_error("Error while attribute `string` writing");
      goto error;
    }

    memory->last_string_offset += written_bytes;
  }

  sc_monitor_release_write(channel_monitor);
  sc_monitor_release_write(&memory->monitor);
  return SC_FS_MEMORY_OK;

error:
  sc_monitor_release_write(channel_monitor);
  sc_monitor_release_write(&memory->monitor);
  return SC_FS_MEMORY_WRITE_ERROR;
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_write_string_terms_string_offset(
    sc_dictionary_fs_memory * memory,
    sc_uint64 const string_offset,
    sc_list * string_terms)
{
  sc_iterator * term_it = sc_list_iterator(string_terms);
  while (sc_iterator_next(term_it))
  {
    sc_char * term = sc_iterator_get(term_it);
    sc_uint64 const term_size = sc_str_len(term);

    // cache term offset in fs-memory
    {
      _sc_dictionary_fs_memory_append(memory->terms_string_offsets_dictionary, term, term_size, (void *)string_offset);
    }

    if (!memory->search_by_substring)
      break;
  }
  sc_iterator_destroy(term_it);

  return SC_FS_MEMORY_OK;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_link_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash const link_hash,
    sc_char const * string,
    sc_uint64 const string_size)
{
  return sc_dictionary_fs_memory_link_string_ext(memory, link_hash, string, string_size, SC_TRUE);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_link_string_ext(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash const link_hash,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_bool is_searchable_string)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to link string");
    return SC_FS_MEMORY_NO;
  }

  is_searchable_string &= string_size < memory->max_searchable_string_size;
  sc_list * string_terms = null_ptr;
  // don't divide into terms big strings if you don't need to search them
  if (is_searchable_string)
    string_terms = _sc_dictionary_fs_memory_get_string_terms(string, memory->term_separators);

  sc_bool is_not_exist = SC_TRUE;
  sc_uint64 string_offset;
  sc_dictionary_fs_memory_status status = _sc_dictionary_fs_memory_write_string(
      memory, link_hash, string, string_size, string_terms, is_searchable_string, &string_offset, &is_not_exist);
  if (status != SC_FS_MEMORY_OK)
    return status;

  // cache string offset and link hash data
  {
    _sc_dictionary_fs_memory_append_link_string_unique(memory, link_hash, string_offset);
  }

  if (is_searchable_string && is_not_exist)
    status = _sc_dictionary_fs_memory_write_string_terms_string_offset(memory, string_offset, string_terms);
  sc_list_clear(string_terms);
  sc_list_destroy(string_terms);

  return status;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unlink_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash const link_hash)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to unlink string");
    return SC_FS_MEMORY_NO;
  }

  sc_monitor_acquire_write(&memory->monitor);

  sc_char link_hash_str[DEFAULT_STRING_INT_SIZE];
  sc_uint64 link_hash_str_size;
  sc_int_to_str_int(link_hash, link_hash_str, link_hash_str_size);

  // remove link for current string
  {
    sc_link_hash_content * link_hash_content =
        sc_dictionary_get_by_key(memory->link_hashes_string_offsets_dictionary, link_hash_str, link_hash_str_size);
    if (link_hash_content == null_ptr)
      goto result;

    sc_list_remove_if(link_hash_content->link_hashes, (void *)link_hash, _sc_addr_hash_compare);
    sc_mem_free(link_hash_content);
  }

  // set empty link
  sc_dictionary_append(memory->link_hashes_string_offsets_dictionary, link_hash_str, link_hash_str_size, null_ptr);

result:
  sc_monitor_release_write(&memory->monitor);

  return SC_FS_MEMORY_OK;
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_read_string_by_offset(
    sc_dictionary_fs_memory * memory,
    sc_uint64 const string_offset,
    sc_char ** string)
{
  sc_monitor * channel_monitor;
  sc_io_channel * strings_channel =
      _sc_dictionary_fs_memory_get_strings_channel_by_offset(memory, string_offset, &channel_monitor);
  if (strings_channel == null_ptr)
  {
    sc_fs_memory_error("Path `%s` doesn't exist", "path");
    return SC_FS_MEMORY_READ_ERROR;
  }

  // read string with size from fs-memory
  sc_uint64 read_bytes;
  sc_uint64 const normalized_string_offset = _sc_dictionary_fs_memory_normalize_offset(memory, string_offset);
  sc_monitor_acquire_write(channel_monitor);
  sc_io_channel_seek(strings_channel, normalized_string_offset, SC_FS_IO_SEEK_SET, null_ptr);
  {
    sc_uint64 string_size;
    if (sc_io_channel_read_chars(strings_channel, (sc_char *)&string_size, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != read_bytes)
    {
      *string = null_ptr;
      goto error;
    }

    *string = sc_mem_new(sc_char, string_size + 1);
    if (sc_io_channel_read_chars(strings_channel, *string, string_size, &read_bytes, null_ptr) != SC_FS_IO_STATUS_NORMAL
        || string_size != read_bytes)
    {
      sc_mem_free(*string);
      *string = null_ptr;
      goto error;
    }
  }

  sc_monitor_release_write(channel_monitor);
  return SC_FS_MEMORY_OK;

error:
  sc_monitor_release_write(channel_monitor);
  return SC_FS_MEMORY_READ_ERROR;
}

void _sc_dictionary_fs_memory_read_file(sc_char * file_path, sc_char ** content, sc_uint32 * size)
{
  if (sc_fs_is_binary_file(file_path))
  {
    sc_char * data;
    sc_fs_get_file_content(file_path, &data, size);
    *content = g_base64_encode((sc_uchar *)data, *size);
    sc_mem_free(data);
  }
  else
    sc_fs_get_file_content(file_path, content, size);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_string_by_link_hash(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash const link_hash,
    sc_char ** string,
    sc_uint64 * string_size)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to get string by link hash");
    return SC_FS_MEMORY_NO;
  }

  sc_char link_hash_str[DEFAULT_STRING_INT_SIZE];
  sc_uint64 link_hash_str_size;
  sc_int_to_str_int(link_hash, link_hash_str, link_hash_str_size);
  sc_link_hash_content * content =
      sc_dictionary_get_by_key(memory->link_hashes_string_offsets_dictionary, link_hash_str, link_hash_str_size);

  if (content == null_ptr)
  {
    *string = null_ptr;
    *string_size = 0;
    return SC_FS_MEMORY_NO_STRING;
  }

  sc_uint64 const string_offset = (sc_uint64)content->string_offset - 1;
  sc_dictionary_fs_memory_status const status =
      _sc_dictionary_fs_memory_read_string_by_offset(memory, string_offset, string);
  if (status != SC_FS_MEMORY_OK)
  {
    *string = null_ptr;
    *string_size = 0;
    return SC_FS_MEMORY_READ_ERROR;
  }

  if ((sc_str_find(*string, ".") || sc_str_find(*string, "/")) && sc_fs_is_file(*string))
  {
    sc_char * file_path = *string;
    _sc_dictionary_fs_memory_read_file(file_path, string, (sc_uint32 *)string_size);
    sc_mem_free(file_path);
  }

  *string_size = sc_str_len(*string);

  return SC_FS_MEMORY_OK;
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_get_link_hashes_by_string_term(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_bool const is_substring,
    sc_bool const to_search_as_prefix,
    sc_list const * string_offsets,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  sc_iterator * string_offset_it = sc_list_iterator(string_offsets);
  if (!sc_iterator_next(string_offset_it))
    return SC_FS_MEMORY_NO_STRING;

  sc_monitor * channel_monitor;
  while (sc_iterator_next(string_offset_it))
  {
    sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(string_offset_it);

    sc_io_channel * strings_channel =
        _sc_dictionary_fs_memory_get_strings_channel_by_offset(memory, string_offset, &channel_monitor);
    if (strings_channel == null_ptr)
    {
      sc_fs_memory_error("Path `%s` doesn't exist", "path");
      return SC_FS_MEMORY_READ_ERROR;
    }

    // read string with size from fs-memory
    sc_uint64 read_bytes;
    sc_uint64 const normalized_string_offset = _sc_dictionary_fs_memory_normalize_offset(memory, string_offset);

    sc_bool go_to_next = SC_FALSE;
    sc_monitor_acquire_write(channel_monitor);
    sc_io_channel_seek(strings_channel, normalized_string_offset, SC_FS_IO_SEEK_SET, null_ptr);
    {
      sc_uint64 other_string_size;
      if (sc_io_channel_read_chars(
              strings_channel, (sc_char *)&other_string_size, sizeof(sc_uint64), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || sizeof(sc_uint64) != read_bytes)
        goto error;

      // optimize needed string search
      if ((is_substring && other_string_size < string_size) || (!is_substring && other_string_size != string_size))
      {
        go_to_next = SC_TRUE;
        goto cont;
      }

      sc_char other_string[other_string_size + 1];
      if (sc_io_channel_read_chars(strings_channel, other_string, other_string_size, &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || other_string_size != read_bytes)
        goto error;

      other_string[other_string_size] = '\0';

      if ((is_substring
           && ((to_search_as_prefix && sc_str_has_prefix(other_string, string) == SC_FALSE)
               || (!to_search_as_prefix && sc_str_find(other_string, string) == SC_FALSE)))
          || (!is_substring && sc_str_cmp(string, other_string) == SC_FALSE))
      {
        go_to_next = SC_TRUE;
        goto cont;
      }
    }

  cont:
    sc_monitor_release_write(channel_monitor);
    if (go_to_next)
      continue;

    sc_char string_offset_str[DEFAULT_STRING_INT_SIZE];
    sc_uint64 string_offset_str_size;
    sc_int_to_str_int(string_offset, string_offset_str, string_offset_str_size);

    sc_list * _data = sc_dictionary_get_by_key(
        memory->string_offsets_link_hashes_dictionary, string_offset_str, string_offset_str_size);

    sc_iterator * data_it = sc_list_iterator(_data);
    while (sc_iterator_next(data_it))
    {
      sc_addr_hash link_hash = (sc_addr_hash)sc_iterator_get(data_it);
      sc_addr link_addr;
      SC_ADDR_LOCAL_FROM_INT(link_hash, link_addr);
      callback(data, link_addr);
    }
    sc_iterator_destroy(data_it);
  }
  sc_iterator_destroy(string_offset_it);

  return SC_FS_MEMORY_OK;

error:
  sc_monitor_release_write(channel_monitor);
  sc_iterator_destroy(string_offset_it);
  return SC_FS_MEMORY_READ_ERROR;
}

sc_bool _sc_dictionary_fs_memory_visit_string_offsets_by_term_prefix(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return SC_TRUE;

  sc_dictionary_fs_memory * memory = arguments[0];
  sc_list * string_offsets = arguments[1];
  sc_iterator * it = sc_list_iterator(node->data);
  if (!sc_iterator_next(it))
  {
    sc_iterator_destroy(it);
    return SC_TRUE;
  }

  while (sc_iterator_next(it))
  {
    sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(it);
    sc_char string_offset_str[DEFAULT_STRING_INT_SIZE];
    sc_uint64 string_offset_str_size;
    sc_int_to_str_int(string_offset, string_offset_str, string_offset_str_size);

    // skip strings without links
    sc_list * link_hashes = sc_dictionary_get_by_key(
        memory->string_offsets_link_hashes_dictionary, string_offset_str, string_offset_str_size);
    if (link_hashes != null_ptr && link_hashes->size != 0)
      sc_list_push_back(string_offsets, (void *)string_offset);
  }
  sc_iterator_destroy(it);

  return SC_TRUE;
}

sc_list * _sc_dictionary_fs_memory_get_string_offsets_by_term_prefix(
    sc_dictionary_fs_memory const * memory,
    sc_char const * term)
{
  sc_uint64 const term_size = sc_str_len(term);
  sc_list * string_offsets;
  sc_list_init(&string_offsets);
  sc_list_push_back(string_offsets, null_ptr);

  void * arguments[2];
  arguments[0] = (void *)memory;
  arguments[1] = string_offsets;

  sc_dictionary_get_by_key_prefix(
      memory->terms_string_offsets_dictionary,
      term,
      term_size,
      _sc_dictionary_fs_memory_visit_string_offsets_by_term_prefix,
      arguments);

  return string_offsets;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_string_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_bool const is_substring,
    sc_bool const to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to get link hashes by string");
    return SC_FS_MEMORY_NO;
  }

  sc_char * term = _sc_dictionary_fs_memory_get_first_term(string, memory->term_separators);
  sc_list * string_offsets = null_ptr;
  if (is_substring)
    string_offsets = _sc_dictionary_fs_memory_get_string_offsets_by_term_prefix(memory, term);
  else
    string_offsets = _sc_dictionary_fs_memory_get_string_offsets_by_term(memory, term);
  sc_mem_free(term);

  sc_dictionary_fs_memory_status const status = _sc_dictionary_fs_memory_get_link_hashes_by_string_term(
      memory, string, string_size, is_substring, to_search_as_prefix, string_offsets, data, callback);

  if (is_substring)
    sc_list_destroy(string_offsets);

  return status;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_string(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 const string_size,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  return sc_dictionary_fs_memory_get_link_hashes_by_string_ext(
      memory, string, string_size, SC_FALSE, SC_FALSE, data, callback);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  return sc_dictionary_fs_memory_get_link_hashes_by_string_ext(
      memory, string, string_size, SC_TRUE, string_size <= max_length_to_search_as_prefix, data, callback);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  return sc_dictionary_fs_memory_get_link_hashes_by_string_ext(
      memory, string, string_size, SC_TRUE, SC_FALSE, data, callback);
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_get_strings_by_substring_term(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_bool const to_search_as_prefix,
    sc_list const * string_offsets,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  sc_iterator * string_offset_it = sc_list_iterator(string_offsets);
  if (!sc_iterator_next(string_offset_it))
    return SC_FS_MEMORY_READ_ERROR;

  sc_monitor * channel_monitor;
  while (sc_iterator_next(string_offset_it))
  {
    sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(string_offset_it);

    sc_io_channel * strings_channel =
        _sc_dictionary_fs_memory_get_strings_channel_by_offset(memory, string_offset, &channel_monitor);
    if (strings_channel == null_ptr)
    {
      sc_fs_memory_error("Path `%s` doesn't exist", "path");
      return SC_FS_MEMORY_READ_ERROR;
    }

    // read string with size from fs-memory
    sc_uint64 read_bytes;
    sc_uint64 const normalized_string_offset = _sc_dictionary_fs_memory_normalize_offset(memory, string_offset);

    sc_bool go_to_next = SC_FALSE;
    sc_monitor_acquire_write(channel_monitor);
    sc_io_channel_seek(strings_channel, normalized_string_offset, SC_FS_IO_SEEK_SET, null_ptr);
    {
      sc_uint64 other_string_size;
      if (sc_io_channel_read_chars(
              strings_channel, (sc_char *)&other_string_size, sizeof(sc_uint64), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || sizeof(sc_uint64) != read_bytes)
        goto error;

      if (other_string_size < string_size)
      {
        go_to_next = SC_TRUE;
        goto cont;
      }

      sc_char * other_string = sc_mem_new(sc_char, other_string_size + 1);
      if (sc_io_channel_read_chars(strings_channel, other_string, other_string_size, &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || other_string_size != read_bytes)
      {
        sc_mem_free(other_string);
        goto error;
      }

      if ((to_search_as_prefix && sc_str_has_prefix(other_string, string) == SC_FALSE)
          || (!to_search_as_prefix && sc_str_find(other_string, string) == SC_FALSE))
      {
        sc_mem_free(other_string);
        go_to_next = SC_TRUE;
        goto cont;
      }

    cont:
      sc_monitor_release_write(channel_monitor);
      if (go_to_next)
        continue;

      callback(data, SC_ADDR_EMPTY, other_string);
      sc_mem_free(other_string);
    }
  }
  sc_iterator_destroy(string_offset_it);

  return SC_FS_MEMORY_OK;

error:
  sc_monitor_release_write(channel_monitor);
  sc_iterator_destroy(string_offset_it);
  return SC_FS_MEMORY_READ_ERROR;
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_get_strings_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 const string_size,
    sc_bool const to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to get link strings by substring");
    return SC_FS_MEMORY_NO;
  }

  sc_char * term = _sc_dictionary_fs_memory_get_first_term(string, memory->term_separators);
  sc_list * string_offsets = _sc_dictionary_fs_memory_get_string_offsets_by_term_prefix(memory, term);
  sc_mem_free(term);

  sc_dictionary_fs_memory_status const status = _sc_dictionary_fs_memory_get_strings_by_substring_term(
      memory, string, string_size, to_search_as_prefix, string_offsets, data, callback);
  sc_list_destroy(string_offsets);

  return status;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  return _sc_dictionary_fs_memory_get_strings_by_substring_ext(
      memory, string, string_size, string_size <= max_length_to_search_as_prefix, data, callback);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  return _sc_dictionary_fs_memory_get_strings_by_substring_ext(memory, string, string_size, SC_FALSE, data, callback);
}

sc_bool _sc_dictionary_fs_memory_get_link_hashes_by_string_offsets(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return SC_TRUE;

  sc_dictionary_fs_memory * memory = arguments[0];
  sc_uint64 const size = (sc_uint64)arguments[1];
  sc_list * link_hashes = arguments[2];

  sc_list * list = node->data;
  // unite or intersect link hashes from fs-memory
  if (size == 0 || list->size == size + 1)
  {
    sc_char const * string_offset_str = list->begin->data;

    sc_list * data = sc_dictionary_get_by_key(
        memory->string_offsets_link_hashes_dictionary, string_offset_str, sc_str_len(string_offset_str));
    sc_iterator * data_it = sc_list_iterator(data);
    while (sc_iterator_next(data_it))
    {
      void * hash = sc_iterator_get(data_it);
      sc_list_push_back(link_hashes, hash);
    }
    sc_iterator_destroy(data_it);
  }

  return SC_TRUE;
}

void _sc_dictionary_fs_memory_get_string_offsets_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_dictionary ** string_offsets_terms_dictionary)
{
  _sc_uchar_dictionary_initialize(string_offsets_terms_dictionary);

  sc_iterator * term_it = sc_list_iterator(terms);
  while (sc_iterator_next(term_it))
  {
    sc_char const * term = sc_iterator_get(term_it);
    sc_uint64 const term_size = sc_str_len(term);

    sc_list * string_offsets = sc_dictionary_get_by_key(memory->terms_string_offsets_dictionary, term, term_size);
    sc_iterator * string_offsets_it = sc_list_iterator(string_offsets);
    if (!sc_iterator_next(string_offsets_it))
    {
      sc_iterator_destroy(string_offsets_it);
      return;
    }

    while (sc_iterator_next(string_offsets_it))
    {
      sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(string_offsets_it);
      sc_char string_offset_str[DEFAULT_STRING_INT_SIZE];
      sc_uint64 string_offset_str_size;
      sc_int_to_str_int(string_offset, string_offset_str, string_offset_str_size);

      _sc_dictionary_fs_memory_append(
          *string_offsets_terms_dictionary, string_offset_str, string_offset_str_size, (void *)term);
    }
    sc_iterator_destroy(string_offsets_it);
  }
  sc_iterator_destroy(term_it);
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_get_link_hashes_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_bool const intersect,
    sc_list ** link_hashes)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to get link hashes by terms");
    return SC_FS_MEMORY_NO;
  }

  sc_list_init(link_hashes);
  if (terms->size == 0)
    return SC_FS_MEMORY_OK;

  sc_dictionary * string_offsets_terms_dictionary;
  _sc_dictionary_fs_memory_get_string_offsets_by_terms(memory, terms, &string_offsets_terms_dictionary);

  void * arguments[3];
  arguments[0] = (void *)memory;
  arguments[1] = intersect ? (void *)(sc_uint64)terms->size : 0;
  arguments[2] = *link_hashes;
  sc_dictionary_fs_memory_status const status = sc_dictionary_visit_down_nodes(
      string_offsets_terms_dictionary, _sc_dictionary_fs_memory_get_link_hashes_by_string_offsets, arguments);
  sc_dictionary_destroy(string_offsets_terms_dictionary, _sc_dictionary_fs_memory_node_clear);
  return status;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_intersect_link_hashes_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** link_hashes)
{
  return _sc_dictionary_fs_memory_get_link_hashes_by_terms(memory, terms, SC_TRUE, link_hashes);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unite_link_hashes_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** link_hashes)
{
  return _sc_dictionary_fs_memory_get_link_hashes_by_terms(memory, terms, SC_FALSE, link_hashes);
}

sc_bool _sc_dictionary_fs_memory_get_string_by_string_offsets(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return SC_TRUE;

  sc_dictionary_fs_memory * memory = arguments[0];
  sc_uint64 const size = (sc_uint64)arguments[1];
  sc_list * strings = arguments[2];

  sc_list * list = node->data;
  // unite or intersect strings from fs-memory
  if (size == 0 || list->size == size + 1)
  {
    sc_char * const string_offset_str = list->begin->data;
    sc_uint64 string_offset;
    sc_str_int_to_int(string_offset_str, string_offset);

    sc_char * string;
    sc_dictionary_fs_memory_status const status =
        _sc_dictionary_fs_memory_read_string_by_offset(memory, string_offset, &string);
    if (status != SC_FS_MEMORY_OK)
      return SC_FALSE;

    sc_list_push_back(strings, string);
  }

  return SC_TRUE;
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_get_strings_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_bool const intersect,
    sc_list ** strings)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to get link strings by terms");
    return SC_FS_MEMORY_NO;
  }

  sc_list_init(strings);
  if (terms->size == 0)
    return SC_FS_MEMORY_OK;

  sc_dictionary * term_string_offsets_dictionary;
  _sc_dictionary_fs_memory_get_string_offsets_by_terms(memory, terms, &term_string_offsets_dictionary);

  void * arguments[3];
  arguments[0] = (void *)memory;
  arguments[1] = intersect ? (void *)(sc_uint64)terms->size : 0;
  arguments[2] = *strings;
  sc_dictionary_visit_down_nodes(
      term_string_offsets_dictionary, _sc_dictionary_fs_memory_get_string_by_string_offsets, arguments);
  sc_dictionary_destroy(term_string_offsets_dictionary, _sc_dictionary_fs_memory_node_clear);

  return SC_FS_MEMORY_OK;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_intersect_strings_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** strings)
{
  return _sc_dictionary_fs_memory_get_strings_by_terms(memory, terms, SC_TRUE, strings);
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unite_strings_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** strings)
{
  return _sc_dictionary_fs_memory_get_strings_by_terms(memory, terms, SC_FALSE, strings);
}

void _sc_dictionary_fs_memory_read_terms_string_offsets(sc_dictionary_fs_memory * memory, sc_io_channel * channel)
{
  sc_uint64 read_bytes = 0;
  while (SC_TRUE)
  {
    sc_uint64 term_size;
    if (sc_io_channel_read_chars(channel, (sc_char *)&term_size, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != read_bytes)
      break;

    // allocate term in stack memory
    sc_char term[term_size + 1];
    if (sc_io_channel_read_chars(channel, (sc_char *)term, term_size, &read_bytes, null_ptr) != SC_FS_IO_STATUS_NORMAL
        || term_size != read_bytes)
      break;
    term[term_size] = '\0';

    sc_uint64 term_offset_count;
    if (sc_io_channel_read_chars(channel, (sc_char *)&term_offset_count, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != read_bytes)
      break;

    for (sc_uint64 i = 0; i < term_offset_count; ++i)
    {
      sc_uint64 string_offset;
      if (sc_io_channel_read_chars(channel, (sc_char *)&string_offset, sizeof(sc_uint64), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || sizeof(sc_uint64) != read_bytes)
        break;

      _sc_dictionary_fs_memory_append(memory->terms_string_offsets_dictionary, term, term_size, (void *)string_offset);
    }
  }
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_load_terms_offsets(sc_dictionary_fs_memory * memory)
{
  sc_fs_memory_info("Load `term - offsets` dictionary from %s", memory->terms_string_offsets_path);
  sc_io_channel * terms_offsets_channel = sc_io_new_read_channel(memory->terms_string_offsets_path, null_ptr);
  if (terms_offsets_channel == null_ptr)
  {
    sc_fs_memory_info("Path `%s` doesn't exist. Nothing to load", memory->terms_string_offsets_path);
    return SC_FS_MEMORY_NO;
  }
  sc_io_channel_set_encoding(terms_offsets_channel, null_ptr, null_ptr);

  sc_uint64 read_bytes = 0;
  if (sc_io_channel_read_chars(
          terms_offsets_channel, (sc_char *)&memory->last_string_offset, sizeof(sc_uint64), &read_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || sizeof(sc_uint64) != read_bytes)
  {
    sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);
    memory->last_string_offset = 0;
    return SC_FS_MEMORY_OK;
  }

  _sc_dictionary_fs_memory_read_terms_string_offsets(memory, terms_offsets_channel);

  sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);

  sc_fs_memory_info("Dictionary `term - offsets` loaded");
  return SC_FS_MEMORY_OK;
}

void _sc_dictionary_fs_memory_read_string_offsets_link_hashes(sc_dictionary_fs_memory * memory, sc_io_channel * channel)
{
  sc_uint64 read_bytes = 0;
  while (SC_TRUE)
  {
    sc_uint64 string_offset;
    if (sc_io_channel_read_chars(channel, (sc_char *)&string_offset, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != read_bytes)
      break;

    sc_uint64 link_hashes_count;
    if (sc_io_channel_read_chars(channel, (sc_char *)&link_hashes_count, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != read_bytes)
      break;

    for (sc_uint64 i = 0; i < link_hashes_count; ++i)
    {
      sc_addr_hash link_hash;
      if (sc_io_channel_read_chars(channel, (sc_char *)&link_hash, sizeof(sc_addr_hash), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || sizeof(sc_addr_hash) != read_bytes)
        break;

      _sc_dictionary_fs_memory_append_link_string_unique(memory, link_hash, string_offset);
    }
  }
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_load_string_offsets_link_hashes(
    sc_dictionary_fs_memory * memory)
{
  sc_fs_memory_info("Load `term - offsets` dictionary from %s", memory->string_offsets_link_hashes_path);
  sc_io_channel * channel = sc_io_new_read_channel(memory->string_offsets_link_hashes_path, null_ptr);
  if (channel == null_ptr)
  {
    sc_fs_memory_info("Path `%s` doesn't exist. Nothing to load", memory->string_offsets_link_hashes_path);
    return SC_FS_MEMORY_NO;
  }
  sc_io_channel_set_encoding(channel, null_ptr, null_ptr);

  _sc_dictionary_fs_memory_read_string_offsets_link_hashes(memory, channel);

  sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
  sc_fs_memory_info("Dictionary `string offsets - link hashes` loaded");

  return SC_FS_MEMORY_OK;
}

sc_fs_memory_status _sc_dictionary_fs_memory_load_deprecated_dictionaries(sc_dictionary_fs_memory * memory)
{
  sc_char * strings_path;
  {
    static sc_char const * strings_channel_name = "strings";
    sc_fs_concat_path_ext(memory->path, strings_channel_name, SC_FS_EXT, &strings_path);
  }

  if (sc_fs_is_file(strings_path) == SC_FALSE)
  {
    sc_mem_free(strings_path);
    return SC_FS_MEMORY_WRONG_PATH;
  }

  sc_fs_memory_warning("Load deprecated file memory dictionary from %s", strings_path);
  sc_io_channel * channel = sc_io_new_read_channel(strings_path, null_ptr);
  if (channel == null_ptr)
  {
    sc_fs_memory_error("Can't open strings from: %s", strings_path);
    return SC_FS_MEMORY_WRONG_PATH;
  }
  sc_io_channel_set_encoding(channel, null_ptr, null_ptr);

  sc_uint64 read_bytes = 0;
  while (SC_TRUE)
  {
    sc_uint8 hashes_size = 0;
    if (sc_io_channel_read_chars(channel, (sc_char *)&hashes_size, sizeof(hashes_size), &read_bytes, null_ptr)
        != SC_FS_IO_STATUS_NORMAL)
      break;

    sc_addr_hash * hashes = sc_mem_new(sc_addr_hash, hashes_size);
    if (sc_io_channel_read_chars(channel, (sc_char *)hashes, sizeof(sc_addr_hash) * hashes_size, &read_bytes, null_ptr)
        != SC_FS_IO_STATUS_NORMAL)
      break;

    sc_uint32 string_size = 0;
    if (sc_io_channel_read_chars(channel, (sc_char *)&string_size, sizeof(string_size), &read_bytes, null_ptr)
        != SC_FS_IO_STATUS_NORMAL)
      break;

    sc_char * string = sc_mem_new(sc_char, string_size + 1);
    if (sc_io_channel_read_chars(channel, (sc_char *)string, string_size, &read_bytes, null_ptr)
        != SC_FS_IO_STATUS_NORMAL)
      break;

    sc_uint8 i;
    for (i = 0; i < hashes_size; ++i)
    {
      if (hashes[i] == 0)
        continue;

      sc_dictionary_fs_memory_link_string(memory, hashes[i], string, string_size);
    }

    sc_mem_free(hashes);
    sc_mem_free(string);
  }

  sc_io_channel_shutdown(channel, SC_FALSE, null_ptr);

  sc_fs_remove_file(strings_path);
  sc_mem_free(strings_path);

  sc_fs_memory_warning("Deprecated file memory dictionary loaded");

  return SC_FS_MEMORY_OK;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_load(sc_dictionary_fs_memory * memory)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to load dictionaries");
    return SC_FS_MEMORY_NO;
  }

  sc_fs_memory_info("Load sc-fs-memory dictionaries");

  if (_sc_dictionary_fs_memory_load_deprecated_dictionaries(memory) != SC_FS_MEMORY_OK)
    _sc_dictionary_fs_memory_load_terms_offsets(memory);

  sc_message("\tLast string offset: %lld", memory->last_string_offset);

  _sc_dictionary_fs_memory_load_string_offsets_link_hashes(memory);

  sc_fs_memory_info("All sc-fs-memory dictionaries loaded");

  return SC_FS_MEMORY_OK;
}

sc_bool _sc_dictionary_fs_memory_write_term_string_offsets(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return SC_TRUE;

  sc_io_channel * channel = arguments[0];

  sc_list * list = node->data;
  sc_iterator * data_it = sc_list_iterator(list);

  // save term and string offsets in fs-memory
  sc_uint64 written_bytes = 0;
  if (sc_iterator_next(data_it))
  {
    sc_char * term = sc_iterator_get(data_it);
    sc_uint64 const term_size = sc_str_len(term);
    if (sc_io_channel_write_chars(channel, (sc_char *)&term_size, sizeof(sc_uint64), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != written_bytes)
    {
      sc_fs_memory_error("Error while attribute `term_size` writing");
      goto error;
    }

    if (sc_io_channel_write_chars(channel, (sc_char *)term, term_size, &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || term_size != written_bytes)
    {
      sc_mem_free(term);
      sc_fs_memory_error("Error while attribute `term` writing");
      goto error;
    }

    sc_uint64 const string_offsets_count = list->size - 1;
    if (sc_io_channel_write_chars(
            channel, (sc_char *)&string_offsets_count, sizeof(sc_uint64), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_uint64) != written_bytes)
    {
      sc_mem_free(term);
      sc_fs_memory_error("Error while attribute `string_offsets_count` writing");
      goto error;
    }

    while (sc_iterator_next(data_it))
    {
      sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(data_it);
      if (sc_io_channel_write_chars(channel, (sc_char *)&string_offset, sizeof(string_offset), &written_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL
          || sizeof(sc_uint64) != written_bytes)
      {
        sc_mem_free(term);
        sc_fs_memory_error("Error while attribute `string_offset` writing");
        goto error;
      }
    }
  }

  sc_iterator_destroy(data_it);
  return SC_TRUE;

error:
{
  sc_iterator_destroy(data_it);
  return SC_FALSE;
}
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_save_term_string_offsets(sc_dictionary_fs_memory const * memory)
{
  sc_io_channel * channel = sc_io_new_write_channel(memory->terms_string_offsets_path, null_ptr);
  sc_io_channel_set_encoding(channel, null_ptr, null_ptr);

  sc_uint64 written_bytes = 0;
  if (sc_io_channel_write_chars(
          channel, (sc_char *)&memory->last_string_offset, sizeof(sc_uint64), &written_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || sizeof(sc_uint64) != written_bytes)
  {
    sc_fs_memory_error("Error while attribute `last_string_offset` writing");
    sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  if (!sc_dictionary_visit_down_nodes(
          memory->terms_string_offsets_dictionary,
          _sc_dictionary_fs_memory_write_term_string_offsets,
          (void **)&channel))
  {
    sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
  sc_fs_memory_info("Dictionary `term - offsets` written");
  return SC_FS_MEMORY_OK;
}

sc_bool _sc_dictionary_fs_memory_write_string_offsets_link_hashes(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return SC_TRUE;

  sc_io_channel * channel = arguments[0];

  sc_link_hash_content * content = node->data;
  sc_iterator * data_it = sc_list_iterator(content->link_hashes);

  sc_uint64 written_bytes = 0;
  sc_uint64 const string_offset = content->string_offset - 1;
  if (sc_io_channel_write_chars(channel, (sc_char *)&string_offset, sizeof(sc_uint64), &written_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || sizeof(sc_uint64) != written_bytes)
  {
    sc_fs_memory_error("Error while attribute `string_offset` writing");
    goto error;
  }

  sc_uint64 const link_hashes_count = content->link_hashes->size;
  if (sc_io_channel_write_chars(channel, (sc_char *)&link_hashes_count, sizeof(sc_uint64), &written_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL
      || sizeof(sc_uint64) != written_bytes)
  {
    sc_fs_memory_error("Error while attribute `link_hashes_count` writing");
    goto error;
  }

  while (sc_iterator_next(data_it))
  {
    sc_addr_hash const link_hash = (sc_uint64)sc_iterator_get(data_it);
    if (sc_io_channel_write_chars(channel, (sc_char *)&link_hash, sizeof(sc_addr_hash), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL
        || sizeof(sc_addr_hash) != written_bytes)
    {
      sc_fs_memory_error("Error while attribute `link_hash` writing");
      goto error;
    }
  }

  sc_iterator_destroy(data_it);
  return SC_TRUE;

error:
{
  sc_iterator_destroy(data_it);
  return SC_FALSE;
}
}

sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_save_string_offsets_link_hashes(
    sc_dictionary_fs_memory const * memory)
{
  sc_io_channel * channel = sc_io_new_write_channel(memory->string_offsets_link_hashes_path, null_ptr);
  sc_io_channel_set_encoding(channel, null_ptr, null_ptr);

  if (!sc_dictionary_visit_down_nodes(
          memory->link_hashes_string_offsets_dictionary,
          _sc_dictionary_fs_memory_write_string_offsets_link_hashes,
          (void **)&channel))
  {
    sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
  sc_fs_memory_info("Dictionary `string offsets - link hashes` written");
  return SC_FS_MEMORY_OK;
}

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_save(sc_dictionary_fs_memory const * memory)
{
  if (memory == null_ptr)
  {
    sc_fs_memory_info("Memory is empty to save dictionaries");
    return SC_FS_MEMORY_NO;
  }

  sc_fs_memory_info("Save sc-fs-memory dictionaries");
  sc_dictionary_fs_memory_status status = _sc_dictionary_fs_memory_save_term_string_offsets(memory);
  if (status != SC_FS_MEMORY_OK)
    return status;

  status = _sc_dictionary_fs_memory_save_string_offsets_link_hashes(memory);
  if (status != SC_FS_MEMORY_OK)
    return status;

  sc_message("\tLast string offset: %lld", memory->last_string_offset);

  sc_fs_memory_info("All sc-fs-memory dictionaries saved");
  return status;
}

#endif
