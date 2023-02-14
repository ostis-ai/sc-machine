/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifdef SC_DICTIONARY_FS_STORAGE

#  include "sc_dictionary_fs_storage.h"
#  include "sc_dictionary_fs_storage_private.h"

#  include "sc_file_system.h"

#  include "../sc-base/sc_message.h"
#  include "../sc-container/sc-list/sc_list.h"
#  include "../sc-container/sc-iterator/sc_container_iterator.h"

#  include <glib/gstdio.h>

#include "math.h"
#  include "sc_io.h"
#  include "../sc-container/sc-pair/sc_pair.h"

sc_uint8 _sc_dictionary_children_size()
{
  sc_uint8 const max_sc_char = 255;
  sc_uint8 const min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _sc_char_to_sc_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 const * mask)
{
  *ch_num = 128 + (sc_uint8)ch;
}

void _sc_init_db_path(sc_char const * path, sc_char const * postfix, sc_char ** out_path)
{
  sc_uint32 size = sc_str_len(path) + sc_str_len(postfix) + 2;
  *out_path = sc_mem_new(sc_char, size + 1);
  sc_str_printf(*out_path, size, "%s/%s", path, postfix);
}

sc_fs_storage_status sc_dictionary_fs_storage_initialize(sc_dictionary_fs_storage ** storage, sc_char const * path)
{
  if (path == null_ptr)
    return SC_FS_STORAGE_WRONG_PATH;

  if (sc_fs_isdir(path) == SC_FALSE)
  {
    if (sc_fs_mkdirs(path) == SC_FALSE)
      return SC_FS_STORAGE_WRONG_PATH;
  }

  *storage = sc_mem_new(sc_dictionary_fs_storage, 1);
  {
    sc_str_cpy((*storage)->path, path, sc_str_len(path));

    {
      sc_dictionary_initialize(
          &(*storage)->terms_offsets_dictionary, _sc_dictionary_children_size(), _sc_char_to_sc_int);
      static sc_char const * term_string_offsets = "term_string_offsets.scdb";
      _sc_init_db_path(path, term_string_offsets, &(*storage)->terms_offsets_path);

      sc_dictionary_initialize(
          &(*storage)->string_offsets_link_hashes_dictionary, _sc_dictionary_children_size(), _sc_char_to_sc_int);
      static sc_char const * string_offsets_link_hashes = "string_offsets_link_hashes.scdb";
      _sc_init_db_path(path, string_offsets_link_hashes, &(*storage)->string_offsets_link_hashes_path);

      sc_dictionary_initialize(
          &(*storage)->link_hashes_string_offsets_dictionary, _sc_dictionary_children_size(), _sc_char_to_sc_int);
      static sc_char const * link_hashes_string_offsets = "links_hashes_string_offset.scdb";
      _sc_init_db_path(path, link_hashes_string_offsets, &(*storage)->link_hashes_string_offsets_path);
    }

    static sc_char const * terms_postfix = "terms.scdb";
    _sc_init_db_path(path, terms_postfix, &(*storage)->terms_path);
    (*storage)->terms_size = 0;

    static sc_char const * strings_postfix = "strings.scdb";
    _sc_init_db_path(path, strings_postfix, &(*storage)->strings_path);
    (*storage)->strings_size = 0;
  }

  return SC_FS_STORAGE_OK;
}

void _sc_dictionary_fs_storage_node_destroy(sc_dictionary_node * node, void ** args)
{
  (void)args;

  if (node->data != null_ptr)
  {
    printf("%s\n", (char *)((sc_list *)node->data)->begin->data);
    sc_mem_free(((sc_list *)node->data)->begin->data);
    sc_list_destroy(node->data);
  }
  node->data = null_ptr;

  sc_mem_free(node->next);
  node->next = null_ptr;

  sc_mem_free(node->offset);
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_mem_free(node);
}

