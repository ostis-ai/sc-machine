/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_container_iterator.h"

#include "../../sc-base/sc_allocator.h"

sc_iterator * sc_iterator_init(sc_struct_node * begin, sc_struct_node * end)
{
  sc_iterator * it = sc_mem_new(sc_iterator, 1);
  it->begin = begin;
  it->end = end;
  it->current = null_ptr;

  return it;
}

sc_bool sc_iterator_next(sc_iterator * it)
{
  if (it == null_ptr || it->begin == null_ptr)
    return SC_FALSE;

  if (it->current == null_ptr)
  {
    it->current = it->begin;
    return it->current != it->end;
  }

  it->current = it->current->next;
  return it->current != it->end;
}

sc_bool sc_iterator_prev(sc_iterator * it)
{
  if (it == null_ptr || it->end == null_ptr)
    return SC_FALSE;

  if (it->current == null_ptr)
  {
    it->current = it->end->prev;
    return it->current != null_ptr;
  }

  it->current = it->current->prev;
  return it->current != null_ptr;
}

void sc_iterator_reset(sc_iterator * it)
{
  it->current = null_ptr;
}

void * sc_iterator_get(sc_iterator * it)
{
  if (it == null_ptr || it->current == null_ptr)
    return null_ptr;

  return it->current->data;
}

sc_bool sc_iterator_destroy(sc_iterator * it)
{
  if (it == null_ptr)
    return SC_FALSE;

  sc_mem_free(it);
  return SC_TRUE;
}
