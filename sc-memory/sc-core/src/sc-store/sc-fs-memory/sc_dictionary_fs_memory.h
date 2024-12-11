/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_memory_h_
#define _sc_dictionary_fs_memory_h_

#include "sc_fs_memory_status.h"

#include "sc-core/sc_memory_params.h"

#include "sc-core/sc_types.h"
#include "sc-core/sc_defines.h"
#include "sc-core/sc_stream.h"

#include "sc-core/sc-container/sc_dictionary.h"
#include "sc-core/sc-container/sc_list.h"

typedef struct _sc_dictionary_fs_memory sc_dictionary_fs_memory;
typedef sc_fs_memory_status sc_dictionary_fs_memory_status;

/*! Initialize sc-dictionary file system memory in specified path.
 * @param memory[out] A pointer to file memory
 * @param params Memory configure params
 * @returns SC_FS_MEMORY_OK, if file system memory initialized, or SC_FS_MEMORY_WRONG_PATH if path is not correct.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize_ext(
    sc_dictionary_fs_memory ** memory,
    sc_memory_params const * params);

/*! Initialize sc-dictionary file system memory in specified path.
 * @param memory[out] A pointer to file memory
 * @param path Path to store on file system
 * @returns SC_FS_MEMORY_OK, if file system memory initialized, or SC_FS_MEMORY_WRONG_PATH if path is not correct.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_initialize(
    sc_dictionary_fs_memory ** memory,
    sc_char const * path);

/*! Shutdown sc-dictionary file system memory.
 * @param memory A pointer to file memory
 * @returns SC_FS_MEMORY_OK, if file system memory shutdown, or SC_FS_MEMORY_WRONG_PATH if path is not correct.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_shutdown(sc_dictionary_fs_memory * memory);

/*! Appends sc-link hash to file system memory with its string content.
 * @param memory A pointer to file memory
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
 * @param memory A pointer to file memory
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
 * @param memory A pointer to file memory
 * @param link_hash A sc-link hash
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_unlink_string(
    sc_dictionary_fs_memory * memory,
    sc_addr_hash link_hash);

/*! Gets sc-link content string with its size by sc-link hash.
 * @param memory A pointer to file memory
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

/*! Function that retrieves sc-link hashes by a full string term from the file memory.
 * @param memory Pointer to the file memory.
 * @param string Pointer to the full string term.
 * @param string_size Size of the full string term.
 * @param link_filter Pointer to object with callbacks for filtering sc-links.
 * @returns Returns the memory status indicating the success or failure of the operation.
 * @note This function is a wrapper for sc_dictionary_fs_memory_get_link_hashes_by_string_ext with is_substring set to
 * SC_FALSE.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_string(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_link_filter * link_filter);

/*! Function that retrieves sc-link hashes by a substring term extension from the file memory.
 * @param memory Pointer to the memory memory.
 * @param string Pointer to the substring term.
 * @param string_size Size of the substring term.
 * @param max_length_to_search_as_prefix Maximum length to consider the search as a prefix search.
 * @param link_filter Pointer to object with callbacks for filtering sc-links.
 * @returns Returns the memory status indicating the success or failure of the operation.
 * @note This function is a wrapper for sc_dictionary_fs_memory_get_link_hashes_by_string_ext with is_substring set to
 * SC_TRUE.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_uint32 max_length_to_search_as_prefix,
    sc_link_filter * link_filter);

/*! Function that retrieves sc-link hashes by a substring term from the file memory.
 * @param memory Pointer to the file memory.
 * @param string Pointer to the substring term.
 * @param string_size Size of the substring term.
 * @param link_filter Pointer to object with callbacks for filtering sc-links.
 * @returns Returns the memory status indicating the success or failure of the operation.
 * @note This function is a wrapper for sc_dictionary_fs_memory_get_link_hashes_by_substring_ext with
 * max_length_to_search_as_prefix set to 0.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_link_hashes_by_substring(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_link_filter * link_filter);

/*! Function that retrieves sc-link strings by a substring term extension from the file memory.
 * @param memory Pointer to the file memory.
 * @param string Pointer to the substring term.
 * @param string_size Size of the substring term.
 * @param max_length_to_search_as_prefix Maximum length to consider the search as a prefix search.
 * @param link_filter Pointer to object with callbacks for filtering sc-links.
 * @returns Returns the memory status indicating the success or failure of the operation.
 * @note This function is a wrapper for _sc_dictionary_fs_memory_get_strings_by_substring_ext with
 * to_search_as_prefix set based on the max_length_to_search_as_prefix parameter. It uses the appropriate function to
 * get string offsets based on the to_search_as_prefix flag. For each matching sc-link sc-address and link content
 * found, the provided callback function is invoked with the user-specific filter, sc-link sc-address, and link content.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring_ext(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_uint32 max_length_to_search_as_prefix,
    sc_link_filter * link_filter);

/*! Function that retrieves sc-link strings by a substring term from the file memory.
 * @param memory Pointer to the file memory.
 * @param string Pointer to the substring term.
 * @param string_size Size of the substring term.
 * @param link_filter Pointer to object with callbacks for filtering sc-links.
 * @returns Returns the memory status indicating the success or failure of the operation.
 * @note This function is a wrapper for _sc_dictionary_fs_memory_get_strings_by_substring_ext with to_search_as_prefix
 * set to SC_FALSE.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_get_strings_by_substring(
    sc_dictionary_fs_memory * memory,
    sc_char const * string,
    sc_uint64 string_size,
    sc_link_filter * link_filter);

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
 * @param memory A pointer to file memory
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_load(sc_dictionary_fs_memory * memory);

/*! Save file system memory to file system
 * @param memory A pointer to file memory
 * @returns SC_FS_MEMORY_OK, if are no reading and writing errors.
 */
sc_dictionary_fs_memory_status sc_dictionary_fs_memory_save(sc_dictionary_fs_memory const * memory);

#endif  //_sc_dictionary_fs_memory_h_
