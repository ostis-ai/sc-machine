/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_struct_node.h"

#include <glib.h>

inline sc_struct_node * sc_struct_node_init(void * data)
{
  sc_struct_node * node = g_new0(sc_struct_node, 1);
  node->next = null_ptr;
  node->prev = null_ptr;

  node->data = data;

  return node;
}
