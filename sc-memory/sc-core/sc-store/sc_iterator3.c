/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_iterator.h"
#include "sc_element.h"
#include "sc_storage.h"
#include "sc_storage_private.h"

#include "sc-base/sc_allocator.h"

sc_iterator3 * sc_iterator3_f_a_a_new(const sc_memory_context * ctx, sc_addr el, sc_type arc_type, sc_type end_type)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_FALSE;
  p1.addr = el;

  p2.is_type = SC_TRUE;
  p2.type = arc_type;

  p3.is_type = SC_TRUE;
  p3.type = end_type;

  return sc_iterator3_new(ctx, sc_iterator3_f_a_a, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_a_a_f_new(const sc_memory_context * ctx, sc_type beg_type, sc_type arc_type, sc_addr el)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_TRUE;
  p1.type = beg_type;

  p2.is_type = SC_TRUE;
  p2.type = arc_type;

  p3.is_type = SC_FALSE;
  p3.addr = el;

  return sc_iterator3_new(ctx, sc_iterator3_a_a_f, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_f_a_f_new(const sc_memory_context * ctx, sc_addr el_beg, sc_type arc_type, sc_addr el_end)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_FALSE;
  p1.addr = el_beg;

  p2.is_type = SC_TRUE;
  p2.type = arc_type;

  p3.is_type = SC_FALSE;
  p3.addr = el_end;

  return sc_iterator3_new(ctx, sc_iterator3_f_a_f, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_a_f_a_new(
    sc_memory_context const * ctx,
    sc_type beg_type,
    sc_addr arc_addr,
    sc_type end_type)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_TRUE;
  p1.type = beg_type;

  p2.is_type = SC_FALSE;
  p2.addr = arc_addr;

  p3.is_type = SC_TRUE;
  p3.type = end_type;

  return sc_iterator3_new(ctx, sc_iterator3_a_f_a, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_f_f_a_new(
    sc_memory_context const * ctx,
    sc_addr beg_addr,
    sc_addr edge_addr,
    sc_type end_type)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_FALSE;
  p1.addr = beg_addr;

  p2.is_type = SC_FALSE;
  p2.addr = edge_addr;

  p3.is_type = SC_TRUE;
  p3.type = end_type;

  return sc_iterator3_new(ctx, sc_iterator3_f_f_a, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_a_f_f_new(
    sc_memory_context const * ctx,
    sc_type beg_type,
    sc_addr edge_addr,
    sc_addr end_addr)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_TRUE;
  p1.type = beg_type;

  p2.is_type = SC_FALSE;
  p2.addr = edge_addr;

  p3.is_type = SC_FALSE;
  p3.addr = end_addr;

  return sc_iterator3_new(ctx, sc_iterator3_a_f_f, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_f_f_f_new(
    sc_memory_context const * ctx,
    sc_addr beg_addr,
    sc_addr edge_addr,
    sc_addr end_addr)
{
  sc_iterator_param p1, p2, p3;

  p1.is_type = SC_FALSE;
  p1.addr = beg_addr;

  p2.is_type = SC_FALSE;
  p2.addr = edge_addr;

  p3.is_type = SC_FALSE;
  p3.addr = end_addr;

  return sc_iterator3_new(ctx, sc_iterator3_f_f_f, p1, p2, p3);
}

sc_iterator3 * sc_iterator3_new(
    const sc_memory_context * ctx,
    sc_iterator3_type type,
    sc_iterator_param p1,
    sc_iterator_param p2,
    sc_iterator_param p3)
{
  // check types
  if (type >= sc_iterator3_count)
    return null_ptr;

  // check params with template
  switch (type)
  {
  case sc_iterator3_f_a_a:
    if (p1.is_type || !p2.is_type || !p3.is_type)
      return null_ptr;
    break;

  case sc_iterator3_a_a_f:
    if (!p1.is_type || !p2.is_type || p3.is_type)
      return null_ptr;
    break;

  case sc_iterator3_f_a_f:
    if (p1.is_type || !p2.is_type || p3.is_type)
      return null_ptr;
    break;

  case sc_iterator3_a_f_a:
    if (!p1.is_type || p2.is_type || !p3.is_type)
      return null_ptr;
    break;

  case sc_iterator3_f_f_a:
    if (p1.is_type || p2.is_type || !p3.is_type)
      return null_ptr;
    break;

  case sc_iterator3_a_f_f:
    if (!p1.is_type || p2.is_type || p3.is_type)
      return null_ptr;
    break;

  case sc_iterator3_f_f_f:
    if (p1.is_type || p2.is_type || p3.is_type)
      return null_ptr;
    break;

  default:
    break;
  }

  sc_iterator3 * it = sc_mem_new(sc_iterator3, 1);

  it->params[0] = p1;
  it->params[1] = p2;
  it->params[2] = p3;

  it->type = type;
  it->ctx = ctx;
  it->finished = SC_FALSE;

  return it;
}

void sc_iterator3_free(sc_iterator3 * it)
{
  if (it == null_ptr)
    return;

  if ((it->finished == SC_FALSE) && SC_ADDR_IS_NOT_EMPTY(it->results[1]))
  {
    sc_element * el = null_ptr;
    sc_storage_get_element_by_addr(it->results[1], &el);
  }

  sc_mem_free(it);
}

sc_bool _sc_iterator3_f_a_a_next(sc_iterator3 * it)
{
  it->results[0] = it->params[0].addr;

  sc_addr arc_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_monitor * arc_monitor = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[0]);
  sc_monitor_acquire_read(monitor);

  // try to find first output arc
  sc_element * el = null_ptr;
  if (SC_ADDR_IS_EMPTY(it->results[1]))
  {
    result = sc_storage_get_element_by_addr(it->results[0], &el);
    if (result != SC_RESULT_OK)
      goto error;

    arc_addr = el->first_out_arc;
  }
  else
  {
    sc_bool const is_not_same = SC_ADDR_IS_NOT_EQUAL(it->results[0], it->results[1]);
    if (is_not_same)
    {
      arc_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[1]);
      sc_monitor_acquire_read(arc_monitor);
    }

    result = sc_storage_get_element_by_addr(it->results[1], &el);
    if (result != SC_RESULT_OK)
    {
      sc_monitor_release_read(arc_monitor);
      goto error;
    }

    arc_addr = el->arc.next_out_arc;

    if (is_not_same)
      sc_monitor_release_read(arc_monitor);
  }

  // iterate through output arcs
  while (SC_ADDR_IS_NOT_EMPTY(arc_addr))
  {
    sc_bool const is_not_same = SC_ADDR_IS_NOT_EQUAL(it->results[0], arc_addr);
    if (is_not_same)
    {
      arc_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, arc_addr);
      sc_monitor_acquire_read(arc_monitor);
    }

    result = sc_storage_get_element_by_addr(arc_addr, &el);
    if (result != SC_RESULT_OK)
    {
      if (is_not_same)
        sc_monitor_release_read(arc_monitor);
      goto error;
    }

    sc_addr next_out_arc = el->arc.next_out_arc;
    // add check addr

    sc_addr arc_end = el->arc.end;
    sc_type arc_type = el->flags.type;

    if (is_not_same)
      sc_monitor_release_read(arc_monitor);

    sc_type el_type;
    result = sc_storage_get_element_type(it->ctx, arc_end, &el_type);
    if (result != SC_RESULT_OK)
      goto error;

    if (sc_iterator_compare_type(arc_type, it->params[1].type) && sc_iterator_compare_type(el_type, it->params[2].type))
    {
      // store found result
      it->results[1] = arc_addr;
      it->results[2] = arc_end;

      goto success;
    }

    // go to next arc
    arc_addr = next_out_arc;
  }

error:
  sc_monitor_release_read(monitor);
  it->finished = SC_TRUE;
  return SC_FALSE;

success:
  sc_monitor_release_read(monitor);
  return SC_TRUE;
}

sc_bool _sc_iterator3_f_a_f_next(sc_iterator3 * it)
{
  it->results[0] = it->params[0].addr;
  it->results[2] = it->params[2].addr;

  sc_addr arc_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_monitor * arc_monitor = null_ptr;

  sc_monitor * beg_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[0]);
  sc_monitor * end_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[2]);
  sc_monitor_acquire_read_n(2, beg_monitor, end_monitor);

  // try to find first input arc
  sc_element * el = null_ptr;
  if (SC_ADDR_IS_EMPTY(it->results[1]))
  {
    result = sc_storage_get_element_by_addr(it->results[2], &el);
    if (result != SC_RESULT_OK)
      goto error;

    arc_addr = el->first_in_arc;
  }
  else
  {
    sc_bool const is_not_same =
        SC_ADDR_IS_NOT_EQUAL(it->results[0], it->results[1]) && SC_ADDR_IS_NOT_EQUAL(it->results[2], it->results[1]);
    if (is_not_same)
    {
      arc_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[1]);
      sc_monitor_acquire_read(arc_monitor);
    }

    result = sc_storage_get_element_by_addr(it->results[1], &el);
    if (result != SC_RESULT_OK)
    {
      sc_monitor_release_read(arc_monitor);
      goto error;
    }

    arc_addr = el->arc.next_in_arc;

    if (is_not_same)
      sc_monitor_release_read(arc_monitor);
  }

  // trying to find input arc, that created before iterator, and wasn't deleted
  while (SC_ADDR_IS_NOT_EMPTY(arc_addr))
  {
    sc_bool const is_not_same =
        SC_ADDR_IS_NOT_EQUAL(it->results[0], arc_addr) && SC_ADDR_IS_NOT_EQUAL(it->results[2], arc_addr);
    if (is_not_same)
    {
      arc_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, arc_addr);
      sc_monitor_acquire_read(arc_monitor);
    }

    result = sc_storage_get_element_by_addr(arc_addr, &el);
    if (result != SC_RESULT_OK)
    {
      if (is_not_same)
        sc_monitor_release_read(arc_monitor);
      goto error;
    }

    sc_addr next_in_arc = el->arc.next_in_arc;

    sc_type arc_type = el->flags.type;
    sc_addr arc_begin = el->arc.begin;

    if (is_not_same)
      sc_monitor_release_read(arc_monitor);

    if (SC_ADDR_IS_EQUAL(it->params[0].addr, arc_begin) && sc_iterator_compare_type(arc_type, it->params[1].type))
    {
      // store found result
      it->results[1] = arc_addr;
      goto success;
    }

    // go to next arc
    arc_addr = next_in_arc;
  }