sc_fs_storage_status sc_dictionary_fs_storage_shutdown(sc_dictionary_fs_storage * storage)
{
  if (storage == null_ptr)
    return SC_FS_STORAGE_NO;

  {
    sc_mem_free(storage->path);

    {
      sc_dictionary_destroy(storage->terms_offsets_dictionary, _sc_dictionary_fs_storage_node_destroy);
      sc_mem_free(storage->terms_offsets_path);

      sc_dictionary_destroy(storage->string_offsets_link_hashes_dictionary, _sc_dictionary_fs_storage_node_destroy);
      sc_mem_free(storage->string_offsets_link_hashes_path);

      sc_dictionary_destroy(storage->link_hashes_string_offsets_dictionary, _sc_dictionary_fs_storage_node_destroy);
      sc_mem_free(storage->link_hashes_string_offsets_path);
    }

    sc_mem_free(storage->terms_path);
    sc_mem_free(storage->strings_path);
  }
  sc_mem_free(storage);

  return SC_FS_STORAGE_OK;
}

void _sc_dictionary_fs_storage_append(sc_dictionary * dictionary, sc_char * key, void * data)
{
  sc_uint32 const key_size = sc_str_len(key);
  sc_list * list = sc_dictionary_get(dictionary, key, key_size);
  if (list == null_ptr)
  {
    sc_list_init(&list);
    sc_dictionary_append(dictionary, key, key_size, list);
    sc_char * copied_key;
    sc_str_cpy(copied_key, key, key_size);
    sc_list_push_back(list, copied_key);
  }

  sc_list_push_back(list, data);
}

sc_fs_storage_status _sc_dictionary_fs_storage_write_strings(
    sc_dictionary_fs_storage * storage, sc_list const * strings_link_hashes, sc_list ** string_offsets)
{
  if (sc_list_init(string_offsets) == SC_FALSE)
    return SC_FS_STORAGE_WRITE_ERROR;

  sc_io_channel * strings_channel = sc_io_new_append_channel(storage->strings_path, null_ptr);
  sc_io_channel_set_encoding(strings_channel, null_ptr, null_ptr);

  sc_iterator * it = sc_list_iterator((sc_list *)strings_link_hashes);
  while (sc_iterator_next(it))
  {
    sc_pair const * pair = sc_iterator_get(it);

    sc_char * string_offset_str;
    sc_uint64 const string_offset = storage->strings_size;
    {
      sc_int_to_str_int(string_offset, string_offset_str);
    }

    sc_char * link_hash_str;
    sc_addr_hash const link_hash = (sc_addr_hash)pair->second;
    {
      sc_int_to_str_int(link_hash, link_hash_str);
    }

    // cache string offset and link hash data
    {
      _sc_dictionary_fs_storage_append(storage->link_hashes_string_offsets_dictionary, string_offset_str, (void *)link_hash);
      _sc_dictionary_fs_storage_append(storage->link_hashes_string_offsets_dictionary, link_hash_str, (void *)string_offset);
      sc_mem_free(string_offset_str);
      sc_mem_free(link_hash_str);
    }

    // local cache string offset
    {
      sc_list_push_back(*string_offsets, (void *)string_offset);
    }

    // save string in db
    {
      sc_char const * string = pair->first;
      sc_uint64 const size = sc_str_len(string);

      sc_uint64 written_bytes = 0;
      if (sc_io_channel_write_chars(strings_channel, &size, sizeof(size), &written_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL || sizeof(size) != written_bytes)
      {
        sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);
        return SC_FS_STORAGE_WRITE_ERROR;
      }

      storage->strings_size += written_bytes;

      if (sc_io_channel_write_chars(strings_channel, string, size, &written_bytes, null_ptr) != SC_FS_IO_STATUS_NORMAL
          || size != written_bytes)
      {
        sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);
        return SC_FS_STORAGE_WRITE_ERROR;
      }

      storage->strings_size += written_bytes;
    }
  }
  sc_iterator_destroy(it);
  sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);

  return SC_FS_STORAGE_OK;
}

