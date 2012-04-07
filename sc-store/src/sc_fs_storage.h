#ifndef _sc_fs_store_h_
#define _sc_fs_store_h_

#include "sc_types.h"
#include "sc_defines.h"
#include <glib.h>

/*! Initialize file system storage in specified path
 *
 * @param path Path to store on file system.
 */
gboolean sc_fs_storage_initialize(const gchar *path);

/*! Shutdown file system storage
 */
gboolean sc_fs_storage_shutdown();

/*! Load segment from file system
 *
 * @param id Segment id.
 *
 * @return Pointer to loaded segment.
 */
sc_segment* sc_fs_storage_load_segment(guint id);


/*! Load segments from file system storage
 *
 * @param segments Pointer to segments array. 
 * It will be contain pointers to loaded segments.
 */
gboolean sc_fs_storage_read_from_path(GPtrArray *segments);

/*! Save segments to file system
 *
 * @param segments Pointer to array that contains segment pointers to save.
 */
gboolean sc_fs_storage_write_to_path(GPtrArray *segments);


#endif
