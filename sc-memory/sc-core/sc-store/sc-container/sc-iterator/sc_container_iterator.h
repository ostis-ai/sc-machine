/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_container_iterator_h_
#define _sc_container_iterator_h_

#include "../../sc_types.h"

#include "../sc_struct_node.h"

//! A sc-container iterator to visit elements in sc-containers
typedef struct _sc_iterator
{
  sc_struct_node * begin;    // a start iterating element
  sc_struct_node * end;      // a finite iterating element
  sc_struct_node * current;  // a current iterating element
} sc_iterator;

/*! Initializes sc-container-iterator.
 * @param begin A start element in sc-container
 * @param end A finite element in sc-container
 * @returns Returns A sc-container-iterator pointer
 */
sc_iterator * sc_iterator_init(sc_struct_node * begin, sc_struct_node * end);

/*! Checks next element existence in a sc-container.
 * @param it A sc-container-iterator pointer
 * @returns Returns SC_TRUE, if next element exists in sc-container
 */
sc_bool sc_iterator_next(sc_iterator * it);

/*! Checks previous element existence in a sc-container.
 * @param it A sc-container-iterator pointer
 * @returns Returns SC_TRUE, if a previous element exists in sc-container
 */
sc_bool sc_iterator_prev(sc_iterator * it);

/*! Resets a sc-container-iterator to begin state
 * @param it A sc-container-iterator pointer
 */
void sc_iterator_reset(sc_iterator * it);

/*! Gets data from current element
 * @param it A sc-container-iterator pointer
 */
void * sc_iterator_get(sc_iterator * it);

/*! Destroys a sc-container-iterator
 * @param it A sc-container-iterator pointer
 * @returns Returns SC_TRUE, if a sc-container-iterator pointer exists
 */
sc_bool sc_iterator_destroy(sc_iterator * it);

#endif