sc_list * _sc_dictionary_fs_storage_get_string_terms(sc_char const * string)
{
  static const sc_char delim[] = " ,.-\0()[]_";

  sc_uint32 const size = sc_str_len(string);
  sc_char copied_string[size + 1];
  sc_mem_cpy(copied_string, string, size);
  copied_string[size] = '\0';

  sc_char * term = strtok(copied_string, delim);
  sc_list * terms;
  sc_list_init(&terms);
  sc_dictionary * unique_terms;
  sc_dictionary_initialize(&unique_terms, _sc_dictionary_children_size(), _sc_char_to_sc_int);
  while (term != null_ptr)
  {
    sc_uint64 const term_length = sc_str_len(term);
    sc_char * term_copy;
    sc_str_cpy(term_copy, term, term_length);

    if (!sc_dictionary_is_in(unique_terms, term_copy, term_length))
      sc_list_push_back(terms, term_copy);

    sc_dictionary_append(unique_terms, term_copy, term_length, null_ptr);
    term = strtok(null_ptr, delim);
  }
  sc_dictionary_destroy(unique_terms, sc_dictionary_node_destroy);

  return terms;
}

void _sc_dictionary_fs_storage_get_strings_with_term(
    sc_char * term,  sc_list * strings, sc_list * string_offsets, sc_list ** string_with_term_offsets)
{
  sc_list_init(string_with_term_offsets);

  sc_iterator * strings_it = sc_list_iterator(strings);
  sc_iterator * string_offsets_it = sc_list_iterator(string_offsets);
  while (sc_iterator_next(strings_it) && sc_iterator_next(string_offsets_it))
  {
    sc_char * const string = sc_iterator_get(strings_it);
    sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(string_offsets_it);

    // if string doesn't contain term then not save it offset
    if (sc_str_find(string, term) == SC_FALSE)
      continue;

    sc_list_push_back(*string_with_term_offsets, (void *)string_offset);
  }
  sc_iterator_destroy(strings_it);
  sc_iterator_destroy(string_offsets_it);
}

sc_dictionary * _sc_dictionary_fs_storage_assign_terms_in_strings(sc_list const * strings_link_hashes, sc_list const * string_offsets)
{
  sc_dictionary * term_string_offsets_dictionary;
  sc_dictionary_initialize(&term_string_offsets_dictionary, _sc_dictionary_children_size(), _sc_char_to_sc_int);

  sc_iterator * strings_link_hashes_it = sc_list_iterator(strings_link_hashes);
  sc_iterator * string_offsets_it = sc_list_iterator(string_offsets);
  while (sc_iterator_next(strings_link_hashes_it) && sc_iterator_next(string_offsets_it))
  {
    sc_pair const * pair = sc_iterator_get(strings_link_hashes_it);
    sc_char const * string = pair->first;

    sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(string_offsets_it);

    sc_list * string_terms = _sc_dictionary_fs_storage_get_string_terms(string);
    sc_iterator * term_it = sc_list_iterator(string_terms);
    while (sc_iterator_next(term_it))
    {
      sc_char * term = sc_iterator_get(term_it);

      _sc_dictionary_fs_storage_append(term_string_offsets_dictionary, term, (void *)string_offset);
      sc_mem_free(term);
    }
    sc_iterator_destroy(term_it);
    sc_list_destroy(string_terms);
  }
  sc_iterator_destroy(strings_link_hashes_it);
  sc_iterator_destroy(string_offsets_it);

  return term_string_offsets_dictionary;
}

