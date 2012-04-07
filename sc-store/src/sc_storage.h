#ifndef _sc_storage_h_
#define _sc_storage_h_

#include "sc_types.h"
#include "sc_config.h"
#include <glib.h>

/* Segments pool */
extern GPtrArray *segments;

struct _sc_elements_stat
{
  guint node_count;
  guint arc_count;
  guint empty_count;
};

//! Initialize sc storage in specified path
gboolean sc_storage_initialize(const gchar *path);

//! Shutdown sc storage
void sc_storage_shutdown();

//! Append sc-element to segments pool
sc_uri sc_storage_append_el_into_segments(sc_element *element);

/*! Get segment by id
 * @param id Segment id
 * 
 * @return Pointer to segment with specified id
 */
sc_segment* sc_storage_get_segment(guint id);

/*! Get element by URI
 * @param uri element URI
 * @param force_load Flag to force load into memory, if segment that contains element isn't loaded.
 *
 * @return Return pointer to element with specified URI. If force_load flag is FALSE, and segment
 * with element isn't loaded, then return null. If element with specified URI doesn't exist, then return null.
 */
sc_element* sc_storage_get_element(sc_uri uri, gboolean force_load);

/*! Check if sc-element with specified uri exist
 * @param uri sc-element uri
 * @return Returns TRUE, if sc-element with \p uri exist; otherwise return false.
 */
gboolean sc_storage_is_element(sc_uri uri);

//! Create new sc-node in storage
sc_uri sc_storage_element_new(sc_type type);

//! Remove sc-node from storage
void sc_storage_element_free(sc_uri el_uri);


sc_uri sc_storage_node_new(sc_type type);

sc_uri sc_storage_arc_new(sc_type type,
			  sc_uri beg_uri,
			  sc_uri end_uri);

#endif


/*! Get statistics information about elements
 * @param stat Pointer to structure to write statistics
 */
void sc_storage_get_elements_stat(sc_elements_stat *stat);
