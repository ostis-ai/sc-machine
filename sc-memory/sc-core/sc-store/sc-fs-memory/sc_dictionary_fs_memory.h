/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_memory_h_
#define _sc_dictionary_fs_memory_h_

#include "../sc_types.h"
#include "../sc_defines.h"
#include "../sc_stream.h"

#include "../sc-container/sc-dictionary/sc_dictionary.h"
#include "../sc-container/sc-list/sc_list.h"

typedef struct _sc_dictionary_fs_memory
{
  sc_char * path;
  sc_uint64 max_searchable_string_size;

  sc_char * strings_path;
  sc_uint64 last_string_offset;

  sc_dictionary * terms_string_offsets_dictionary;
  sc_char * terms_string_offsets_path;

  sc_dictionary * string_offsets_link_hashes_dictionary;
  sc_dictionary * link_hashes_string_offsets_dictionary;
  sc_char * string_offsets_link_hashes_path;

} sc_dictionary_fs_memory;

typedef struct _sc_link_hash_content
{
  sc_list * link_hashes;
  sc_uint64 string_offset;
} sc_link_hash_content;

typedef enum _sc_dictionary_fs_memory_status
{
  SC_FS_MEMORY_OK = 0,

  SC_FS_MEMORY_NO,
  SC_FS_MEMORY_NO_STRING,
  SC_FS_MEMORY_WRONG_PATH,
  SC_FS_MEMORY_WRITE_ERROR,
  SC_FS_MEMORY_READ_ERROR
} sc_dictionary_fs_memory_status;

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize(
    sc_dictionary_fs_memory ** memory,
    sc_char const * path);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_shutdown(sc_dictionary_fs_memory * memory);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_link_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash,
    sc_char const * string,
    sc_uint64 string_size);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_string_by_link_hash(
    sc_dictionary_fs_memory const * memory,
    sc_addr_hash link_hash,
    sc_char ** string,
    sc_uint64 * string_size);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_string(
    sc_dictionary_fs_memory const * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_list ** link_hashes);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring(
    sc_dictionary_fs_memory const * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_list ** link_hashes);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring(
    sc_dictionary_fs_memory const * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_list ** strings);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_intersect_link_hashes_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** link_hashes);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unite_link_hashes_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** link_hashes);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_intersect_strings_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** strings);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unite_strings_by_terms(
    sc_dictionary_fs_memory const * memory,
    sc_list const * terms,
    sc_list ** strings);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_load(sc_dictionary_fs_memory * memory);

sc_dictionary_fs_memory_status sc_dictionary_fs_memory_save(sc_dictionary_fs_memory const * memory);

#endif  //_sc_dictionary_fs_memory_h_
