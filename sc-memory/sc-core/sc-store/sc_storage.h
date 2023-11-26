/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_storage_h_
#define _sc_storage_h_

#include "sc_defines.h"
#include "sc-event/sc_event_queue.h"
#include "sc_event.h"
#include "../sc_memory_params.h"

#include "sc_types.h"
#include "sc_stream.h"

#include "sc-container/sc-list/sc_list.h"
#include "sc-base/sc_monitor.h"

typedef struct _sc_storage sc_storage;

//! Initialize sc storage in specified path
sc_bool sc_storage_initialize(sc_memory_params const * params);

//! Shutdown sc storage
sc_bool sc_storage_shutdown(sc_bool save_state);

//! Check if storage initialized
sc_bool sc_storage_is_initialized();

sc_storage * sc_storage_get();

sc_event_emission_manager * sc_storage_get_event_emission_manager();

sc_event_registration_manager * sc_storage_get_event_registration_manager();

/*! Append sc-element to segments pool
 * @param addr Pointer to sc-addr structure, that will contains sc-addr of appended sc-element
 * @return Return pointer to created sc-element data. If sc-element wasn't appended, then return 0.
 * @note Returned sc-element is locked
 */
sc_element * sc_storage_allocate_new_element(sc_memory_context const * ctx, sc_addr * addr);

void sc_storage_start_new_process();

void sc_storage_end_new_process();

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns SC_TRUE, if sc-element with \p addr exist; otherwise return false.
 * If element deleted, then return SC_FALSE.
 */
sc_bool sc_storage_is_element(sc_memory_context const * ctx, sc_addr addr);

sc_result sc_storage_get_element_by_addr(sc_addr addr, sc_element ** el);

sc_result sc_storage_free_element(sc_addr addr);

/*! Remove sc-element from storage
 * @param addr sc-addr of element to erase
 * @return If input params are correct and element erased, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result sc_storage_element_free(sc_memory_context const * ctx, sc_addr addr);

/*! Create new sc-node
 * @param type Type of new sc-node
 * @return Return sc-addr of created sc-node or empty sc-addr if sc-node wasn't created
 */
sc_addr sc_storage_node_new(sc_memory_context const * ctx, sc_type type);

/*! Create new sc-link
 * @return Return sc-addr of created sc-link or empty sc-addr if sc-link wasn't created
 */
sc_addr sc_storage_link_new(sc_memory_context const * ctx, sc_type type);

/*! Create new sc-arc.
 * @param type Type of new sc-arc
 * @param beg sc-addr of begin sc-element
 * @param end sc-addr of end sc-element
 *
 * @return Return sc-addr of created sc-arc or empty sc-addr if sc-arc wasn't created
 */
sc_addr sc_storage_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end);

sc_uint32 sc_storage_get_element_output_arcs_count(sc_memory_context const * ctx, sc_addr addr);

sc_uint32 sc_storage_get_element_input_arcs_count(sc_memory_context const * ctx, sc_addr addr);

/*! Get type of sc-element with specified sc-addr
 * @param addr sc-addr of element to get type
 * @param result Pointer to result container
 * @return If input params are correct and type resolved, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result sc_storage_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * result);

/*! Change element subtype
 * @param addr sc-addr of element to set new subtype
 * @param type New type of sc-element
 * @return If type changed, then returns SC_RESULT_OK; otherwise returns SC_RESULT_ERROR
 */
sc_result sc_storage_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type);

/*! Returns sc-addr of begin element of specified arc
 * @param addr sc-addr of arc to get begin element
 * @param result Pointer to result container
 * @return If input params are correct and begin element resolved, then return SC_OK.
 * If element with specified addr isn't an arc, then return SC_INVALID_TYPE
 */
sc_result sc_storage_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result);

/*! Returns sc-addr of end element of specified arc
 * @param addr sc-addr of arc to get end element
 * @param result Pointer to result container
 * @return If input params are correct and end element resolved, then return SC_OK.
 * If element with specified addr isn't an arc, then return SC_INVALID_TYPE
 */
sc_result sc_storage_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result);

/*! Like a sc_storage_get_arc_begin and sc_storage_get_arc_end call
 * @see sc_storage_get_arc_begin, @see sc_storage_get_arc_end
 */
sc_result sc_storage_get_arc_info(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_addr * result_begin_addr,
    sc_addr * result_end_addr);

/*! Setup content data for specified sc-link
 * @param addr sc-addr of sc-link to setup content
 * @param stream Pointer to stream
 * @param is_searchable_string Ability to search for sc-links on this content string
 * @return If content of specified link changed without any errors, then return SC_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_ERROR_INVALID_PARAMS - element with specified \p addr doesn't exist
 * <li>SC_ERROR - unknown error</li>
 * </ul>
 */
sc_result sc_storage_set_link_content(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string);

/*! Returns content data from specified sc-link
 * @param addr sc-addr of sc-link to get content data
 * @param stream Pointer to returned data stream
 * @return If content of specified link returned without any errors, then return SC_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_ERROR_INVALID_PARAMS - element with specified \p addr doesn't exist
 * <li>SC_ERROR - unknown error</li>
 * </ul>
 */
sc_result sc_storage_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream);

/*! Search sc-link addrs by specified data
 * @param stream Pointer to stream that contains data for search
 * @param result_addrs Pointer to result container
 * @return If sc-links with specified content found, then sc-addrs of found link
 * writes into \p result array and function returns SC_OK; otherwise \p function returns SC_OK.
 * In any case \p result_count contains number of found sc-addrs.
 * @attention \p result array need to be free after usage
 */
sc_result sc_storage_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_addrs);

/*! Search sc-link addrs by specified data substring
 * @param stream Pointer to stream that contains data for search
 * @param result_hashes Pointer to result container of sc-links with specified data started with substring
 * @param result_count Container for results count
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @return If sc-links with specified substring found, then sc-addrs of found link
 * writes into \p result array and function returns SC_RESULT_OK; otherwise function returns SC_RESULT_OK.
 * In any case \p result_count contains number of found sc-addrs.
 * @attention \p result array need to be free after usage
 */
sc_result sc_storage_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes,
    sc_uint32 max_length_to_search_as_prefix);

/*! Search sc-strings by specified substring
 * @param stream Pointer to stream that contains data for search
 * @param result_strings Pointer to result container of sc-strings with substring
 * @param max_length_to_search_as_prefix Search by prefix as substring length <= max_length_to_search_as_prefix
 * @return If sc-strings with specified substring found, then they
 * writes into \p result array and function returns SC_RESULT_OK; otherwise function returns SC_RESULT_OK.
 * In any case \p result_count contains number of found sc-strings.
 * @attention \p result array need to be free after usage
 */
sc_result sc_storage_find_links_contents_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_strings,
    sc_uint32 max_length_to_search_as_prefix);

/*! Get statistics information about elements
 * @param stat Pointer to structure that store statistic
 * @return If statistics info collect without any errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result sc_storage_get_elements_stat(sc_stat * stat);

sc_result sc_storage_save(sc_memory_context const * ctx);

#endif
