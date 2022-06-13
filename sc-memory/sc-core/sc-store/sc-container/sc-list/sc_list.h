/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_list_h_
#define _sc_list_h_

#include "../../sc_types.h"

#include "../sc_struct_node.h"
#include "../sc-iterator/sc_container_iterator.h"

//! A sc-list container to store data in linear order
typedef struct _sc_list
{
  sc_struct_node * begin;  // root element in sc-list
  sc_struct_node * end;    // last element in sc-list
  sc_uint32 size;          // sc-list elements size
} sc_list;

/*! Initializes sc-list.
 * @param[out] list Pointer to a sc-list pointer to initialize
 * @returns Returns SC_TRUE, if sc-list didn't exist; otherwise return SC_FALSE.
 */
sc_bool sc_list_init(sc_list ** list);

/*! Destroys a sc-list.
 * @param list A sc-dictionary pointer to destroy
 * @returns Returns SC_TRUE, if a sc-dictionary exists; otherwise return SC_FALSE.
 */
sc_bool sc_list_destroy(sc_list * list);

/*! Free a sc-list values.
 * @param list A sc-dictionary pointer to clear
 * @returns Returns SC_TRUE, if a sc-dictionary exists; otherwise return SC_FALSE.
 */
sc_bool sc_list_clear(sc_list * list);

/*! Pushes data in sc-list node after specified, adding new node.
 * @param list A sc-list pointer
 * @param node A node after that new node inserts to store data
 * @param data A pushable data
 * @returns Returns New added node.
 */
sc_struct_node * sc_list_push(sc_list * list, sc_struct_node * node, void * data);

/*! Pushes data in sc-list node at the end of sc-list, adding new node.
 * @param list A sc-list pointer
 * @param data A pushable data
 * @returns Returns New added node.
 */
sc_struct_node * sc_list_push_back(sc_list * list, void * data);

/*! Pops last node from sc-list.
 * @param list A sc-list pointer
 * @returns Returns Popped node.
 */
sc_struct_node * sc_list_pop_back(sc_list * list);

/*! Removes node from sc-list if predicate for values is SC_TRUE.
 * @param list A sc-list pointer
 * @returns Returns Popped node.
 */
sc_bool sc_list_remove_if(sc_list * list, void * data, sc_bool (*predicate)(void * data, void * other));

/*! Gets sc-list forward-backward iterator.
 * @param list A sc-list pointer
 * @return Returns Iterator.
 */
sc_iterator * sc_list_iterator(sc_list * list);

#endif
