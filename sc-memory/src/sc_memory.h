#ifndef _sc_memory_h_
#define _sc_memory_h_

#include "sc-store/sc_types.h"
#include "sc-store/sc_stream.h"

// Public functions that used by developer

/*! Initialize sc-memory with specified path to repository
 * @param repo_path Path to repository on file system
 */
sc_bool sc_memory_initialize(const sc_char *repo_path);

//! Shutdown sc-memory (save repository to file system)
void sc_memory_shutdown();

//! Check if sc-memory is initialized
sc_bool sc_memory_is_initialized();

/*! Check if sc-element with specified sc-addr exist
 * @param addr sc-addr of element
 * @return Returns SC_TRUE, if sc-element with \p addr exist; otherwise return SC_FALSE.
 * If element deleted, then return SC_FALSE.
 * @note This function is thread safe
 */
sc_bool sc_memory_is_element(sc_addr addr);

//! Remove sc-element from sc-memory
void sc_memory_element_free(sc_addr addr);

/*! Create new sc-node
 * @param type Type of new sc-node
 * @return Return sc-addr of created sc-node
 * @note This function is thread safe
 */
sc_addr sc_memory_node_new(sc_type type);

/*! Create new sc-link
 * @note This function is thread safe
 */
sc_addr sc_memory_link_new();

/*! Create new sc-arc.
 * @param type Type of new sc-arc
 * @param beg sc-addr of begin sc-element
 * @param end sc-addr of end sc-element
 *
 * @return Return sc-addr of created sc-arc
 * @note This function is thread safe
 */
sc_addr sc_memory_arc_new(sc_type type,
                          sc_addr beg,
                          sc_addr end);

/*! Get type of sc-element with specified sc-addr
 * @param addr sc-addr of element to get type
 * @param result Pointer to result container
 * @return If input params are correct and type resolved, then return SC_OK;
 * otherwise return SC_ERROR
 * @note This function is thread safe
 */
sc_result sc_memory_get_element_type(sc_addr addr, sc_type *result);

/*! Returns sc-addr of begin element of specified arc
 * @param addr sc-addr of arc to get begin element
 * @param result Pointer to result container
 * @return If input params are correct and begin element resolved, then return SC_OK.
 * If element with specified addr isn't an arc, then return SC_INVALID_TYPE
 * @note This function is thread safe
 */
sc_result sc_memory_get_arc_begin(sc_addr addr, sc_addr *result);

/*! Returns sc-addr of end element of specified arc
 * @param addr sc-addr of arc to get end element
 * @param result PoOinter to result container
 * @return If input params are correct and end element resolved, then return SC_OK.
 * If element with specified addr isn't an arc, then return SC_INVALID_TYPE
 * @note This function is thread safe
 */
sc_result sc_memory_get_arc_end(sc_addr addr, sc_addr *result);

/*! Setup content data for specified sc-link
 * @param addr sc-addr of sc-link to setup content
 * @param stream Pointer to stream
 * @return If content of specified link changed without any errors, then return SC_OK; otherwise
 * returns on of error codes:
 * <ul>
 * <li>SC_INVALID_TYPE - element with \p addr isn't a sc-link</li>
 * <li>SC_ERROR - unknown error</li>
 * </ul>
 * @note This function is thread safe
 */
sc_result sc_memory_set_link_content(sc_addr addr, const sc_stream *stream);

#endif
