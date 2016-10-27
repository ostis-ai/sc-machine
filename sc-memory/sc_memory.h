/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_h_
#define _sc_memory_h_

#include "sc-store/sc_types.h"
#include "sc-store/sc_stream.h"

// Public functions that used by developer

struct _sc_memory_params
{
    const sc_char *repo_path;
    const sc_char *config_file;
    const sc_char *ext_path;
    sc_bool clear;

};

typedef struct _sc_memory_params sc_memory_params;

//! Function to clear memory parameters
_SC_EXTERN void sc_memory_params_clear(sc_memory_params *params);

/*! Initialize sc-memory with specified path to repository
 * @param params Pointer to initialization parameters
 * @returns Returns pointer to created sc-memory context
 */
_SC_EXTERN sc_memory_context* sc_memory_initialize(const sc_memory_params *params);


//! Shutdown sc-memory (save repository to file system)
_SC_EXTERN void sc_memory_shutdown(sc_bool save_state);

/*! Function that create memory context with specified params
 * @param levels Access levels, you can create it with macros @see sc_access_level_make
 * @returns Retursn pointer to create memory context. If there were any errors during
 * context creation, then function returns 0
 * @note Do not use one context in different threads.
 */
_SC_EXTERN sc_memory_context* sc_memory_context_new(sc_uint8 levels);

/*! Function that destroys created memory context. You can use that function
 * just for contexts, that were created with @see sc_memory_context_new
 */
_SC_EXTERN void sc_memory_context_free(sc_memory_context *ctx);

//! Check if sc-memory is initialized
_SC_EXTERN sc_bool sc_memory_is_initialized();

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns SC_TRUE, if sc-element with \p addr exist; otherwise return SC_FALSE.
 * If element deleted, then return SC_FALSE.

 */
_SC_EXTERN sc_bool sc_memory_is_element(sc_memory_context const * ctx, sc_addr addr);

//! Remove sc-element from sc-memory
_SC_EXTERN sc_result sc_memory_element_free(sc_memory_context const * ctx, sc_addr addr);

/*! Create new sc-node
 * @param type Type of new sc-node
 * @return Return sc-addr of created sc-node
 * @note This function is a thread safe
 */
_SC_EXTERN sc_addr sc_memory_node_new(sc_memory_context const * ctx, sc_type type);

//! Create new sc-link
_SC_EXTERN sc_addr sc_memory_link_new(sc_memory_context const * ctx);

/*! Create new sc-arc.
 * @param type Type of new sc-arc
 * @param beg sc-addr of begin sc-element
 * @param end sc-addr of end sc-element
 *
 * @return Return sc-addr of created sc-arc

 */
_SC_EXTERN sc_addr sc_memory_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end);

/*! Get type of sc-element with specified sc-addr
 * @param addr sc-addr of element to get type
 * @param result Pointer to result container
 * @return If input params are correct and type resolved, then return SC_RESULT_OK;
 * otherwise return SC_RESULT_ERROR

 */
_SC_EXTERN sc_result sc_memory_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * result);

/*! Change element sub-type
 * @param addr sc-addr of element to set new type
 * @param type New sub-type of sc-element (this type must be: type & sc_type_element_mask == 0)
 * @return If sub-type changed, then returns SC_RESULT_OK; otherwise returns SC_RESULT_ERROR

 */
_SC_EXTERN sc_result sc_memory_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type);

/*! Returns sc-addr of begin element of specified arc
 * @param addr sc-addr of arc to get begin element
 * @param result Pointer to result container
 * @return If input params are correct and begin element resolved, then return SC_RESULT_OK.
 * If element with specified addr isn't an arc, then return SC_RESULT_INVALID_TYPE

 */
_SC_EXTERN sc_result sc_memory_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result);

/*! Returns sc-addr of end element of specified arc
 * @param addr sc-addr of arc to get end element
 * @param result Pointer to result container
 * @return If input params are correct and end element resolved, then return SC_RESULT_OK.
 * If element with specified addr isn't an arc, then return SC_RESULT_INVALID_TYPE

 */
_SC_EXTERN sc_result sc_memory_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result);

/*! Returns sc-addr of start and end elements of specified arc.
 * Call logic equal to calls sc_memory_get_arc_begin and sc_memory_get_arc_end,
 * but this one do this work faster.
 */
_SC_EXTERN sc_result sc_memory_get_arc_info(sc_memory_context const * ctx, sc_addr addr,	
                                            sc_addr * result_start_addr, sc_addr * result_end_addr);

/*! Setup content data for specified sc-link
 * @param addr sc-addr of sc-link to setup content
 * @param stream Pointer to stream
 * @return If content of specified link changed without any errors, then return SC_RESULT_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_RESULT_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_RESULT_ERROR - unknown error</li>
 * </ul>

 */
_SC_EXTERN sc_result sc_memory_set_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream const *stream);

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

 */
_SC_EXTERN sc_result sc_memory_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream **stream);

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
_SC_EXTERN sc_result sc_memory_find_links_with_content(sc_memory_context const * ctx, sc_stream const * stream, sc_addr **result, sc_uint32 *result_count);

/*! Free buffer allocated for links content find result
 */
_SC_EXTERN void sc_memory_free_buff(sc_pointer buff);

/*! Setup new access levele for sc-element. New access levels will be a minimum from context access levels and parameter \b access_levels
 * @param addr sc-add of sc-element to change access levels
 * @param access_levels New access levels
 * @param new_value Pointer to structure that contains new value of access levels. This pointer can be a NULL.
 */
_SC_EXTERN sc_result sc_memory_set_element_access_levels(sc_memory_context const * ctx, sc_addr addr, sc_access_levels access_levels, sc_access_levels * new_value);

/*! Get access levels of sc-element
 * @param addr sc-addr of sc-element to get access levels
 * @param result Pointer to container for result
 *
 * @return If access levele returned in \b result, then return SC_RESULT_OK; otherwise returns error code
 */
_SC_EXTERN sc_result sc_memory_get_element_access_levels(sc_memory_context const * ctx, sc_addr addr, sc_access_levels * result);

/*! Collect statistic information about current state of sc-memory
 * @param stat Pointer to structure, that will contains statistics info
 * @return If info collected without errors, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR
 */
_SC_EXTERN sc_result sc_memory_stat(sc_memory_context const * ctx, sc_stat *stat);

/*! Save sc-memory state.
 * Calls from application, when request to save memory state
 */
_SC_EXTERN sc_result sc_memory_save(sc_memory_context const * ctx);



#endif
