/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_memory_h_
#define _sc_fs_memory_h_

#include "../sc_types.h"
#include "../sc_defines.h"
#include "../sc_stream.h"
#include "../sc-container/sc-list/sc_list.h"

#define SC_STORAGE_SEG_CHECKSUM_SIZE 64

#ifdef SC_DICTIONARY_FS_MEMORY
typedef struct _sc_dictionary_fs_memory sc_fs_memory;
typedef enum _sc_dictionary_fs_memory_status sc_fs_memory_status;
#endif

typedef struct _sc_fs_memory_manager
{
  sc_fs_memory * fs_memory;
  sc_char * segments_path;

  sc_fs_memory_status (*initialize)(sc_fs_memory ** memory, const sc_char * path);
  sc_fs_memory_status (*shutdown)(sc_fs_memory * memory);
  sc_fs_memory_status (*clear)();
  sc_fs_memory_status (*load)(sc_fs_memory * memory);
  sc_fs_memory_status (*save)(sc_fs_memory const * memory);
  sc_fs_memory_status (*link_string)(
      sc_fs_memory * memory,
      sc_addr_hash const link_hash,
      sc_char const * string,
      sc_uint64 const string_size);
  sc_fs_memory_status (*get_string_by_link_hash)(
      sc_fs_memory const * memory,
      sc_addr_hash const link_hash,
      sc_char ** string,
      sc_uint64 * string_size);
  sc_fs_memory_status (*get_link_hashes_by_string)(
      sc_fs_memory const * memory,
      sc_char const * string,
      sc_uint64 const string_size,
      sc_list ** link_hashes);
  sc_fs_memory_status (*get_link_hashes_by_substring)(
      sc_fs_memory const * memory,
      sc_char const * substring,
      sc_uint64 const substring_size,
      sc_list ** link_hashes);
  sc_fs_memory_status (*get_strings_by_substring)(
      sc_fs_memory const * memory,
      sc_char const * substring,
      sc_uint64 const substring_size,
      sc_list ** strings);
  sc_fs_memory_status (*unlink_string)(sc_fs_memory * memory, sc_addr_hash const link_hash);
} sc_fs_memory_manager;

typedef struct _sc_fs_memory_segments_header
{
  sc_uint32 version;
  sc_uint16 segments_num;
  sc_uint64 timestamp;
  sc_uint8 checksum[SC_STORAGE_SEG_CHECKSUM_SIZE];  // buffer size for sha 512

} sc_fs_memory_segments_header;

/*! Initialize file system memory in specified path
 * @param path Path to store on file system.
 * @param clear Flag to initialize empty memory
 */
sc_bool sc_fs_memory_initialize(const sc_char * path, sc_bool clear);

//! Shutdowns file system memory
sc_bool sc_fs_memory_shutdown();

/*! Appends sc-link to file system memory by its string content.
 * @param link_hash An appendable sc-link hash
 * @param string A key string
 * @param string_size A key string size
 */
sc_bool sc_fs_memory_link_string(sc_addr_hash link_hash, sc_char const * string, sc_uint32 string_size);

/*! Gets sc-link content string with its size.
 * @param link_hash A sc-link hash
 * @param[out] string A content string
 * @param[out] string_size A content string size
 */
sc_bool sc_fs_memory_get_string_by_link_hash(sc_addr_hash link_hash, sc_char ** string, sc_uint32 * string_size);

/*! Gets sc-link hashes from file system memory by it string content.
 * @param string A key string
 * @param string_size A key string size
 * @param[out] link_hashes A pointer to sc-link hashes
 * @returns SC_TRUE, if sc-link_hashes exist.
 */
sc_bool sc_fs_memory_get_link_hashes_by_string(sc_char const * string, sc_uint32 string_size, sc_list ** link_hashes);

/*! Gets sc-link hashes from file system memory by it substring content.
 * @param substring A key substring
 * @param string_size A key string size
 * @param[out] link_hashes A pointer to sc-link hashes
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @returns SC_TRUE, if such sc-link hashes exist.
 */
sc_bool sc_fs_memory_get_link_hashes_by_substring(
    const sc_char * substring,
    sc_uint32 substring_size,
    sc_list ** link_hashes,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-strings from file system memory by it substring content.
 * @param substring A key substring
 * @param string_size A key string size
 * @param[out] strings A pointer to sc-strings array
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @returns SC_TRUE, if such sc-strings exist.
 */
sc_bool sc_fs_memory_get_strings_by_substring(
    const sc_char * substring,
    sc_uint32 string_size,
    sc_list ** strings,
    sc_uint32 max_length_to_search_as_prefix);

/*! Removes sc-link content string from file system memory.
 * @param link_hash A sc-link hash
 * @returns SC_TRUE, if such sc-string exists.
 */
sc_bool sc_fs_memory_unlink_string(sc_addr_hash link_hash);

sc_bool sc_fs_memory_load(sc_segment ** segments, sc_uint32 * segments_num);

sc_bool sc_fs_memory_save(sc_segment ** segments);

#endif
