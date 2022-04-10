/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_struct_node_h_
#define _sc_struct_node_h_

#include "../sc_types.h"


typedef struct _sc_struct_node_data
{
  void * value;
  sc_uint32 size;
} sc_struct_node_data;

typedef struct _sc_struct_node
{
  struct _sc_struct_node * next;
  struct _sc_struct_node * prev;
  sc_struct_node_data * data;
} sc_struct_node;


sc_struct_node * sc_struct_node_init(void * value, sc_uint32 size);

#endif
