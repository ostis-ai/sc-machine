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

#ifndef _sc_memory_h_
#define _sc_memory_h_

#include "sc-store/sc_types.h"
#include "sc-store/sc_stream.h"

// Public functions that used by developer

/*! Initialize sc-memory with specified path to repository
 * @param repo_path Path to repository on file system
 * @param config_file Path to configuration file. If it null, then default values will be used
 * @param clear Flag to clear memory on initialization. This function destroys whole elements, that exists in memory
 */
sc_bool sc_memory_initialize(const sc_char *repo_path, const sc_char *config_file, sc_bool clear);

/*! Initialize sc-memory extensions from specified \p path
 * @param path Path to directory, that contains extensions
 */
sc_bool sc_memory_initialize_ext(const sc_char *path);

//! Shutdown extensions
void sc_memory_shutdown_ext();

//! Shutdown sc-memory (save repository to file system)
void sc_memory_shutdown();

//! Check if sc-memory is initialized
sc_bool sc_memory_is_initialized();

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns SC_TRUE, if sc-element with \p addr exist; otherwise return SC_FALSE.
 * If element deleted, then return SC_FALSE.
 * @note This function is a thread safe
 */
sc_bool sc_memory_is_element(sc_addr addr);

//! Remove sc-element from sc-memory
sc_result sc_memory_element_free(sc_addr addr);

/*! Create new sc-node
 * @param type Type of new sc-node
 * @return Return sc-addr of created sc-node
 * @note This function is a thread safe
 */
sc_addr sc_memory_node_new(sc_type type);

/*! Create new sc-link
 * @note This function is a thread safe
 */
sc_addr sc_memory_link_new();

/*! Create new sc-arc.
 * @param type Type of new sc-arc
 * @param beg sc-addr of begin sc-element
 * @param end sc-addr of end sc-element
 *
 * @return Return sc-addr of created sc-arc
 * @note This function is a thread safe
 */
sc_addr sc_memory_arc_new(sc_type type, sc_addr beg, sc_addr end);

/*! Get type of sc-element with specified sc-addr
 * @param addr sc-addr of element to get type
 * @param result Pointer to result container
 * @return If input params are correct and type resolved, then return SC_RESULT_OK;
 * otherwise return SC_RESULT_ERROR
 * @note This function is a thread safe
 */
sc_result sc_memory_get_element_type(sc_addr addr, sc_type *result);

/*! Change element sub-type
 * @param addr sc-addr of element to set new type
 * @param type New sub-type of sc-element (this type must be: type & sc_type_element_mask == 0)
 * @return If sub-type changed, then returns SC_RESULT_OK; otherwise returns SC_RESULT_ERROR
 * @note This function is a thread safe
 */
sc_result sc_memory_change_element_subtype(sc_addr addr, sc_type type);

/*! Returns sc-addr of begin element of specified arc
 * @param addr sc-addr of arc to get begin element
 * @param result Pointer to result container
 * @return If input params are correct and begin element resolved, then return SC_RESULT_OK.
 * If element with specified addr isn't an arc, then return SC_RESULT_INVALID_TYPE
 * @note This function is a thread safe
 */
sc_result sc_memory_get_arc_begin(sc_addr addr, sc_addr *result);

/*! Returns sc-addr of end element of specified arc
 * @param addr sc-addr of arc to get end element
 * @param result Pointer to result container
 * @return If input params are correct and end element resolved, then return SC_RESULT_OK.
 * If element with specified addr isn't an arc, then return SC_RESULT_INVALID_TYPE
 * @note This function is a thread safe
 */
sc_result sc_memory_get_arc_end(sc_addr addr, sc_addr *result);

/*! Setup content data for specified sc-link
 * @param addr sc-addr of sc-link to setup content
 * @param stream Pointer to stream
 * @return If content of specified link changed without any errors, then return SC_RESULT_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_RESULT_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_RESULT_ERROR - unknown error</li>
 * </ul>
 * @note This function is a thread safe
 */
sc_result sc_memory_set_link_content(sc_addr addr, const sc_stream *stream);

/*! Returns content of specified sc-link
 * @param addr sc-addr of sc-link to return content data
 * @param stream Pointer to returned stream.
 * @attention New stream will be allocated, so it would be need to free stream after using.
 * @return If content of specified link content returned without any errors, then return SC_RESULT_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_RESULT_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_RESULT_ERROR - unknown error</li>
 * </ul>
 * @note This function is a thread safe
 */
sc_result sc_memory_get_link_content(sc_addr addr, sc_stream **stream);

/*! Search sc-link addrs by specified checksum
 * @param stream Pointert to stream that contains data for search
 * @param result Pointer to result container
 * @param result_count Container for results count
 * @return If sc-links with specified checksum found, then sc-addrs of found link
 * writes into \p result array and function returns SC_RESULT_OK; otherwise \p result will contain
 * empty sc-addr and function returns SC_RESULT_OK. In any case \p result_count contains number of found
 * sc-addrs
 * @attention \p result array need to be free after usage
 */
sc_result sc_memory_find_links_with_content(const sc_stream *stream, sc_addr **result, sc_uint32 *result_count);

/*! Collect statistic information about current state of sc-memory
 * @param stat Pointer to structure, that will contains statistics info
 * @return If info collected without errors, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR
 */
sc_result sc_memory_stat(sc_stat *stat);



#endif