error:
  sc_monitor_release_read_n(2, beg_monitor, end_monitor);
  it->finished = SC_TRUE;
  return SC_FALSE;

success:
  sc_monitor_release_read_n(2, beg_monitor, end_monitor);
  return SC_TRUE;
}

sc_bool _sc_iterator3_a_a_f_next(sc_iterator3 * it)
{
  it->results[2] = it->params[2].addr;

  sc_addr arc_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_monitor * arc_monitor;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[2]);
  sc_monitor_acquire_read(monitor);

  // try to find first input arc
  sc_element * el = null_ptr;
  if (SC_ADDR_IS_EMPTY(it->results[1]))
  {
    result = sc_storage_get_element_by_addr(it->results[2], &el);
    if (result != SC_RESULT_OK)
      goto error;

    arc_addr = el->first_in_arc;
  }
  else
  {
    sc_bool const is_not_same = SC_ADDR_IS_NOT_EQUAL(it->results[2], it->results[1]);
    if (is_not_same)
    {
      arc_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[1]);
      sc_monitor_acquire_read(arc_monitor);
    }

    result = sc_storage_get_element_by_addr(it->results[1], &el);
    if (result != SC_RESULT_OK)
    {
      sc_monitor_release_read(arc_monitor);
      goto error;
    }

    arc_addr = el->arc.next_in_arc;

    if (is_not_same)
      sc_monitor_release_read(arc_monitor);
  }

  // trying to find input arc, that created before iterator, and wasn't deleted
  while (SC_ADDR_IS_NOT_EMPTY(arc_addr))
  {
    sc_bool const is_not_same = SC_ADDR_IS_NOT_EQUAL(it->results[2], arc_addr);
    if (is_not_same)
    {
      arc_monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, arc_addr);
      sc_monitor_acquire_read(arc_monitor);
    }

    result = sc_storage_get_element_by_addr(arc_addr, &el);
    if (result != SC_RESULT_OK)
    {
      if (is_not_same)
        sc_monitor_release_read(arc_monitor);
      goto error;
    }

    sc_addr next_in_arc = el->arc.next_in_arc;

    sc_type arc_type = el->flags.type;
    sc_addr arc_begin = el->arc.begin;

    if (is_not_same)
      sc_monitor_release_read(arc_monitor);

    sc_type el_type = 0;
    sc_storage_get_element_type(it->ctx, arc_begin, &el_type);

    if (sc_iterator_compare_type(arc_type, it->params[1].type) && sc_iterator_compare_type(el_type, it->params[0].type))
    {
      // store found result
      it->results[1] = arc_addr;
      it->results[0] = arc_begin;

      goto success;
    }

    // go to next arc
    arc_addr = next_in_arc;
  }

