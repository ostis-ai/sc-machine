/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_rocksdb_fs_storage_h_
#define _sc_rocksdb_fs_storage_h_

#include "../sc_types.h"
#include "../sc_stream.h"
#include "../sc-container/sc-list/sc_list.h"

/*! Initialize sc-rocksdb fs-storage in specified path
 * @param path Path to store on sc-rocksdb fs-storage.
 */
sc_bool sc_rocksdb_fs_storage_initialize(const sc_char * repo_path);

/// Shutdowns sc-rocksdb fs-storage
sc_bool sc_rocksdb_fs_storage_shutdown();

/// Clears strings in sc-rocksdb fs-storage
sc_bool sc_rocksdb_fs_storage_clear();

/// Saves strings in sc-rocksdb fs-storage
sc_bool sc_rocksdb_fs_storage_save();

/*! Appends sc-link to sc-rocksdb by its string content.
 * @param element An appendable sc-link
 * @param addr An appendable sc-link addr
 * @param sc_string A key string
 * @param sc_string_size A key string size
 * @returns SC_TRUE, if link was added
 */
sc_bool sc_rocksdb_fs_storage_append_sc_link_content_string(
    const sc_element * element,
    sc_addr addr,
    const sc_char * sc_string,
    sc_uint32 sc_string_size);

/*! Gets sc-links from sc-rocksdb by it string content.
 * @param sc_string A key string
 * @param sc_string_size A key string size
 * @param[out] links A pointer to sc-links
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_rocksdb_fs_storage_get_sc_links_by_content_string(
    const sc_char * sc_string,
    sc_uint32 sc_string_size,
    sc_list ** links);

/*! Finds sc-links in sc-rocksdb by it substring content.
 * @param sc_substring A key substring
 * @param sc_substring_size A key string size
 * @param[out] links A pointer to sc-links
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_rocksdb_fs_storage_find_sc_links_by_content_substring(
    const sc_char * sc_substring,
    sc_uint32 sc_substring_size,
    sc_list ** links,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-strings from sc-rocksdb by it substring.
 * @param sc_substring A key substring
 * @param sc_substring_size A key string size
 * @param[out] strings A pointer to sc-string array
 * @param[out] size A sc-links size
 * @returns SC_TRUE, if sc-strings array exist.
 */
sc_bool sc_rocksdb_fs_storage_find_sc_links_contents_by_content_substring(
    const sc_char * sc_substring,
    sc_uint32 sc_substring_size,
    sc_list ** strings,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-link content string with its size.
 * @param element A sc-link
 * @param addr A sc-link addr
 * @param[out] sc_string A content string
 * @param[out] size A content string size
 */
void sc_rocksdb_fs_storage_get_sc_link_content_string_ext(
    const sc_element * element,
    sc_addr addr,
    sc_char ** sc_string,
    sc_uint32 * size);

/*! Removes sc-link content string from sc-rocksdb.
 * @param element A sc-link element
 * @param addr A sc-link addr
 * @returns SC_TRUE, if such sc-link exists in sc-rocksdb.
 */
sc_bool sc_rocksdb_fs_storage_remove_sc_link_content_string(const sc_element * element, sc_addr addr);

#endif  // _sc_rocksdb_fs_storage_h_
