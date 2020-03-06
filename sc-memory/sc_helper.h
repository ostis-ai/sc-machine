/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_helper_h_
#define _sc_helper_h_

#include "sc_memory.h"

//! Enumeration of sc-keynodes
enum _sc_keynode
{
    SC_KEYNODE_NREL_SYSTEM_IDENTIFIER = 0,      // nrel_system_identifier
    SC_KEYNODE_COUNT
};

typedef enum _sc_keynode sc_keynode;


/*! Finds sc-addr of element with specified system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @param result_addr Pointer to result container
 * @return If sc-element with spefcified system identifier found, then return SC_RESULT_OK and result_addr
 * contains sc-addr of this one; otherwise return SC_RESULT_ERROR. If there are more then one sc-elements with
 * specified system identifier, then return SC_RESULT_ERROR_INVALID_STATE, but result_addr will contains sc-addr
 * of firstly found sc-element.
 */
_SC_EXTERN sc_result sc_helper_find_element_by_system_identifier(sc_memory_context const * ctx, const sc_char* data, sc_uint32 len, sc_addr *result_addr);

/*! Setup new system identifier for specified sc-element
 * @param addr sc-addr of sc-element to setup new system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @remarks If sc-element already has system identifier, then it would be replaced. If system identifier
 * already used for another sc-element, then function returns SC_ERROR_INVALID_PARAMS
 */
_SC_EXTERN sc_result sc_helper_set_system_identifier(sc_memory_context const * ctx, sc_addr addr, const sc_char* data, sc_uint32 len);

/*! Return sc-addr of system identifier for specified sc-element
 * @param el sc-addr of element to get it system identifier
 * @param sys_idtf_addr Pointer to found sc-addr of system identifier
 * @return If system identifier found, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR
 */
_SC_EXTERN sc_result sc_helper_get_system_identifier_link(sc_memory_context const * ctx, sc_addr el, sc_addr *sys_idtf_addr);

/*! Returns sc-addr of specified sc-keynode
 * @param keynode Code of specified sc-keynode
 * @param keynode_addr Pointer to structure, that contains keynode sc-addr
 * @return If specified keynode exist, then return SC_OK and keynode_addr contains sc-addr of this one;
 * otherwise return SC_ERROR
 */
_SC_EXTERN sc_result sc_helper_get_keynode(sc_memory_context const * ctx, sc_keynode keynode, sc_addr *keynode_addr);

/*! Resolve sc-elemen by specified string system identifier
 * @param system_idtf String that represents system identifier (it will be converted into utf-8)
 * @param result Pointer to result sc-addr container
 * @return If sc-element was founded, then return SC_TRUE; otherwise return SC_FALSE.
 */
_SC_EXTERN sc_bool sc_helper_resolve_system_identifier(sc_memory_context const * ctx, const char *system_idtf, sc_addr *result);

/*! Check if specified arc type exist between two objects
 * @param beg_el sc-addr of begin element
 * @param end_el sc-addr of end element
 * @param arc_type Type of sc-arc to check
 * @return If arc with specified type between beg_el and end_el exist, then return SC_TRUE;
 * otherwise return SC_FALSE
 */
_SC_EXTERN sc_bool sc_helper_check_arc(sc_memory_context const * ctx, sc_addr beg_el, sc_addr end_el, sc_type arc_type);


/*! Check if current version equal to specified
 * @return If version equal to current memory version, then function returns SC_TRUE;
 * otherwise it returns SC_FALSE
 */
_SC_EXTERN sc_bool sc_helper_check_version_equal(sc_uint8 major, sc_uint8 minor, sc_uint8 patch);



#endif // IDENTIFICATION_H
