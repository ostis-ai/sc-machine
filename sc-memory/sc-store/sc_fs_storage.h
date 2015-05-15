/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_store_h_
#define _sc_fs_store_h_

#include "sc_types.h"
#include "sc_defines.h"
#include "sc_stream.h"

#define SC_STORAGE_SEG_CHECKSUM_SIZE 64

typedef struct _sc_fs_storage_segments_header
{
    sc_uint32 version;
    sc_uint16 segments_num;
    sc_uint64 timestamp;
    sc_uint8  checksum[SC_STORAGE_SEG_CHECKSUM_SIZE]; // buffer size for sha 512

} sc_fs_storage_segments_header;

/*! Initialize file system storage in specified path
 * @param path Path to store on file system.
 * @param clear Flag to initialize empty storage
 */
sc_bool sc_fs_storage_initialize(const char *path, sc_bool clear);

/*! Shutdown file system storage
 */
sc_bool sc_fs_storage_shutdown(sc_segment **segments, sc_bool save_segments);

/*! Load segments from file system storage
 *
 * @param segments Pointer to segments array.
 * @param segments_num Pointer to container for number of segments
 * It will be contain pointers to loaded segments.
 */
sc_bool sc_fs_storage_read_from_path(sc_segment **segments, sc_uint32 *segments_num);

/*! Save segments to file system
 *
 * @param segments Pointer to array that contains segment pointers to save.
 */
sc_bool sc_fs_storage_write_to_path(sc_segment **segments);

// -------------------------------------------------
/*! Write specified stream as content
 * @param addr sc-addr of sc-link that contains data
 * @param check_sum Pointer to checksum data
 * @param stream Pointer to stream that contains data for saving
 * @return If content saved, then return SC_RESULT_OK; otherwise return one of error code
 */
sc_result sc_fs_storage_write_content(sc_addr addr, const sc_check_sum *check_sum, const sc_stream *stream);

/*! Add new sc-addr to content backward links
 * @param addr sc-addr to append to backward links
 * @param check_sum Checksum of content
 * @return If sc-addr has been written to backward links, then return SC_RESULT_OK; otherwise return error code
 */
sc_result sc_fs_storage_add_content_addr(sc_addr addr, const sc_check_sum *check_sum);

/*! Remove sc-addr from content backward links
 * @param addr sc-addr to remove from backward links
 * @param check_sum Checksum of content
 * @return If sc-addr has been removed from backward links, then return SC_RESULT_OK; otherwise return error code
 */
sc_result sc_fs_storage_remove_content_addr(sc_addr addr, const sc_check_sum *check_sum);

/*! Search sc-link addrs by specified checksum
 * @param check_sum Checksum for search
 * @param result Pointer to result container
 * @param result_count Container for results count
 * @return If sc-links with specified checksum found, then sc-addrs of found link
 * writes into \p result array and function returns SC_RESULT_OK; otherwise \p result will contain
 * empty sc-addr and function returns SC_RESULT_OK. In any case \p result_count contains number of found
 * sc-addrs
 * @attention \p result array need to be free after usage
 */
sc_result sc_fs_storage_find_links_with_content(const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count);

/*! Returns stream that contains content with specified checksum
 * @param check_sum Pointer to checksum for content data returning
 * @param stream Pointer to returned stream
 * @attention Returned pointer to stream need to be free after usage
 * @return If data by specified checksum found, then return SC_RESULT_OK; otherwise return SC_ERROR
 */
sc_result sc_fs_storage_get_checksum_content(const sc_check_sum *check_sum, sc_stream **stream);


#endif
