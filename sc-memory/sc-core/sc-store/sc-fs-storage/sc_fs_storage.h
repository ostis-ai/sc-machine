/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_storage_h_
#define _sc_fs_storage_h_

#include "../sc_types.h"
#include "../sc_defines.h"
#include "../sc_stream.h"

#define SC_STORAGE_SEG_CHECKSUM_SIZE 64

typedef struct _sc_fs_storage
{
  sc_bool (*initialize)(const sc_char *);
  sc_bool (*shutdown)();
  sc_bool (*clear)();
  sc_bool (*fill)();
  sc_bool (*save)();
  sc_bool (*append_sc_link_content)(sc_element *, sc_addr, sc_char *, sc_uint32);
  sc_bool (*get_sc_links_by_content)(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size);
  sc_bool (*get_sc_links_by_content_substring)(
      const sc_char * sc_substr,
      sc_addr ** links,
      sc_uint32 * size,
      sc_uint32 max_length_to_search_as_prefix);
  sc_bool (*get_sc_links_contents_by_content_substring)(
      const sc_char * sc_substr,
      sc_char *** strings,
      sc_uint32 * size,
      sc_uint32 max_length_to_search_as_prefix);
  void (*get_sc_link_content_ext)(sc_element * element, sc_addr addr, sc_char ** sc_string, sc_uint32 * size);
  sc_bool (*remove_sc_link_content)(sc_element * element, sc_addr addr);
} sc_fs_storage;

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
sc_bool sc_fs_storage_initialize(const sc_char * path, sc_bool clear);

//! Shutdowns file system storage
sc_bool sc_fs_storage_shutdown(sc_segment ** segments, sc_bool save_segments);

/*! Appends sc-link to file system storage by its string content.
 * @param element An appendable sc-link
 * @param addr An appendable sc-link address
 * @param sc_string A key string
 * @param size A key string size
 */
sc_bool sc_fs_storage_append_sc_link(sc_element * element, sc_addr addr, sc_char * sc_string, sc_uint32 size);

/*! Gets sc-links from file system storage by it string content.
 * @param sc_string A key string
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_fs_storage_get_sc_links_by_content(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size);

/*! Gets sc-links from file system storage by it substring content.
 * @param sc_substr A key substring
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @returns SC_TRUE, if such sc-links exist.
 */
sc_bool sc_fs_storage_get_sc_links_by_content_substr(
    const sc_char * sc_substr,
    sc_addr ** links,
    sc_uint32 * size,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-strings from file system storage by it substring content.
 * @param sc_substr A key substring
 * @param[out] strings A pointer to sc-strings array
 * @param[out] size A sc-strings array size
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @returns SC_TRUE, if such sc-strings exist.
 */
sc_bool sc_fs_storage_get_sc_links_contents_by_content_substr(
    const sc_char * sc_substr,
    sc_char *** strings,
    sc_uint32 * size,
    sc_uint32 max_length_to_search_as_prefix);

/*! Gets sc-link content string with its size.
 * @param addr A sc-link
 * @param[out] sc_string A content string
 * @param[out] size A content string size
 */
void sc_fs_storage_get_sc_link_content_ext(sc_element * element, sc_addr addr, sc_char ** sc_string, sc_uint32 * size);

/*! Removes sc-link content string from file system storage.
 * @param element A sc-link element
 * @param addr A sc-link addr
 * @returns SC_TRUE, if such sc-string exists.
 */
sc_bool sc_fs_storage_remove_sc_link_content(sc_element * element, sc_addr addr);

/*! Loads segments from file system storage.
 * @param segments Pointer to segments array
 * @param segments_num Pointer to container for number of segments
 * It will be contain pointers to loaded segments.
 */
sc_bool sc_fs_storage_read_from_path(sc_segment ** segments, sc_uint32 * segments_num);

/*! Saves segments to file system storage.
 * @param segments Pointer to array that contains segment pointers to save.
 */
sc_bool sc_fs_storage_write_to_path(sc_segment ** segments);

sc_bool sc_fs_storage_save(sc_segment ** segments);

#endif
