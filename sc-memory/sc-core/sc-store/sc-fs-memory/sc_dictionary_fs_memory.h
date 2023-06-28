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

#include "sc_fs_memory_status.h"
#include "../../sc_memory_params.h"

typedef sc_fs_memory_status sc_dictionary_fs_memory_status;

typedef struct _sc_dictionary_fs_memory
{
  sc_char * path;  // path to all dictionary files
  sc_bool clear;

  sc_uint16 max_strings_channels;
  sc_uint32 max_strings_channel_size;
  sc_uint32 max_searchable_string_size;  // maximal size of strings that can be found by string/substring
  sc_char const * term_separators;
  sc_bool search_by_substring;

  void ** strings_channels;
  sc_uint64 last_string_offset;  // last offset of string in 'string_path`
  sc_mutex rw_mutex;

  sc_char * terms_string_offsets_path;              // path to dictionary file with terms and its strings offsets
  sc_dictionary * terms_string_offsets_dictionary;  // dictionary instance with terms and its strings offsets

  sc_char * string_offsets_link_hashes_path;  // path to dictionary file with strings offsets and its link hashes
  sc_dictionary *
      string_offsets_link_hashes_dictionary;  // dictionary instance with strings offsets and its link hashes
  sc_dictionary *
      link_hashes_string_offsets_dictionary;  // dictionary instance with link hashes and its strings offsets
} sc_dictionary_fs_memory;

typedef struct _sc_link_hash_content
{
  sc_list * link_hashes;
  sc_uint64 string_offset;
} sc_link_hash_content;

/*! Initialize sc-dictionary file system memory in specified path.
 * @param memory[out] A pointer to sc-memory instance
 * @param params Memory configure params
 * @returns SC_FS_MEMORY_OK, if file system memory initialized, or SC_FS_MEMORY_WRONG_PATH if path is not correct.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize_ext(
    sc_dictionary_fs_memory ** memory,
    sc_memory_params const * params);

/*! Initialize sc-dictionary file system memory in specified path.
 * @param memory[out] A pointer to sc-memory instance
 * @param path Path to store on file system
 * @returns SC_FS_MEMORY_OK, if file system memory initialized, or SC_FS_MEMORY_WRONG_PATH if path is not correct.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize(
    sc_dictionary_fs_memory ** memory,
    sc_char const * path);

/*! Shutdown sc-dictionary file system memory.
 * @param memory A pointer to sc-memory instance
 * @returns SC_FS_MEMORY_OK, if file system memory shutdown, or SC_FS_MEMORY_WRONG_PATH if path is not correct.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_shutdown(sc_dictionary_fs_memory * memory);

/*! Appends sc-link hash to file system memory with its string content.
 * @param memory A pointer to sc-memory instance
 * @param link_hash An appendable sc-link hash
 * @param string A sc-link string content
 * @param string_size A sc-link string content size
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_link_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash,
    sc_char const * string,
    sc_uint64 string_size);

/*! Appends sc-link hash to file system memory with its string content.
 * @param memory A pointer to sc-memory instance
 * @param link_hash An appendable sc-link hash
 * @param string A sc-link string content
 * @param string_size A sc-link string content size
 * @param is_searchable_string Ability to search for sc-links on this content string
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_link_string_ext(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash,
    sc_char const * string,
    sc_uint64 string_size,
    sc_bool is_searchable_string);

/*! Removes sc-link content string from file system memory.
 * @param memory A pointer to sc-memory instance
 * @param link_hash A sc-link hash
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unlink_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash);

/*! Gets sc-link content string with its size by sc-link hash.
 * @param memory A pointer to sc-memory instance
 * @param link_hash A sc-link hash
 * @param[out] string A sc-link content string
 * @param[out] string_size A sc-link content string size
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_string_by_link_hash(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash,
    sc_char ** string,
    sc_uint64 * string_size);

/*! Gets sc-link hashes from file system memory by its string content.
 * @param memory A pointer to sc-memory instance
 * @param string A sc-links content string
 * @param string_size A sc-links content string size
 * @param[out] link_hashes A pointer to sc-link hashes list
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_string(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_list ** link_hashes);

/*! Gets sc-link hashes from file system memory by its substring content.
 * @param memory A pointer to sc-memory instance
 * @param substring A sc-links content substring
 * @param string_size A sc-links content substring size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @param[out] link_hashes A pointer to sc-link hashes list
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_uint32 max_length_to_search_as_prefix,
    sc_list ** link_hashes);

/*! Gets sc-link hashes from file system memory by its substring content.
 * @param memory A pointer to sc-memory instance
 * @param substring A sc-links content substring
 * @param string_size A sc-links content substring size
 * @param[out] link_hashes A pointer to sc-link hashes list
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_list ** link_hashes);

/*! Gets sc-strings from file system memory by its substring content.
 * @param memory A pointer to sc-memory instance
 * @param substring A sc-strings content substring
 * @param string_size A sc-strings content substring size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @param[out] strings A pointer to sc-strings list
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_uint32 max_length_to_search_as_prefix,
    sc_list ** strings);

/*! Gets sc-strings from file system memory by its substring content.
 * @param memory A pointer to sc-memory instance
 * @param substring A sc-strings content substring
 * @param string_size A sc-strings content substring size
 * @param[out] strings A pointer to sc-strings list
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring(
    sc_dictionary_fs_memory * memory,
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

/*! Load file system memory from file system
 * @param memory A pointer to sc-memory instance
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_load(sc_dictionary_fs_memory * memory);

/*! Save file system memory to file system
 * @param memory A pointer to sc-memory instance
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_save(sc_dictionary_fs_memory const * memory);

#endif  //_sc_dictionary_fs_memory_h_
