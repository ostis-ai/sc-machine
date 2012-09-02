#ifndef _sc_storage_h_
#define _sc_storage_h_

#include "sc_types.h"
#include "sc_config.h"

struct _sc_elements_stat
{
    sc_uint32 node_count;
    sc_uint32 arc_count;
    sc_uint32 empty_count;

    sc_uint32 node_deleted;
    sc_uint32 arc_deleted;
};

//! Initialize sc storage in specified path
sc_bool sc_storage_initialize(const char *path);

//! Shutdown sc storage
void sc_storage_shutdown();

//! Check if storage initialized
sc_bool sc_storage_is_initialized();

/*! Append sc-element to segments pool
 * @param element Pointer to structure, that contains element information
 * @param addr Pointer to sc-addr structure, that will contains sc-addr of appended sc-element
 * @return Return pointer to created sc-element data. If sc-element wasn't appended, then return 0.
 */
sc_element* sc_storage_append_el_into_segments(sc_element *element, sc_addr *addr);

/*! Get segment by index
 * @param seg Segment index
 * @param force_load Flag to force load into memory, if segment isn't loaded.
 * @return Pointer to segment with specified index
 */
sc_segment* sc_storage_get_segment(sc_addr_seg seg, sc_bool force_load);

/*! Get element by sc-addr
 * @param addr sc-addr of element
 * @param force_load Flag to force load into memory, if segment that contains element isn't loaded.
 *
 * @return Return pointer to element with specified sc-addr. If force_load flag is SC_FALSE, and segment
 * with element isn't loaded, then return null. If element with specified sc-addr doesn't exist, then return null.
 */
sc_element* sc_storage_get_element(sc_addr addr, sc_bool force_load);

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns SC_TRUE, if sc-element with \p addr exist; otherwise return false.
 * If element deleted, then return SC_FALSE.
 */
sc_bool sc_storage_is_element(sc_addr addr);

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

/*! Create new sc-link
 */
sc_addr sc_storage_link_new();

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

/*! Get type of sc-element with specified sc-addr
 * @param addr sc-addr of element to get type
 * @param result Pointer to result container
 * @return If input params are correct and type resolved, then return SC_TRUE;
 * otherwise return SC_FALSE
 */
sc_bool sc_storage_get_element_type(sc_addr addr, sc_type *result);

/*! Returns sc-addr of begin element of specified arc
 * @param addr sc-addr of arc to get begin element
 * @param result Pointer to result container
 * @return If input params are correct and begin element resolved, then return SC_TRUE;
 * otherwise return SC_FALSE
 */
sc_bool sc_storage_get_arc_begin(sc_addr addr, sc_addr *result);

/*! Returns sc-addr of end element of specified arc
 * @param addr sc-addr of arc to get end element
 * @param result PoOinter to result container
 * @return If input params are correct and end element resolved, then return SC_TRUE;
 * otherwise return SC_FALSE
 */
sc_bool sc_storage_get_arc_end(sc_addr addr, sc_addr *result);

//! Returns number of segments
sc_uint sc_storage_get_segments_count();

/*! Get statistics information about elements
 * @param stat Pointer to structure to write statistics
 */
void sc_storage_get_elements_stat(sc_elements_stat *stat);


//! Returns time stamp value
sc_uint sc_storage_get_time_stamp();

#endif

