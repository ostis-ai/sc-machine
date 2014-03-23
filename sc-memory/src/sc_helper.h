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
 * @return If sc-element with spefcified system identifier found, then return SC_OK and result_addr
 * contains sc-addr of this one; otherwise return SC_ERROR. If there are more then one sc-elements with
 * specified system identifier, then return SC_ERROR_INVALID_STATE, but result_addr will contains sc-addr
 * of firstly found sc-element.
 */
sc_result sc_helper_find_element_by_system_identifier(const sc_char* data, sc_uint32 len, sc_addr *result_addr);

/*! Setup new system identifier for specified sc-element
 * @param addr sc-addr of sc-element to setup new system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @remarks If sc-element already has system identifier, then it would be replaced. If system identifier
 * already used for another sc-element, then function returns SC_ERROR_INVALID_PARAMS
 */
sc_result sc_helper_set_system_identifier(sc_addr addr, const sc_char* data, sc_uint32 len);

/*! Return sc-addr of system identifier for specified sc-element
 * @param el sc-addr of element to get it system identifier
 * @param sys_idtf_addr Pointer to found sc-addr of system identifier
 * @return If system identifier found, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR
 */
sc_result sc_helper_get_system_identifier(sc_addr el, sc_addr *sys_idtf_addr);

/*! Returns sc-addr of specified sc-keynode
 * @param keynode Code of specified sc-keynode
 * @param keynode_addr Pointer to structure, that contains keynode sc-addr
 * @return If specified keynode exist, then return SC_OK and keynode_addr contains sc-addr of this one;
 * otherwise return SC_ERROR
 */
sc_result sc_helper_get_keynode(sc_keynode keynode, sc_addr *keynode_addr);

/*! Resolve sc-elemen by specified string system identifier
 * @param system_idtf String that represents system identifier (it will be converted into utf-8)
 * @param result Pointer to result sc-addr container
 * @return If sc-element was founded, then return SC_TRUE; otherwise return SC_FALSE.
 */
sc_bool sc_helper_resolve_system_identifier(const char *system_idtf, sc_addr *result);

/*! Check if specified arc type exist between two objects
 * @param beg_el sc-addr of begin element
 * @param end_el sc-addr of end element
 * @param arc_type Type of sc-arc to check
 * @return If arc with specified type between beg_el and end_el exist, then return SC_TRUE;
 * otherwise return SC_FALSE
 */
sc_bool sc_helper_check_arc(sc_addr beg_el, sc_addr end_el, sc_type arc_type);


/*! Check if current version equal to specified
 * @return If version equal to current memory version, then function returns SC_TRUE;
 * otherwise it returns SC_FALSE
 */
sc_bool sc_helper_check_version_equal(sc_uint8 major, sc_uint8 minor, sc_uint8 patch);



#endif // IDENTIFICATION_H
