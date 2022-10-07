/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_list.h"

#include "../../sc-base/sc_allocator.h"

sc_bool sc_list_init(sc_list ** list)
{
  if (list == null_ptr)
    return SC_FALSE;

  *list = sc_mem_new(sc_list, 1);
  (*list)->begin = null_ptr;
  (*list)->end = null_ptr;
  (*list)->size = 0;

  return SC_TRUE;
}

sc_bool sc_list_destroy(sc_list * list)
{
  if (list == null_ptr)
    return SC_FALSE;

  sc_struct_node * node = list->begin;
  sc_struct_node * temp;
  while (node != null_ptr)
  {
    temp = node;
    node = node->next;

    sc_mem_free(temp);
  }

  list->begin = null_ptr;
  list->end = null_ptr;
  list->size = 0;

  sc_mem_free(list);

  return SC_TRUE;
}

sc_bool sc_list_clear(sc_list * list)
{
  if (list == null_ptr)
    return SC_FALSE;

  sc_struct_node * node = list->begin;
  while (node != null_ptr)
  {
    sc_mem_free(node->data);
    node->data = null_ptr;
    node = node->next;
  }

  return SC_TRUE;
}

sc_struct_node * sc_list_push(sc_list * list, sc_struct_node * node, void * data)
{
  if (list == null_ptr)
    return null_ptr;

  if (data == null_ptr)
    return null_ptr;

  if (node == null_ptr)
  {
    list->begin = sc_struct_node_init(data);
    list->size = 1;

    if (list->end == null_ptr)
      list->end = sc_mem_new(sc_struct_node, 1);

    list->begin->next = list->end;
    list->end->prev = list->begin;
    list->end->next = null_ptr;
    list->end->data = null_ptr;

    return list->begin;
  }

  sc_struct_node * temp = node->next;
  node->next = sc_struct_node_init(data);
  node->next->prev = node;
  node->next->next = temp;
  temp->prev = node->next;

  ++list->size;

  return node->next;
}

sc_struct_node * sc_list_push_back(sc_list * list, void * data)
{
  return sc_list_push(list, list->end ? list->end->prev : null_ptr, data);
}

sc_struct_node * sc_list_pop_back(sc_list * list)
{
  if (list == null_ptr)
    return null_ptr;

  if (list->size == 0)
    return null_ptr;

  sc_struct_node * temp = list->end->prev;

  if (list->end->prev->prev != null_ptr)
    list->end->prev->prev->next = list->end;

  list->end->prev = list->end->prev->prev;

  --list->size;

  return temp;
}

sc_bool sc_list_remove_if(sc_list * list, void * data, sc_bool (*predicate)(void * data, void * other))
{
  if (list == null_ptr)
    return SC_FALSE;

  sc_bool is_removed = SC_FALSE;
  sc_struct_node * node = list->begin;
  sc_struct_node * temp = null_ptr;
  while (node != null_ptr)
  {
    if (node->data != null_ptr && predicate(node->data, data))
    {
      if (node->prev != null_ptr)
        node->prev->next = node->next;

      if (node->next != null_ptr)
        node->next->prev = node->prev;

      temp = node;
      node = node->next;
      sc_mem_free(temp);
      is_removed = SC_TRUE;

      --list->size;
    }
    else
      node = node->next;
  }

  if (list->size == 0)
    list->begin = null_ptr;

  return is_removed;
}

sc_struct_node * sc_list_front(sc_list * list)
{
  if (list == null_ptr)
    return null_ptr;

  return list->begin;
}

sc_struct_node * sc_list_back(sc_list * list)
{
  if (list == null_ptr)
    return null_ptr;

  if (list->size == 0)
    return null_ptr;

  return list->end->prev;
}

sc_iterator * sc_list_iterator(sc_list * list)
{
  if (list == null_ptr)
    return null_ptr;

  return sc_iterator_init(list->begin, list->end);
}