error:
  sc_monitor_release_read(monitor);
  it->finished = SC_TRUE;
  return SC_FALSE;

success:
  sc_monitor_release_read(monitor);
  return SC_TRUE;
}

sc_bool _sc_iterator3_a_f_a_next(sc_iterator3 * it)
{
  it->results[1] = it->params[1].addr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&sc_storage_get()->addr_monitors_table, it->results[1]);
  sc_monitor_acquire_read(monitor);

  sc_element * arc_el;
  sc_result result = sc_storage_get_element_by_addr(it->results[1], &arc_el);
  if (result != SC_RESULT_OK)
    goto error;

  it->results[0] = arc_el->arc.begin;
  it->results[2] = arc_el->arc.end;

  sc_monitor_release_read(monitor);
  it->finished = SC_TRUE;
  return SC_TRUE;

error:
  sc_monitor_release_read(monitor);
  it->finished = SC_TRUE;
  return SC_FALSE;
}

sc_bool _sc_iterator3_f_f_a_next(sc_iterator3 * it)
{
  it->results[1] = it->params[1].addr;

  sc_element * edge_el;
  sc_result result = sc_storage_get_element_by_addr(it->results[1], &edge_el);
  if (result != SC_RESULT_OK)
    goto error;

  if (SC_ADDR_IS_NOT_EQUAL(it->params[0].addr, edge_el->arc.begin))
    goto error;

  it->results[0] = edge_el->arc.begin;
  it->results[2] = edge_el->arc.end;

  it->finished = SC_TRUE;
  return SC_TRUE;

error:
  it->finished = SC_TRUE;
  return SC_FALSE;
}

