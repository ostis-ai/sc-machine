/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */



#ifndef _sc_fs_memory_h_
#define _sc_fs_memory_h_

#include "sc_fs_memory_status.h"
#include "sc_fs_memory_header.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <libaio.h>
#include <pthread.h>



#include "sc-core/sc_types.h"
#include "sc-core/sc_defines.h"
#include "sc-core/sc_stream.h"
#include "sc-core/sc-container/sc_list.h"
#include "sc-core/sc_memory_params.h"
#include "sc-store/sc_storage.h"

#ifdef SC_DICTIONARY_FS_MEMORY
typedef struct _sc_dictionary_fs_memory sc_fs_memory;
#endif

#define PAGE_SIZE 4096
#define NUM_THREADS 4

typedef struct {
  int fd;
  sc_segment* segment;
  off_t offset;
} thread_args;


typedef struct _sc_fs_memory_manager
{
  sc_fs_memory * fs_memory;  // file system memory instance
  sc_char const * path;      // repo path
  sc_char * segments_path;   // file path to sc-memory segments

  sc_version version;
  sc_fs_memory_header header;

  sc_fs_memory_status (*initialize)(sc_fs_memory ** memory, sc_memory_params const * params);
  sc_fs_memory_status (*shutdown)(sc_fs_memory * memory);
  sc_fs_memory_status (*load)(sc_fs_memory * memory);
  sc_fs_memory_status (*save)(sc_fs_memory const * memory);
  sc_fs_memory_status (*link_string)(
      sc_fs_memory * memory,
      sc_addr_hash const link_hash,
      sc_char const * string,
      sc_uint64 const string_size,
      sc_bool is_searchable_string);
  sc_fs_memory_status (*get_string_by_link_hash)(
      sc_fs_memory * memory,
      sc_addr_hash const link_hash,
      sc_char ** string,
      sc_uint64 * string_size);
  sc_fs_memory_status (*get_link_hashes_by_string)(
      sc_fs_memory * memory,
      sc_char const * string,
      sc_uint64 const string_size,
      sc_link_handler * link_handler);
  sc_fs_memory_status (*get_link_hashes_by_substring)(
      sc_fs_memory * memory,
      sc_char const * substring,
      sc_uint64 const substring_size,
      sc_uint32 const max_length_to_search_as_prefix,
      sc_link_handler * link_handler);
  sc_fs_memory_status (*get_strings_by_substring)(
      sc_fs_memory * memory,
      sc_char const * substring,
      sc_uint64 const substring_size,
      sc_uint32 const max_length_to_search_as_prefix,
      sc_link_handler * link_handler);
  sc_fs_memory_status (*unlink_string)(sc_fs_memory * memory, sc_addr_hash const link_hash);
} sc_fs_memory_manager;

/*! Initialize file system memory in specified path.
 * @param params Memory configure params
 * @returns SC_TRUE, if file system memory initialized.
 */
sc_fs_memory_status sc_fs_memory_initialize_ext(sc_memory_params const * params);

/*! Initialize file system memory in specified path.
 * @param path Path to store on file system
 * @param clear Flag to initialize empty memory
 * @returns SC_TRUE, if file system memory initialized.
 */
sc_fs_memory_status sc_fs_memory_initialize(sc_char const * path, sc_bool clear);

/*! Shutdowns file system memory.
 * @return SC_TRUE, if file system memory shutdown.
 */
sc_fs_memory_status sc_fs_memory_shutdown();

/*! Appends sc-link hash to file system memory with its string content.
 * @param link_hash An appendable sc-link hash
 * @param string A sc-link string content
 * @param string_size A sc-link string content size
 * @returns SC_TRUE, if are no writing errors.
 */
sc_fs_memory_status sc_fs_memory_link_string(sc_addr_hash link_hash, sc_char const * string, sc_uint32 string_size);

/*! Appends sc-link hash to file system memory with its string content.
 * @param link_hash An appendable sc-link hash
 * @param string A sc-link string content
 * @param string_size A sc-link string content size
 * @param is_searchable_string Ability to search for sc-links on this content string
 * @returns SC_TRUE, if are no writing errors.
 */
sc_fs_memory_status sc_fs_memory_link_string_ext(
    sc_addr_hash link_hash,
    sc_char const * string,
    sc_uint32 string_size,
    sc_bool is_searchable_string);

/*! Removes sc-link content string from file system memory.
 * @param link_hash A sc-link hash
 * @returns SC_TRUE, if such sc-string content exists.
 */
sc_fs_memory_status sc_fs_memory_unlink_string(sc_addr_hash link_hash);

/*! Gets sc-link content string with its size by sc-link hash.
 * @param link_hash A sc-link hash
 * @param[out] string A sc-link content string
 * @param[out] string_size A sc-link content string size
 * @returns SC_TRUE, if sc-link content exists.
 */
sc_fs_memory_status sc_fs_memory_get_string_by_link_hash(
    sc_addr_hash link_hash,
    sc_char ** string,
    sc_uint32 * string_size);

/*! Gets sc-link hashes from file system memory by its string content.
 * @param string A sc-links content string
 * @param string_size A sc-links content string size
 * @param link_handler Pointer to object with callbacks for handling sc-links.
 * @returns SC_TRUE, if sc-link hashes exist.
 */
sc_fs_memory_status sc_fs_memory_get_link_hashes_by_string(
    sc_char const * string,
    sc_uint32 string_size,
    sc_link_handler * link_handler);

/*! Gets sc-link hashes from file system memory by its substring content.
 * @param substring A sc-links content substring
 * @param substring_size A sc-links content substring size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @param link_handler Pointer to object with callbacks for handling sc-links.
 * @returns SC_TRUE, if such sc-link hashes exist.
 */
sc_fs_memory_status sc_fs_memory_get_link_hashes_by_substring(
    sc_char const * substring,
    sc_uint32 substring_size,
    sc_uint32 max_length_to_search_as_prefix,
    sc_link_handler * link_handler);

/*! Gets sc-strings from file system memory by its substring content.
 * @param substring A sc-strings content substring
 * @param substring_size A sc-strings content substring size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @param link_handler Pointer to object with callbacks for handling sc-links.
 * @returns SC_TRUE, if such sc-strings exist.
 */
sc_fs_memory_status sc_fs_memory_get_strings_by_substring(
    sc_char const * substring,
    sc_uint32 substring_size,
    sc_uint32 max_length_to_search_as_prefix,
    sc_link_handler * link_handler);

/*! Load file system memory from file system
 * @returns SC_TRUE, if file system loaded.
 */
sc_fs_memory_status sc_fs_memory_load(sc_storage * storage);

/*! Save file system memory to file system
 * @returns SC_TRUE, if file system saved.
 */
sc_fs_memory_status sc_fs_memory_save(sc_storage * storage);

#endif
