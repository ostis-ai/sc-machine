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

typedef struct _sc_list
{
  sc_struct_node * begin;
  sc_struct_node * end;
  sc_uint32 size;
} sc_list;


sc_bool sc_list_init(sc_list ** list);

sc_bool sc_list_destroy(sc_list * list);

sc_struct_node * sc_list_push(sc_list * list, sc_struct_node * node, void * value, sc_uint32 size);

sc_struct_node * sc_list_push_back(sc_list * list, void * value, sc_uint32 size);

sc_struct_node * sc_list_pop_back(sc_list * list);

sc_bool sc_list_remove_if(sc_list * list, void * value, sc_uint32 size, sc_bool (*predicate)(void * value, void * other));

sc_iterator * sc_list_iterator(sc_list * list);


#endif