void _sc_dictionary_fs_storage_pass_term_string_offset(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return;

  sc_dictionary_fs_storage * storage = arguments[0];
  sc_io_channel * channel = arguments[1];

  sc_iterator * data_it = sc_list_iterator(node->data);

  // save term in db
  sc_uint64 written_bytes = 0;
  if (sc_iterator_next(data_it))
  {
    sc_char * term = sc_iterator_get(data_it);
    sc_uint64 const term_size = sc_str_len(term);

    // cache term offset
    {
      _sc_dictionary_fs_storage_append(storage->terms_offsets_dictionary, term, (void *)storage->terms_size);
    }

    if (sc_io_channel_write_chars(channel, (sc_char *)&term_size, sizeof(sc_uint64), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
    {
      sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
    }
    storage->terms_size += written_bytes;

    if (sc_io_channel_write_chars(channel, (sc_char *)term, term_size, &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || term_size != written_bytes)
    {
      sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
    }
    storage->terms_size += written_bytes;

    sc_uint64 const strings_with_term_count = ((sc_list *)node->data)->size - 1;
    if (sc_io_channel_write_chars(
            channel, (sc_char *)&strings_with_term_count, sizeof(sc_uint64), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
    {
      sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
    }
    storage->terms_size += written_bytes;

    while (sc_iterator_next(data_it))
    {
      sc_uint64 const string_offset = (sc_uint64)sc_iterator_get(data_it);
      if (sc_io_channel_write_chars(
              channel, (sc_char *)&string_offset, sizeof(sc_uint64), &written_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
      {
        sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
      }
      storage->terms_size += written_bytes;
    }

    sc_mem_free(term);
  }
  sc_iterator_destroy(data_it);
  sc_list_destroy(node->data);
}

sc_fs_storage_status _sc_dictionary_fs_storage_write_term_string_offsets(
    sc_dictionary_fs_storage * storage, sc_dictionary * term_string_offsets_dictionary)
{
  if (term_string_offsets_dictionary == null_ptr)
    return SC_FS_STORAGE_WRITE_ERROR;

  sc_io_channel * channel = sc_io_new_append_channel(storage->terms_path, null_ptr);
  sc_io_channel_set_encoding(channel, null_ptr, null_ptr);

  void * arguments[2];
  arguments[0] = storage;
  arguments[1] = channel;
  sc_dictionary_visit_down_nodes(
      term_string_offsets_dictionary, _sc_dictionary_fs_storage_pass_term_string_offset, arguments);

  sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);

  return SC_FS_STORAGE_OK;
}

sc_fs_storage_status sc_dictionary_fs_storage_add_strings(sc_dictionary_fs_storage * storage, sc_list const * strings_link_hashes)
{
  if (storage == null_ptr)
    return SC_FS_STORAGE_NO;

  sc_list * string_offsets;
  if (_sc_dictionary_fs_storage_write_strings(storage, strings_link_hashes, &string_offsets) != SC_FS_STORAGE_OK)
    return SC_FS_STORAGE_WRITE_ERROR;

  sc_dictionary * term_string_offsets_dictionary = _sc_dictionary_fs_storage_assign_terms_in_strings(strings_link_hashes, string_offsets);
  sc_list_destroy(string_offsets);

  sc_fs_storage_status const status
      = _sc_dictionary_fs_storage_write_term_string_offsets(storage, term_string_offsets_dictionary);
  sc_dictionary_destroy(term_string_offsets_dictionary, sc_dictionary_node_destroy);

  return status;
}

sc_fs_storage_status _sc_dictionary_fs_storage_read_strings_by_term(
    sc_dictionary_fs_storage * storage, sc_uint64 const term_offset, sc_list ** strings)
{
  sc_list_init(&*strings);

  sc_io_channel * terms_channel = sc_io_new_read_channel(storage->terms_path, null_ptr);
  sc_io_channel_set_encoding(terms_channel, null_ptr, null_ptr);
  sc_io_channel_seek(terms_channel, term_offset, SC_FS_IO_SEEK_SET, null_ptr);

  sc_uint64 strings_with_term_count;
  sc_uint64 read_bytes = 0;
  {
    sc_uint64 term_size;
    if (sc_io_channel_read_chars(terms_channel, (sc_char *)&term_size, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
    {
      sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);
      return SC_FS_STORAGE_READ_ERROR;
    }

    sc_char * term = sc_mem_new(sc_char, term_size + 1);
    if (sc_io_channel_read_chars(terms_channel, (sc_char *)term, term_size, &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || term_size != read_bytes)
    {
      sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);
      return SC_FS_STORAGE_READ_ERROR;
    }
    sc_mem_free(term);

    if (sc_io_channel_read_chars(
            terms_channel, (sc_char *)&strings_with_term_count, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
    {
      sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);
      return SC_FS_STORAGE_READ_ERROR;
    }
  }

  sc_io_channel * strings_channel = sc_io_new_read_channel(storage->strings_path, null_ptr);
  sc_io_channel_set_encoding(strings_channel, null_ptr, null_ptr);

  for (sc_uint64 i = 0; i < strings_with_term_count; ++i)
  {
    sc_uint64 string_offset;
    if (sc_io_channel_read_chars(terms_channel, (sc_char *)&string_offset, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
    {
      sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);
      sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);
      return SC_FS_STORAGE_READ_ERROR;
    }

    // read string with size
    sc_io_channel_seek(strings_channel, string_offset, SC_FS_IO_SEEK_SET, null_ptr);
    {
      sc_uint64 string_size;
      if (sc_io_channel_read_chars(
              strings_channel, (sc_char *)&string_size, sizeof(sc_uint64), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
      {
        sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);
        sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);
        return SC_FS_STORAGE_READ_ERROR;
      }

      if (string_size > INT_MAX)
        continue;

      sc_char * string = sc_mem_new(sc_char, string_size + 1);
      if (sc_io_channel_read_chars(strings_channel, (sc_char *)string, string_size, &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL || string_size != read_bytes)
      {
        sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);
        sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);
        return SC_FS_STORAGE_READ_ERROR;
      }
      sc_list_push_back(*strings, string);
    }
  }
  sc_io_channel_shutdown(strings_channel, SC_TRUE, null_ptr);
  sc_io_channel_shutdown(terms_channel, SC_TRUE, null_ptr);

  return SC_FS_STORAGE_OK;
}

sc_fs_storage_status sc_dictionary_fs_storage_get_strings(sc_dictionary_fs_storage * storage, sc_list const * terms, sc_list ** strings_list)
{
  sc_list_init(&*strings_list);

  sc_iterator * term_it = sc_list_iterator((sc_list *)terms);
  while (sc_iterator_next(term_it))
  {
    sc_char const * term = sc_iterator_get(term_it);
    sc_list * term_offsets = sc_dictionary_get(storage->terms_offsets_dictionary, term, sc_str_len(term));
    sc_iterator * term_offsets_it = sc_list_iterator(term_offsets);
    if (!sc_iterator_next(term_offsets_it))
      continue;

    sc_list * strings_with_term;
    sc_uint32 count = 0;
    while (sc_iterator_next(term_offsets_it))
    {
      sc_uint64 const term_offset = (sc_uint64)sc_iterator_get(term_offsets_it);
      if (_sc_dictionary_fs_storage_read_strings_by_term(storage, term_offset, &strings_with_term) != SC_FS_STORAGE_OK)
      {
        sc_list_clear(strings_with_term);
        sc_list_destroy(strings_with_term);
        return SC_FS_STORAGE_READ_ERROR;
      }

      ++count;
    }
    sc_iterator_destroy(term_offsets_it);
    if (count != 0)
    {
      sc_list_push_back(*strings_list, strings_with_term);
    }
  }
  sc_iterator_destroy(term_it);

  return SC_FS_STORAGE_OK;
}

void _sc_dictionary_fs_storage_read_term_offsets(sc_dictionary_fs_storage * storage, sc_io_channel * channel)
{
  // load terms
  sc_uint64 read_bytes = 0;
  while (SC_TRUE)
  {
    sc_uint64 term_size;
    if (sc_io_channel_read_chars(channel, (sc_char *)&term_size, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
      break;

    sc_char * term = sc_mem_new(sc_char, 1);
    if (sc_io_channel_read_chars(channel, (sc_char *)term, term_size, &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || term_size != read_bytes)
      break;

    sc_uint64 term_offset_count;
    if (sc_io_channel_read_chars(channel, (sc_char *)&term_offset_count, sizeof(sc_uint64), &read_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
      break;

    for (sc_uint64 i = 0; i < term_offset_count; ++i)
    {
      sc_uint64 term_offset;
      if (sc_io_channel_read_chars(channel, (sc_char *)&term_offset, sizeof(sc_uint64), &read_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
        break;

      _sc_dictionary_fs_storage_append(storage->terms_offsets_dictionary, term, (void *)term_offset);
    }
  }
}

sc_fs_storage_status sc_dictionary_fs_storage_load(sc_dictionary_fs_storage * storage)
{
  sc_io_channel * terms_offsets_channel = sc_io_new_read_channel(storage->terms_offsets_path, null_ptr);
  if (terms_offsets_channel == null_ptr)
    return SC_FS_STORAGE_OK;

  sc_io_channel_set_encoding(terms_offsets_channel, null_ptr, null_ptr);

  sc_uint64 read_bytes = 0;
  if (sc_io_channel_read_chars(
          terms_offsets_channel, (sc_char *)&storage->terms_size, sizeof(sc_uint64), &read_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
  {
    sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);
    return SC_FS_STORAGE_READ_ERROR;
  }

  if (sc_io_channel_read_chars(
          terms_offsets_channel, (sc_char *)&storage->strings_size, sizeof(sc_uint64), &read_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != read_bytes)
  {
    sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);
    return SC_FS_STORAGE_READ_ERROR;
  }

  _sc_dictionary_fs_storage_read_term_offsets(storage, terms_offsets_channel);

  sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);
  sc_mem_free(terms_offsets_channel);

  return SC_FS_STORAGE_OK;
}

void _sc_dictionary_fs_storage_write_term_offsets(sc_dictionary_node * node, void ** arguments)
{
  if (node->data == null_ptr)
    return;

  sc_io_channel * channel = arguments[0];

  sc_list * list = node->data;
  sc_iterator * data_it = sc_list_iterator(list);

  // save term in db
  sc_uint64 written_bytes = 0;
  if (sc_iterator_next(data_it))
  {
    sc_char * term = sc_iterator_get(data_it);
    sc_uint64 const term_size = sc_str_len(term);
    if (sc_io_channel_write_chars(channel, (sc_char *)&term_size, sizeof(sc_uint64), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
    {
      sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
      g_error("Error occurred: term size didn't written.");
    }

    if (sc_io_channel_write_chars(channel, (sc_char *)term, term_size, &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || term_size != written_bytes)
    {
      sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
      g_error("Error occurred: term didn't written.");
    }

    sc_uint64 const term_offsets_count = list->size - 1;
    if (sc_io_channel_write_chars(channel, (sc_char *)&term_offsets_count, sizeof(sc_uint64), &written_bytes, null_ptr)
            != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
    {
      sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
      g_error("Error occurred: term offset count didn't written.");
    }

    while (sc_iterator_next(data_it))
    {
      sc_uint64 const term_offset = (sc_uint64)sc_iterator_get(data_it);
      if (sc_io_channel_write_chars(channel, (sc_char *)term_offset, sizeof(sc_uint64), &written_bytes, null_ptr)
              != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
      {
        sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
        g_error("Error occurred: term offset didn't written.");
      }
    }
  }
}

sc_fs_storage_status sc_dictionary_fs_storage_save(sc_dictionary_fs_storage * storage)
{
  sc_io_channel * terms_offsets_channel = sc_io_new_read_channel(storage->terms_offsets_path, null_ptr);
  sc_io_channel_set_encoding(terms_offsets_channel, null_ptr, null_ptr);

  sc_uint64 written_bytes = 0;
  if (sc_io_channel_write_chars(
          terms_offsets_channel, (sc_char *)&storage->terms_size, sizeof(sc_uint64), &written_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
  {
    sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);
    return SC_FS_STORAGE_WRITE_ERROR;
  }

  if (sc_io_channel_write_chars(
          terms_offsets_channel, (sc_char *)&storage->strings_size, sizeof(sc_uint64), &written_bytes, null_ptr)
          != SC_FS_IO_STATUS_NORMAL || sizeof(sc_uint64) != written_bytes)
  {
    sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);
    return SC_FS_STORAGE_WRITE_ERROR;
  }

  sc_dictionary_visit_down_nodes(
      storage->terms_offsets_dictionary, _sc_dictionary_fs_storage_write_term_offsets, (void **)&terms_offsets_channel);

  sc_io_channel_shutdown(terms_offsets_channel, SC_TRUE, null_ptr);

  return SC_FS_STORAGE_OK;
}

#endif
