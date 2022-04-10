/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_container_iterator_h_
#define _sc_container_iterator_h_

#include "../../sc_types.h"

#include "../sc_struct_node.h"


typedef struct _sc_iterator
{
  sc_struct_node * begin;
  sc_struct_node * end;
  sc_struct_node * current;
} sc_iterator;


sc_iterator * sc_iterator_init(sc_struct_node * begin, sc_struct_node * end);

sc_bool sc_iterator_next(sc_iterator * it);

sc_bool sc_iterator_prev(sc_iterator * it);

void sc_iterator_reset(sc_iterator * it);

sc_struct_node_data * sc_iterator_get(sc_iterator * it);

void sc_iterator_destroy(sc_iterator * it);

#endif
