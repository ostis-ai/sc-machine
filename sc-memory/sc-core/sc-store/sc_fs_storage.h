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
  sc_uint8 checksum[SC_STORAGE_SEG_CHECKSUM_SIZE]; // buffer size for sha 512

} sc_fs_storage_segments_header;

void _sc_fm_remove_dir(const char *path);
sc_bool _sc_fs_mkdirs(const char *path);

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

sc_bool sc_fs_storage_write_strings();

sc_bool sc_fs_storage_read_strings();

#endif
