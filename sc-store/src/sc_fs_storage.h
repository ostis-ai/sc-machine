#ifndef _sc_fs_store_h_
#define _sc_fs_store_h_

#include "sc_types.h"
#include "sc_defines.h"

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


#endif