sc_bool _sc_iterator3_a_f_f_next(sc_iterator3 * it)
{
  it->results[1] = it->params[1].addr;

  sc_element * edge_el;
  sc_result result = sc_storage_get_element_by_addr(it->results[1], &edge_el);
  if (result != SC_RESULT_OK)
    goto error;

  if (SC_ADDR_IS_NOT_EQUAL(it->params[2].addr, edge_el->arc.end))
    goto error;

  it->results[0] = edge_el->arc.begin;
  it->results[2] = edge_el->arc.end;

  it->finished = SC_TRUE;
  return SC_TRUE;

error:
  it->finished = SC_TRUE;
  return SC_FALSE;
}

sc_bool _sc_iterator3_f_f_f_next(sc_iterator3 * it)
{
  it->results[1] = it->params[1].addr;

  sc_element * edge_el;
  sc_result result = sc_storage_get_element_by_addr(it->results[1], &edge_el);
  if (result != SC_RESULT_OK)
    goto error;

  if (SC_ADDR_IS_NOT_EQUAL(it->params[0].addr, edge_el->arc.begin))
    goto error;

  it->results[0] = edge_el->arc.begin;

  if (SC_ADDR_IS_NOT_EQUAL(it->params[2].addr, edge_el->arc.end))
    goto error;

  it->results[2] = edge_el->arc.end;

  it->finished = SC_TRUE;
  return SC_TRUE;

error:
  it->finished = SC_TRUE;
  return SC_FALSE;
}

sc_bool sc_iterator3_next(sc_iterator3 * it)
{
  if ((it == null_ptr) || (it->finished == SC_TRUE))
    return SC_FALSE;

  switch (it->type)
  {
  case sc_iterator3_f_a_a:
    return _sc_iterator3_f_a_a_next(it);

  case sc_iterator3_f_a_f:
    return _sc_iterator3_f_a_f_next(it);

  case sc_iterator3_a_a_f:
    return _sc_iterator3_a_a_f_next(it);

  case sc_iterator3_a_f_a:
    return _sc_iterator3_a_f_a_next(it);

  case sc_iterator3_f_f_a:
    return _sc_iterator3_f_f_a_next(it);

  case sc_iterator3_a_f_f:
    return _sc_iterator3_a_f_f_next(it);

  case sc_iterator3_f_f_f:
    return _sc_iterator3_f_f_f_next(it);

  default:
    break;
  }

  return SC_FALSE;
}

sc_addr sc_iterator3_value(sc_iterator3 * it, sc_uint vid)
{
  return it->results[vid];
}

sc_bool sc_iterator_compare_type(sc_type el_type, sc_type it_type)
{
  if ((it_type & el_type) == it_type)
    return SC_TRUE;

  return SC_FALSE;
}
