/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_helper_h_
#define _sc_helper_h_

#define REGEX_SYSTEM_IDTF "([a-z]|[A-Z]|'_'|'.'|[0-9])+"

#include "sc_memory.h"

//! Enumeration of sc-keynodes
enum _sc_keynode
{
  SC_KEYNODE_NREL_SYSTEM_IDENTIFIER = 0,  // nrel_system_identifier
  SC_KEYNODE_COUNT
};

typedef enum _sc_keynode sc_keynode;

typedef struct _sc_system_identifier_fiver
{
  sc_addr addr1;
  sc_addr addr2;
  sc_addr addr3;
  sc_addr addr4;
  sc_addr addr5;
} sc_system_identifier_fiver;

void sc_system_identifier_fiver_make_empty(sc_system_identifier_fiver * fiver);

/*! Finds sc-addr of element with specified system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @param result_addr Pointer to result container
 * @return If sc-element with specified system identifier found, then return SC_RESULT_OK and result_addr
 * contains sc-addr of this one; otherwise return SC_RESULT_ERROR. If there are more then one sc-elements with
 * specified system identifier, then return SC_RESULT_ERROR_INVALID_STATE, but result_addr will contains sc-addr
 * of firstly found sc-element.
 */
_SC_EXTERN sc_result sc_helper_find_element_by_system_identifier(
    sc_memory_context const * ctx,
    sc_char const * data,
    sc_uint32 len,
    sc_addr * result_addr);

/*! Finds sc-addr of element with specified system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @param out_fiver Structure contains the 1th, 2d, 3d, 4th and 5th sc-element address of generated fiver
 *                          addr1 (found sc-element address)
 *               addr4        |
 *    addr5 --------------->  | addr3
 * (nrel_system_identifier)   |
 *                          addr3 (system identifier sc-link)
 * @return If sc-element with specified system identifier found, then return SC_RESULT_OK and result_addr
 * contains sc-addr of this one; otherwise return SC_RESULT_ERROR. If there are more then one sc-elements with
 * specified system identifier, then return SC_RESULT_ERROR_INVALID_STATE, but result_addr will contains sc-addr
 * of firstly found sc-element.
 */
_SC_EXTERN sc_result sc_helper_find_element_by_system_identifier_ext(
    sc_memory_context const * ctx,
    sc_char const * data,
    sc_uint32 len,
    sc_system_identifier_fiver * out_fiver);

/*! Setup new system identifier for specified sc-element
 * @param addr sc-addr of sc-element to setup new system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @remarks If sc-element already has system identifier, then it would be replaced. If system identifier
 * already used for another sc-element, then function returns SC_ERROR_INVALID_PARAMS
 */
_SC_EXTERN sc_result
sc_helper_set_system_identifier(sc_memory_context * ctx, sc_addr addr, sc_char const * data, sc_uint32 len);

/*! Setup new system identifier for specified sc-element
 * @param addr sc-addr of sc-element to setup new system identifier
 * @param data Buffer that contains system identifier for sc-element (must be an UTF-8 encoded)
 * @param len Length of data buffer
 * @param out_fiver Structure contains the 1th, 2d, 3d, 4th and 5th sc-element address of generated fiver
 *                          addr1 (`addr`)
 *               addr4        |
 *    addr5 --------------->  | addr3
 * (nrel_system_identifier)   |
 *                          addr3 (system identifier sc-link)
 * @remarks If sc-element already has system identifier, then it would be replaced. If system identifier
 * already used for another sc-element, then function returns SC_ERROR_INVALID_PARAMS
 */
_SC_EXTERN sc_result sc_helper_set_system_identifier_ext(
    sc_memory_context * ctx,
    sc_addr addr,
    sc_char const * data,
    sc_uint32 len,
    sc_system_identifier_fiver * out_fiver);

/*! Return sc-addr of system identifier for specified sc-element
 * @param el sc-addr of element to get it system identifier
 * @param sys_idtf_addr Pointer to found sc-addr of system identifier
 * @return If system identifier found, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR
 */
_SC_EXTERN sc_result
sc_helper_get_system_identifier_link(sc_memory_context const * ctx, sc_addr el, sc_addr * sys_idtf_addr);

/*! Resolve sc-elemen by specified string system identifier
 * @param system_idtf String that represents system identifier (it will be converted into utf-8)
 * @param result Pointer to result sc-addr container
 * @return If sc-element was founded, then return SC_TRUE; otherwise return SC_FALSE.
 */
_SC_EXTERN sc_bool
sc_helper_resolve_system_identifier(sc_memory_context * ctx, sc_char const * system_idtf, sc_addr * result);

/*! Check if specified arc type exist between two objects
 * @param beg_el sc-addr of begin element
 * @param end_el sc-addr of end element
 * @param arc_type Type of sc-arc to check
 * @return If arc with specified type between beg_el and end_el exist, then return SC_TRUE;
 * otherwise return SC_FALSE
 */
_SC_EXTERN sc_bool sc_helper_check_arc(sc_memory_context const * ctx, sc_addr beg_el, sc_addr end_el, sc_type arc_type);

/*! Check if system identifier is valid
 * @param data system identifier
 * @return If system identifier is valid, then return SC_TRUE; otherwise return SC_FALSE.
 */
_SC_EXTERN sc_result sc_helper_check_system_identifier(sc_char const * data);

#endif  // _sc_helper_h_
