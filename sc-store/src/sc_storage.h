#ifndef _sc_storage_h_
#define _sc_storage_h_

#include "sc_types.h"
#include "sc_config.h"
#include <glib.h>

/* Segments pool */
extern GPtrArray *segments;

struct _sc_elements_stat
{
  guint32 node_count;
  guint32 arc_count;
  guint32 empty_count;

  guint32 node_deleted;
  guint32 arc_deleted;
};

//! Initialize sc storage in specified path
gboolean sc_storage_initialize(const gchar *path);

//! Shutdown sc storage
void sc_storage_shutdown();

/*! Append sc-element to segments pool
 */
sc_addr sc_storage_append_el_into_segments(sc_element *element);

/*! Get segment by id
 * @param id Segment id
 * 
 * @return Pointer to segment with specified id
 */
sc_segment* sc_storage_get_segment(guint id);

/*! Get element by sc-addr
 * @param addr sc-addr of element
 * @param force_load Flag to force load into memory, if segment that contains element isn't loaded.
 *
 * @return Return pointer to element with specified sc-addr. If force_load flag is FALSE, and segment
 * with element isn't loaded, then return null. If element with specified sc-addr doesn't exist, then return null.
 */
sc_element* sc_storage_get_element(sc_addr addr, gboolean force_load);

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns TRUE, if sc-element with \p addr exist; otherwise return false.
 */
gboolean sc_storage_is_element(sc_addr addr);

/*! Create new sc-element in storage.
 * Only for internal usage.
 * Use \b sc_storage_node_new and \b sc_storage_arc_new functions instead.
 */
sc_addr sc_storage_element_new(sc_type type);

//! Remove sc-element from storage
void sc_storage_element_free(sc_addr addr);

/*! Create new sc-node
 * @param type Type of new sc-node
 *
 * @return Return sc-addr of created sc-node
 */
sc_addr sc_storage_node_new(sc_type type);

/*! Create new sc-arc.
 * @param type Type of new sc-arc
 * @param beg sc-addr of begin sc-element
 * @param end sc-addr of end sc-element
 *
 * @return Return sc-addr of created sc-arc
 */
sc_addr sc_storage_arc_new(sc_type type,
			  sc_addr beg,
			  sc_addr end);

#endif


/*! Get statistics information about elements
 * @param stat Pointer to structure to write statistics
 */
void sc_storage_get_elements_stat(sc_elements_stat *stat);
