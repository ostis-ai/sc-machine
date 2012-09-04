#ifndef _sc_fs_store_h_
#define _sc_fs_store_h_

#include "sc_types.h"
#include "sc_defines.h"
#include <glib.h>


/*! Initialize file system storage in specified path
 *
 * @param path Path to store on file system.
 */
sc_bool sc_fs_storage_initialize(const char *path);

/*! Shutdown file system storage
 */
sc_bool sc_fs_storage_shutdown(GPtrArray *segments);

/*! Load segment from file system
 *
 * @param id Segment id.
 *
 * @return Pointer to loaded segment.
 */
sc_segment* sc_fs_storage_load_segment(sc_uint id);


/*! Load segments from file system storage
 *
 * @param segments Pointer to segments array.
 * It will be contain pointers to loaded segments.
 */
sc_bool sc_fs_storage_read_from_path(GPtrArray *segments);

/*! Save segments to file system
 *
 * @param segments Pointer to array that contains segment pointers to save.
 */
sc_bool sc_fs_storage_write_to_path(GPtrArray *segments);

/*! Write specified data as content
 * @param addr sc-addr of sc-link that contains data
 * @param check_sum Pointer to checksum data
 * @param data Pointer to data
 * @param data_len Data length in bytes
 * @return If content saved, then return SC_OK; otherwise return one of error code
 */
sc_result sc_fs_storage_write_content(sc_addr addr, const sc_check_sum *check_sum, const sc_uint8 *data, sc_uint32 data_len);


/*! Make directory path from checksum
 * @param check_sum Checksum pointer to make path
 * @return Returns null terminated string that contains directory path (relative to contents directory). The
 * returned string should be freed with free, when done using it.
 * If there are any errors, then return null.
 */
sc_uint8* sc_fs_storage_make_checksum_path(const sc_check_sum *check_sum);

#endif
