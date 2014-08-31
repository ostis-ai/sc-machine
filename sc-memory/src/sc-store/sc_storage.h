/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _sc_storage_h_
#define _sc_storage_h_

#include "sc_types.h"
#include "sc_defines.h"
#include "sc_stream.h"

#define STORAGE_CHECK_CALL(x) {sc_result r = x; g_assert(r == SC_RESULT_OK); }

/*! Initialize sc storage in specified path
 * @param path Path to repository
 * @param clear Flag to clear initialize empty storage
 */
sc_bool sc_storage_initialize(const char *path, sc_bool clear);

//! Shutdown sc storage
void sc_storage_shutdown(sc_bool save_state);

//! Check if storage initialized
sc_bool sc_storage_is_initialized();

/*! Append sc-element to segments pool
 * @param element Pointer to structure, that contains element information
 * @param addr Pointer to sc-addr structure, that will contains sc-addr of appended sc-element
 * @return Return pointer to created sc-element data. If sc-element wasn't appended, then return 0.
 * @note Returned sc-element is locked
 */
sc_element* sc_storage_append_el_into_segments(const sc_memory_context *ctx, sc_element *element, sc_addr *addr);

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns SC_TRUE, if sc-element with \p addr exist; otherwise return false.
 * If element deleted, then return SC_FALSE.
 */
sc_bool sc_storage_is_element(const sc_memory_context *ctx, sc_addr addr);

/*! Create new sc-element in storage.
 * Only for internal usage.
 * Use \b sc_storage_node_new and \b sc_storage_arc_new functions instead.
 */
sc_addr sc_storage_element_new(const sc_memory_context *ctx, sc_type type);

/*! Remove sc-element from storage
 * @param addr sc-addr of element to erase
 * @return If input params are correct and element erased, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result sc_storage_element_free(const sc_memory_context *ctx, sc_addr addr);

/*! Create new sc-node
 * @param type Type of new sc-node
 *
 * @return Return sc-addr of created sc-node
 */
sc_addr sc_storage_node_new(const sc_memory_context *ctx, sc_type type);

/*! Create new sc-link
 */
sc_addr sc_storage_link_new(const sc_memory_context *ctx);

/*! Create new sc-arc.
 * @param type Type of new sc-arc
 * @param beg sc-addr of begin sc-element
 * @param end sc-addr of end sc-element
 *
 * @return Return sc-addr of created sc-arc
 */
sc_addr sc_storage_arc_new(const sc_memory_context *ctx, sc_type type, sc_addr beg, sc_addr end);

/*! Get type of sc-element with specified sc-addr
 * @param addr sc-addr of element to get type
 * @param result Pointer to result container
 * @return If input params are correct and type resolved, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result sc_storage_get_element_type(const sc_memory_context *ctx, sc_addr addr, sc_type *result);

/*! Change element sub-type
 * @param addr sc-addr of element to set new type
 * @param type New sub-type of sc-element (this type must be: type & sc_type_element_mask == 0)
 * @return If sub-type changed, then returns SC_RESULT_OK; otherwise returns SC_RESULT_ERROR
 */
sc_result sc_storage_change_element_subtype(const sc_memory_context *ctx, sc_addr addr, sc_type type);

/*! Returns sc-addr of begin element of specified arc
 * @param addr sc-addr of arc to get begin element
 * @param result Pointer to result container
 * @return If input params are correct and begin element resolved, then return SC_OK.
 * If element with specified addr isn't an arc, then return SC_INVALID_TYPE
 */
sc_result sc_storage_get_arc_begin(const sc_memory_context *ctx, sc_addr addr, sc_addr *result);

/*! Returns sc-addr of end element of specified arc
 * @param addr sc-addr of arc to get end element
 * @param result PoOinter to result container
 * @return If input params are correct and end element resolved, then return SC_OK.
 * If element with specified addr isn't an arc, then return SC_INVALID_TYPE
 */
sc_result sc_storage_get_arc_end(const sc_memory_context *ctx, sc_addr addr, sc_addr *result);

/*! Setup content data for specified sc-link
 * @param addr sc-addr of sc-link to setup content
 * @param stream Pointer to stream
 * @return If content of specified link changed without any errors, then return SC_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_ERROR_INVALID_PARAMS - element with specifed \p addr doesn't exist
 * <li>SC_ERROR - unknown error</li>
 * </ul>
 */
sc_result sc_storage_set_link_content(const sc_memory_context *ctx, sc_addr addr, const sc_stream *stream);

/*! Returns content data from specified sc-link
 * @param addr sc-addr of sc-link to get content data
 * @param stream Pointer to returned data stream
 * @return If content of specified link returned without any errors, then return SC_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_ERROR_INVALID_PARAMS - element with specifed \p addr doesn't exist
 * <li>SC_ERROR - unknown error</li>
 * </ul>
 */
sc_result sc_storage_get_link_content(const sc_memory_context *ctx, sc_addr addr, sc_stream **stream);

/*! Search sc-link addrs by specified data
 * @param stream Pointert to stream that contains data for search
 * @param result Pointer to result container
 * @param result_count Container for results count
 * @return If sc-links with specified checksum found, then sc-addrs of found link
 * writes into \p result array and function returns SC_OK; otherwise \p result will contain
 * empty sc-addr and function returns SC_OK. In any case \p result_count contains number of found
 * sc-addrs
 * @attention \p result array need to be free after usage
 */
sc_result sc_storage_find_links_with_content(const sc_memory_context *ctx, const sc_stream *stream, sc_addr **result, sc_uint32 *result_count);

//! Returns number of segments
sc_uint sc_storage_get_segments_count();

/*! Get statistics information about elements
 * @param stat Pointer to structure that store statistic
 * @return If statictics info collect without any errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result sc_storage_get_elements_stat(const sc_memory_context *ctx, sc_stat *stat);

/*! Updates segments.
 * Calculate empty slots. Delete garbage
 * @note Only for internal usage
 */
void sc_storage_update_segments();


// ----- Locks -----
//! Locks specified sc-element. Pointer to locked sc-element stores in el
sc_result sc_storage_element_lock(const sc_memory_context *ctx, sc_addr addr, sc_element **el);
//! Try to lock sc-element by maximum attempts. If element wasn't locked and there are no errors, then el pointer will have null value.
sc_result sc_storage_element_lock_try(const sc_memory_context *ctx, sc_addr addr, sc_uint16 max_attempts, sc_element **el);
//! Unlocks specified sc-element
sc_result sc_storage_element_unlock(const sc_memory_context *ctx, sc_addr addr);

#endif

