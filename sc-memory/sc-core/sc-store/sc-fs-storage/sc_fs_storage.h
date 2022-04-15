/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_store_h_
#define _sc_fs_store_h_

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

#define SC_STORAGE_SEG_CHECKSUM_SIZE 64

typedef struct _sc_fs_storage_segments_header
{
  sc_uint32 version;
  sc_uint16 segments_num;
  sc_uint64 timestamp;
  sc_uint8 checksum[SC_STORAGE_SEG_CHECKSUM_SIZE];  // buffer size for sha 512

} sc_fs_storage_segments_header;

/*! Initialize file system storage in specified path
 * @param path Path to store on file system.
 * @param clear Flag to initialize empty storage
 */
sc_bool sc_fs_storage_initialize(const char * path, sc_bool clear);

//! Shutdowns file system storage
sc_bool sc_fs_storage_shutdown(sc_segment ** segments, sc_bool save_segments);

/*! Appends sc-link to sc-dictionary by its string content.
 * @param addr An appendable sc-link
 * @param sc_string A key string
 * @param size A key string size
 * @returns A terminal node in sc-dictionary tree with sc-link with the same string content.
 */
sc_dictionary_node * sc_fs_storage_append_sc_link(sc_addr addr, sc_char * sc_string, sc_uint32 size);

/*! Gets sc-link from sc-dictionary by its string content.
 * @param sc_string A key string
 * @returns A sc-link.
 */
sc_addr sc_fs_storage_get_sc_link(const sc_char * sc_string);

/*! Gets sc-links from sc-dictionary by it string content.
 * @param sc_string A key string
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_fs_storage_get_sc_links(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size);

sc_dictionary_node * sc_fs_storage_get_node(sc_addr addr);

/*! Gets sc-link content.
 * @param addr A sc-link with content
 * @returns A sc-link-content node.
 */
sc_link_content * sc_fs_storage_get_link_content(sc_addr addr);

/*! Gets sc-link content string with its size.
 * @param addr A sc-link
 * @param[out] sc_string A content string
 * @param[out] size A content string size
 */
void sc_fs_storage_get_sc_string_ext(sc_addr addr, sc_char ** sc_string, sc_uint32 * size);

/*! Gets sc-link content string.
 * @param addr A sc-link
 * @returns A content string.
 */
sc_char * sc_fs_storage_get_sc_string(sc_addr addr);

/*! Loads segments from file system storage.
 * @param segments Pointer to segments array
 * @param segments_num Pointer to container for number of segments
 * It will be contain pointers to loaded segments.
 */
sc_bool sc_fs_storage_read_from_path(sc_segment ** segments, sc_uint32 * segments_num);

/*! Saves segments to file system.
 * @param segments Pointer to array that contains segment pointers to save.
 */
sc_bool sc_fs_storage_write_to_path(sc_segment ** segments);

sc_bool sc_fs_storage_write_strings();

sc_bool sc_fs_storage_read_strings();

sc_uint32 sc_addr_to_hash(sc_addr addr);

sc_char * sc_addr_to_str(sc_addr addr);

#endif