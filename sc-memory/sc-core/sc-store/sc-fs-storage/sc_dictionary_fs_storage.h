/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_storage_h_
#define _sc_dictionary_fs_storage_h_

#include "../sc_types.h"
#include "../sc_defines.h"
#include "../sc_stream.h"

#include "../sc-container/sc-dictionary/sc_dictionary.h"

typedef struct _sc_link_content
{
  sc_dictionary_node * node;
  sc_char * sc_string;
  sc_uint32 string_size;
} sc_link_content;

/*! Initialize sc-dictionary fs-storage in specified path
 * @param path Path to store on file system.
 */
sc_bool sc_dictionary_fs_storage_initialize(const char * path);

//! Shutdowns sc-dictionary fs-storage
sc_bool sc_dictionary_fs_storage_shutdown();

/// Save sc-dictionary fs-storage
sc_bool sc_dictionary_fs_storage_save();

/// Read and fill sc-dictionary fs-storage
sc_bool sc_dictionary_fs_storage_fill();

/*! Appends sc-link to sc-dictionary by its string content.
 * @param element An appendable sc-link
 * @param addr An appendable sc-link addr
 * @param sc_string A key string
 * @param size A key string size
 * @returns SC_TRUE, if link was added
 */
sc_bool sc_dictionary_fs_storage_append_sc_link_content(
    sc_element * element,
    sc_addr addr,
    sc_char * sc_string,
    sc_uint32 size);

/*! Gets sc-links from sc-dictionary by it string content.
 * @param sc_string A key string
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_dictionary_fs_storage_get_sc_links_by_content(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size);

/*! Gets sc-links from sc-dictionary by it substring content.
 * @param sc_substr A key substring
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_dictionary_fs_storage_get_sc_links_by_content_substr(
    const sc_char * sc_substr,
    sc_addr ** links,
    sc_uint32 * size,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-strings from sc-dictionary by it substring content.
 * @param sc_substr A key substring
 * @param[out] strings A pointer to sc-strings array
 * @param[out] size A sc-strings size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_dictionary_fs_storage_get_sc_links_contents_by_content_substr(
    const sc_char * sc_substr,
    sc_char *** strings,
    sc_uint32 * size,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-link content.
 * @param addr A sc-link with content
 * @returns A sc-link-content node.
 */
sc_link_content * sc_dictionary_fs_storage_get_sc_link_content(sc_addr addr);

/*! Gets sc-link content string with its size.
 * @param element A sc-link
 * @param addr A sc-link addr
 * @param[out] sc_string A content string
 * @param[out] size A content string size
 */
void sc_dictionary_fs_storage_get_sc_link_content_ext(
    sc_element * element,
    sc_addr addr,
    sc_char ** sc_string,
    sc_uint32 * size);

/*! Removes sc-link content string from sc-dictionary.
 * @param element A sc-link element
 * @param addr A sc-link addr
 * @returns SC_TRUE, if such sc-link exists in sc-dictionary.
 */
sc_bool sc_dictionary_fs_storage_remove_sc_link_content(sc_element * element, sc_addr addr);

sc_uint32 sc_addr_to_hash(sc_addr addr);

sc_char * sc_addr_to_str(sc_addr addr);

#endif
