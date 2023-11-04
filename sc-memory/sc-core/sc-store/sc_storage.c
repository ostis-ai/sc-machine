/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_storage.h"

#include "sc_segment.h"
#include "sc_element.h"
#include "sc_stream_memory.h"

#include "sc_event/sc_event_private.h"
#include "../sc_memory_private.h"
#include "sc-fs-memory/sc_fs_memory.h"

#include "sc-base/sc_mutex.h"
#include "sc-base/sc_allocator.h"
#include "sc-container/sc-string/sc_string.h"

sc_storage * storage;

sc_bool sc_storage_initialize(sc_memory_params const * params)
{
  if (sc_fs_memory_initialize_ext(params) == SC_FALSE)
    return SC_FALSE;

  storage = sc_mem_new(sc_storage, 1);
  storage->max_segments_count = params->max_loaded_segments;
  storage->segments_count = 0;
  storage->last_not_engaged_segment_num = 0;
  storage->last_released_segment_num = 0;
  storage->segments = sc_mem_new(sc_segment *, params->max_loaded_segments);
  sc_monitor_init(&storage->segments_monitor);
  _sc_monitor_global_init(&storage->addr_monitors_table);

  sc_memory_info("Configuration:");
  sc_message("\tSc-element size: %zd", sizeof(sc_element));
  sc_message("\tSc-segment size: %zd", sizeof(sc_segment));
  sc_message("\tSc-segment elements count: %d", SC_SEGMENT_ELEMENTS_COUNT);
  sc_message("\tSc-storage size: %zd", sizeof(sc_storage));
  sc_message("\tMax segments count: %d", storage->max_segments_count);
  sc_message("\tMax threads count: %d", params->max_threads);
  sc_message("\tSave period: %d", params->save_period);
  sc_message("\tUpdate period: %d", params->update_period);
  sc_message("\tClean on initialize: %s", params->clear ? "On" : "Off");

  sc_result result = SC_TRUE;
  if (params->clear == SC_FALSE)
  {
    sc_monitor_acquire_write(&storage->segments_monitor);
    result = sc_fs_memory_load(storage);
    sc_monitor_release_write(&storage->segments_monitor);
  }

  storage->event_queue = sc_events_initialize_ext(params->max_events_and_agents_threads);

  return result;
}

sc_bool sc_storage_shutdown(sc_bool save_state)
{
  sc_events_stop_processing(storage->event_queue);
  sc_events_shutdown(storage->event_queue);

  if (save_state == SC_TRUE)
  {
    if (sc_fs_memory_save(storage) == SC_FALSE)
      return SC_FALSE;
  }

  if (sc_fs_memory_shutdown() == SC_FALSE)
    return SC_FALSE;

  sc_monitor_acquire_write(&storage->segments_monitor);

  for (sc_addr_seg idx = 0; idx < storage->segments_count; idx++)
  {
    sc_segment * segment = storage->segments[idx];
    if (segment == null_ptr)
      continue;
    sc_segment_free(segment);
  }

  sc_monitor_release_write(&storage->segments_monitor);

  sc_mem_free(storage->segments);
  sc_monitor_destroy(&storage->segments_monitor);
  _sc_monitor_global_destroy(&storage->addr_monitors_table);
  sc_mem_free(storage);
  storage = null_ptr;

  return SC_TRUE;
}

sc_bool sc_storage_is_initialized()
{
  return storage != null_ptr;
}

sc_storage * sc_storage_get()
{
  return storage;
}

sc_bool sc_storage_is_element(sc_memory_context const * ctx, sc_addr addr)
{
  sc_element * el = null_ptr;
  sc_result result = sc_storage_get_element_by_addr(addr, &el);

  return result == SC_RESULT_OK;
}

sc_result sc_storage_get_element_by_addr(sc_addr addr, sc_element ** el)
{
  *el = null_ptr;
  sc_result result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  if (storage == null_ptr || addr.offset == 0 || addr.seg >= storage->max_segments_count ||
      addr.offset >= SC_SEGMENT_ELEMENTS_COUNT)
    goto error;

  sc_segment * segment = storage->segments[addr.seg];
  if (segment == null_ptr)
    goto error;

  *el = sc_segment_get_element_by_offset(segment, addr.offset);
  if (((*el)->flags.access_levels & SC_ACCESS_LVL_ELEMENT_EXIST) != SC_ACCESS_LVL_ELEMENT_EXIST)
    goto error;

  result = SC_RESULT_OK;
error:
  return result;
}

sc_result sc_storage_remove_element_by_addr(sc_addr addr)
{
  sc_result result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  if (SC_ADDR_IS_EMPTY(addr) || addr.seg >= storage->max_segments_count || addr.offset >= SC_SEGMENT_ELEMENTS_COUNT)
    goto error;

  sc_monitor_acquire_read(&storage->segments_monitor);
  sc_segment * segment = storage->segments[addr.seg];
  sc_monitor_release_read(&storage->segments_monitor);
  if (segment == null_ptr)
    goto error;

  sc_monitor_acquire_write(&segment->monitor);
  sc_addr_offset const last_released_offset = segment->last_released_offset;
  segment->elements[addr.offset] = (sc_element){(sc_element_flags){.type = last_released_offset}};

  if (last_released_offset == 0)
  {
    sc_monitor_acquire_write(&storage->segments_monitor);
    storage->last_released_segment_num = segment->elements[0].flags.type;
    sc_monitor_release_write(&storage->segments_monitor);
  }

  segment->last_released_offset = addr.offset;
  sc_monitor_release_write(&segment->monitor);

  result = SC_RESULT_OK;
error:
  return result;
}

sc_segment * _sc_storage_get_new_segment(sc_thread * thread)
{
  sc_segment * segment = null_ptr;
  if (storage->segments_count + 1 == storage->max_segments_count)
    goto error;

  segment = storage->segments[storage->segments_count] = sc_segment_new(storage->segments_count);
  ++storage->segments_count;

error:
  return segment;
}

sc_segment * _sc_storage_get_last_free_segment()
{
  sc_segment * segment = null_ptr;

  if (storage->segments_count == 0)
    goto error;

  sc_addr_seg last_segment_idx = storage->segments_count - 1;
  segment = storage->segments[last_segment_idx];

  if (segment->last_engaged_offset + 1 == SC_SEGMENT_ELEMENTS_COUNT)
  {
    segment = null_ptr;
    goto error;
  }

error:
  return segment;
}

sc_segment * _sc_storage_get_next_segment(sc_thread * thread)
{
  sc_monitor_acquire_write(&storage->event_queue->processes_monitor);

  sc_segment * segment = null_ptr;
  sc_addr_seg const segment_num = storage->last_not_engaged_segment_num;
  if (segment_num != 0)
  {
    sc_monitor_acquire_write(&storage->segments_monitor);
    segment = storage->segments[segment_num];
    if (segment != null_ptr)
    {
      storage->last_not_engaged_segment_num = segment->elements[0].flags.access_levels;
      segment->elements[0].flags.access_levels = 0;
    }
    sc_monitor_release_write(&storage->segments_monitor);
  }

  sc_monitor_release_write(&storage->event_queue->processes_monitor);

  if (segment == null_ptr)
  {
    sc_monitor_acquire_write(&storage->segments_monitor);

    segment = _sc_storage_get_new_segment(thread);
    if (segment == null_ptr)
      segment = _sc_storage_get_last_free_segment();

    sc_monitor_release_write(&storage->segments_monitor);
  }

  if (segment != null_ptr)
  {
    sc_monitor_acquire_write(&storage->event_queue->processes_monitor);
    if (storage->event_queue->processes_segments_table != null_ptr)
      sc_hash_table_insert(storage->event_queue->processes_segments_table, thread, segment);
    sc_monitor_release_write(&storage->event_queue->processes_monitor);
  }

  return segment;
}

void _sc_storage_get_segment(sc_segment ** segment_ptr, sc_bool * released)
{
  *segment_ptr = null_ptr;
  *released = SC_FALSE;
  sc_segment * segment = null_ptr;

  sc_thread * thread = sc_thread_self();
  sc_monitor_acquire_read(&storage->event_queue->processes_monitor);
  if (storage->event_queue->processes_segments_table != null_ptr)
    segment = (sc_segment *)sc_hash_table_get(storage->event_queue->processes_segments_table, thread);
  sc_monitor_release_read(&storage->event_queue->processes_monitor);

  if (segment == null_ptr)
    segment = _sc_storage_get_next_segment(thread);
  else
  {
    sc_monitor_acquire_read(&segment->monitor);
    sc_addr_offset last_engaged_offset = segment->last_engaged_offset;
    sc_addr_offset last_released_offset = segment->last_released_offset;
    sc_monitor_release_read(&segment->monitor);

    if (last_released_offset != 0)
      *released = SC_TRUE;
    else if (last_engaged_offset + 1 == SC_SEGMENT_ELEMENTS_COUNT)
      segment = _sc_storage_get_next_segment(thread);
  }

  *segment_ptr = segment;
}

sc_element * _sc_storage_get_next_element(sc_addr * addr)
{
  sc_element * element = null_ptr;

  sc_segment * segment;
  sc_bool released;
  _sc_storage_get_segment(&segment, &released);
  if (segment == null_ptr)
  {
    sc_memory_error(
        "Max segments count is %d. SC-memory is full. Please, extends or swap sc-memory", storage->max_segments_count);
    goto error;
  }

  sc_monitor_acquire_write(&segment->monitor);

  if (released)
  {
    sc_addr_offset const element_offset = segment->last_released_offset;
    element = &segment->elements[element_offset];
    segment->last_released_offset = element->flags.type;
    element->flags.type = 0;

    if (segment->last_released_offset == 0)
    {
      sc_monitor_acquire_write(&storage->event_queue->processes_monitor);
      storage->last_released_segment_num = segment->elements[0].flags.type;
      sc_monitor_release_write(&storage->event_queue->processes_monitor);

      segment->elements[0].flags.type = 0;
    }

    (*addr) = (sc_addr){segment->num, element_offset};
  }
  else
  {
    sc_addr_offset const element_offset = ++segment->last_engaged_offset;
    element = &segment->elements[element_offset];

    *addr = (sc_addr){segment->num, segment->last_engaged_offset};
  }

  sc_monitor_release_write(&segment->monitor);

error:
  return element;
}

sc_element * _sc_storage_get_released_element(sc_addr * addr)
{
  sc_monitor_acquire_write(&storage->segments_monitor);

  sc_addr_seg const segment_num = storage->last_released_segment_num;
  sc_segment * segment = storage->segments[segment_num];

  sc_addr_offset const element_offset = segment->last_released_offset;
  sc_element * element = &segment->elements[element_offset];
  segment->last_released_offset = element->flags.type;
  element->flags.type = 0;

  if (segment->last_released_offset == 0)
  {
    storage->last_released_segment_num = segment->elements[0].flags.type;
    segment->elements[0].flags.type = 0;
  }

  sc_monitor_release_write(&storage->segments_monitor);

  (*addr) = (sc_addr){segment_num, element_offset};

  return element;
}

sc_element * sc_storage_append_el_into_segments(sc_memory_context const * ctx, sc_addr * addr)
{
  *addr = SC_ADDR_EMPTY;
  sc_element * element = null_ptr;

  element = _sc_storage_get_next_element(addr);
  if (element == null_ptr)
    element = _sc_storage_get_released_element(addr);

  if (element != null_ptr)
    element->flags.access_levels |= SC_ACCESS_LVL_ELEMENT_EXIST;

  return element;
}

void sc_storage_start_new_process()
{
  sc_thread * thread = sc_thread_self();
  sc_monitor_acquire_write(&storage->event_queue->processes_monitor);
  if (storage->event_queue->processes_segments_table == null_ptr)
    goto end;

  sc_hash_table_insert(storage->event_queue->processes_segments_table, thread, null_ptr);

end:
  sc_monitor_release_write(&storage->event_queue->processes_monitor);
}

void sc_storage_end_new_process()
{
  sc_thread * thread = sc_thread_self();
  sc_monitor_acquire_write(&storage->event_queue->processes_monitor);
  if (storage->event_queue->processes_segments_table == null_ptr)
    goto end;

  sc_segment * segment = sc_hash_table_get(storage->event_queue->processes_segments_table, thread);
  if (segment != null_ptr)
  {
    sc_addr_seg const segment_num = storage->last_not_engaged_segment_num;
    sc_monitor_acquire_write(&segment->monitor);
    segment->elements[0].flags.access_levels = segment_num;
    sc_monitor_release_write(&segment->monitor);
    storage->last_not_engaged_segment_num = segment->num;
  }
  sc_hash_table_remove(storage->event_queue->processes_segments_table, thread);

end:
  sc_monitor_release_write(&storage->event_queue->processes_monitor);
}

sc_result sc_storage_element_free(sc_memory_context const * ctx, sc_addr addr)
{
  sc_result result;

  // the first we need to collect and lock all elements
  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  sc_hash_table * cache_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);

  sc_queue * iter_queue;
  sc_queue_init(&iter_queue);
  sc_pointer p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr));
  sc_queue_push(iter_queue, p_addr);

  sc_queue * remove_queue;
  sc_queue_init(&remove_queue);
  sc_queue_push(remove_queue, p_addr);
  while (!sc_queue_empty(iter_queue))
  {
    // get sc-addr for removing
    sc_addr_hash addr_int = (sc_addr_hash)sc_queue_pop(iter_queue);
    sc_addr _addr;
    _addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_int);
    _addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_int);

    result = sc_storage_get_element_by_addr(_addr, &el);
    if (result != SC_RESULT_OK)
      continue;

    sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, _addr);
    sc_monitor_acquire_read(monitor);

    // Iterate all connectors for deleted element and append them into iter_list
    _addr = el->first_out_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));

      sc_element * el2 = sc_hash_table_get(cache_table, p_addr);
      if (el2 == null_ptr)
      {
        result = sc_storage_get_element_by_addr(_addr, &el2);
        if (result != SC_RESULT_OK)
          break;

        sc_hash_table_insert(cache_table, p_addr, el2);
        sc_queue_push(remove_queue, p_addr);
        sc_queue_push(iter_queue, p_addr);
      }

      _addr = el2->arc.next_out_arc;
    }

    _addr = el->first_in_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));

      sc_element * el2 = sc_hash_table_get(cache_table, p_addr);
      if (el2 == null_ptr)
      {
        result = sc_storage_get_element_by_addr(_addr, &el2);
        if (result != SC_RESULT_OK)
          break;

        sc_hash_table_insert(cache_table, p_addr, el2);
        sc_queue_push(remove_queue, p_addr);
        sc_queue_push(iter_queue, p_addr);
      }

      _addr = el2->arc.next_in_arc;
    }

    sc_monitor_release_read(monitor);
  }

  sc_queue_destroy(iter_queue);
  sc_hash_table_destroy(cache_table);

  while (!sc_queue_empty(remove_queue))
  {
    sc_addr_hash addr_int = (sc_addr_hash)sc_queue_pop(remove_queue);
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_int);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_int);

    sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);

    sc_element * element;
    result = sc_storage_get_element_by_addr(addr, &element);
    if (result != SC_RESULT_OK)
      continue;

    if (element->flags.type & sc_type_link)
      sc_fs_memory_unlink_string(SC_ADDR_LOCAL_TO_INT(addr));
    else if (element->flags.type & sc_type_arc_mask)
    {
      sc_addr begin = element->arc.begin;
      sc_addr end = element->arc.end;

      sc_monitor * beg_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, begin);
      sc_monitor * end_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, end);

      sc_monitor_acquire_write_n(2, beg_monitor, end_monitor);

      // output arcs
      sc_addr prev_out_arc = element->arc.prev_out_arc;
      sc_monitor * prev_out_arc_monitor = null_ptr;
      if (SC_ADDR_IS_NOT_EQUAL(begin, prev_out_arc) && SC_ADDR_IS_NOT_EQUAL(end, prev_out_arc))
        prev_out_arc_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, prev_out_arc);

      sc_addr next_out_arc = element->arc.next_out_arc;
      sc_monitor * next_out_arc_monitor = null_ptr;
      if (SC_ADDR_IS_NOT_EQUAL(begin, next_out_arc) && SC_ADDR_IS_NOT_EQUAL(end, next_out_arc))
        next_out_arc_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, next_out_arc);

      // input arcs
      sc_addr prev_in_arc = element->arc.prev_in_arc;
      sc_monitor * prev_in_arc_monitor = null_ptr;
      if (SC_ADDR_IS_NOT_EQUAL(begin, prev_in_arc) && SC_ADDR_IS_NOT_EQUAL(end, prev_in_arc))
        prev_in_arc_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, prev_in_arc);

      sc_addr next_in_arc = element->arc.next_in_arc;
      sc_monitor * next_in_arc_monitor = null_ptr;
      if (SC_ADDR_IS_NOT_EQUAL(begin, next_in_arc) && SC_ADDR_IS_NOT_EQUAL(end, next_in_arc))
        next_in_arc_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, next_in_arc);

      sc_monitor_acquire_write_n(
          4, prev_out_arc_monitor, next_out_arc_monitor, prev_in_arc_monitor, next_in_arc_monitor);

      if (SC_ADDR_IS_NOT_EMPTY(prev_out_arc))
      {
        sc_element * prev_el_arc;
        result = sc_storage_get_element_by_addr(prev_out_arc, &prev_el_arc);
        if (result == SC_RESULT_OK)
          prev_el_arc->arc.next_out_arc = next_out_arc;
      }

      if (SC_ADDR_IS_NOT_EMPTY(next_out_arc))
      {
        sc_element * next_el_arc;
        result = sc_storage_get_element_by_addr(next_out_arc, &next_el_arc);
        if (result == SC_RESULT_OK)
          next_el_arc->arc.prev_out_arc = prev_out_arc;
      }

      sc_element * b_el;
      result = sc_storage_get_element_by_addr(begin, &b_el);
      if (result == SC_RESULT_OK)
      {
        if (SC_ADDR_IS_EQUAL(addr, b_el->first_out_arc))
          b_el->first_out_arc = next_out_arc;

        --b_el->output_arcs_count;
      }

      sc_event_emit(ctx, begin, element->flags.access_levels, SC_EVENT_REMOVE_OUTPUT_ARC, addr, end);

      if (SC_ADDR_IS_NOT_EMPTY(prev_in_arc))
      {
        sc_element * prev_el_arc;
        result = sc_storage_get_element_by_addr(prev_in_arc, &prev_el_arc);
        if (result == SC_RESULT_OK)
          prev_el_arc->arc.next_in_arc = next_in_arc;
      }

      if (SC_ADDR_IS_NOT_EMPTY(next_in_arc))
      {
        sc_element * next_el_arc;
        result = sc_storage_get_element_by_addr(next_in_arc, &next_el_arc);
        if (result == SC_RESULT_OK)
          next_el_arc->arc.prev_in_arc = prev_in_arc;
      }

      sc_element * e_el;
      result = sc_storage_get_element_by_addr(end, &e_el);
      if (result == SC_RESULT_OK)
      {
        if (SC_ADDR_IS_EQUAL(addr, e_el->first_in_arc))
          e_el->first_in_arc = next_in_arc;

        --e_el->input_arcs_count;
      }

      sc_event_emit(ctx, end, element->flags.access_levels, SC_EVENT_REMOVE_INPUT_ARC, addr, begin);

      sc_monitor_release_write_n(
          4, prev_out_arc_monitor, next_out_arc_monitor, prev_in_arc_monitor, next_in_arc_monitor);
      sc_monitor_release_write_n(2, beg_monitor, end_monitor);
    }

    sc_event_emit(ctx, addr, element->flags.access_levels, SC_EVENT_REMOVE_ELEMENT, SC_ADDR_EMPTY, SC_ADDR_EMPTY);

    sc_storage_remove_element_by_addr(addr);

    sc_monitor_release_write(monitor);

    // remove registered events before deletion
    sc_event_notify_element_deleted(addr);
  }

  sc_queue_destroy(remove_queue);

  result = SC_RESULT_OK;
error:
  return result;
}

sc_addr sc_storage_node_new(sc_memory_context const * ctx, sc_type type)
{
  return sc_storage_node_new_ext(ctx, type, ctx->access_levels);
}

sc_addr sc_storage_node_new_ext(sc_memory_context const * ctx, sc_type type, sc_access_levels access_levels)
{
  sc_addr addr = SC_ADDR_EMPTY;

  sc_element * element = sc_storage_append_el_into_segments(ctx, &addr);
  if (element != null_ptr)
  {
    element->flags.type = sc_type_node | type;
    element->flags.access_levels |= access_levels;
  }

  return addr;
}

sc_addr sc_storage_link_new(sc_memory_context const * ctx, sc_type type)
{
  return sc_storage_link_new_ext(ctx, ctx->access_levels, type);
}

sc_addr sc_storage_link_new_ext(sc_memory_context const * ctx, sc_access_levels access_levels, sc_type type)
{
  sc_addr addr = SC_ADDR_EMPTY;

  sc_element * element = sc_storage_append_el_into_segments(ctx, &addr);
  if (element != null_ptr)
  {
    element->flags.type = sc_type_link | type;
    element->flags.access_levels |= access_levels;
  }

  return addr;
}

sc_addr sc_storage_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end)
{
  return sc_storage_arc_new_ext(ctx, type, beg, end, ctx->access_levels);
}

sc_addr sc_storage_arc_new_ext(
    sc_memory_context const * ctx,
    sc_type type,
    sc_addr beg,
    sc_addr end,
    sc_access_levels access_levels)
{
  sc_addr addr = SC_ADDR_EMPTY;
  sc_result result;

  if (SC_ADDR_IS_EMPTY(beg) || SC_ADDR_IS_EMPTY(end))
    return addr;

  sc_element *beg_el = null_ptr, *end_el = null_ptr;
  sc_element *f_out_arc = null_ptr, *f_in_arc = null_ptr;

  sc_element * tmp_el = sc_storage_append_el_into_segments(ctx, &addr);
  tmp_el->flags.type = (type & sc_type_arc_mask) ? type : (sc_type_arc_common | type);
  tmp_el->flags.access_levels |= access_levels;

  // try to lock begin and end elements
  sc_monitor * beg_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, beg);
  sc_monitor * end_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, end);
  sc_monitor_acquire_write_n(2, beg_monitor, end_monitor);

  result = sc_storage_get_element_by_addr(beg, &beg_el);
  if (result != SC_RESULT_OK)
    goto error;

  result = sc_storage_get_element_by_addr(end, &end_el);
  if (result != SC_RESULT_OK)
    goto error;

  // lock arcs to change output/input list
  sc_addr first_out_arc = beg_el->first_out_arc;
  sc_addr first_in_arc = end_el->first_in_arc;

  sc_monitor * first_out_arc_monitor = null_ptr;
  sc_monitor * first_in_arc_monitor = null_ptr;

  if (SC_ADDR_IS_NOT_EQUAL(first_out_arc, beg) && SC_ADDR_IS_NOT_EQUAL(first_out_arc, end))
    first_out_arc_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, first_out_arc);
  if (SC_ADDR_IS_NOT_EQUAL(first_in_arc, beg) && SC_ADDR_IS_NOT_EQUAL(first_in_arc, end))
    first_in_arc_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, first_in_arc);

  sc_monitor_acquire_write_n(2, first_out_arc_monitor, first_in_arc_monitor);

  if (SC_ADDR_IS_NOT_EMPTY(first_out_arc))
    sc_storage_get_element_by_addr(first_out_arc, &f_out_arc);

  if (SC_ADDR_IS_NOT_EMPTY(first_in_arc))
    sc_storage_get_element_by_addr(first_in_arc, &f_in_arc);

  // set next output arc for our created arc
  tmp_el->arc.next_out_arc = first_out_arc;
  tmp_el->arc.next_in_arc = first_in_arc;

  if (f_out_arc)
    f_out_arc->arc.prev_out_arc = addr;

  if (f_in_arc)
    f_in_arc->arc.prev_in_arc = addr;

  sc_monitor_release_write_n(2, first_out_arc_monitor, first_in_arc_monitor);

  // set our arc as first output/input at begin/end elements
  beg_el->first_out_arc = addr;
  end_el->first_in_arc = addr;

  tmp_el->arc.begin = beg;
  tmp_el->arc.end = end;

  ++beg_el->output_arcs_count;
  ++end_el->input_arcs_count;

  // emit events
  sc_event_emit(ctx, beg, beg_el->flags.access_levels, SC_EVENT_ADD_OUTPUT_ARC, addr, end);
  sc_event_emit(ctx, end, beg_el->flags.access_levels, SC_EVENT_ADD_INPUT_ARC, addr, beg);

  sc_monitor_release_write_n(2, beg_monitor, end_monitor);

  return addr;
error:
  sc_storage_remove_element_by_addr(addr);
  sc_monitor_release_write_n(2, beg_monitor, end_monitor);
  return SC_ADDR_EMPTY;
}

sc_uint32 sc_storage_get_element_output_arcs_count(sc_memory_context const * ctx, sc_addr addr)
{
  sc_uint32 count = 0;
  sc_result result;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  count = el->output_arcs_count;

error:
  sc_monitor_release_read(monitor);
  return count;
}

sc_uint32 sc_storage_get_element_input_arcs_count(sc_memory_context const * ctx, sc_addr addr)
{
  sc_uint32 count = 0;
  sc_result result;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  count = el->input_arcs_count;

error:
  sc_monitor_release_read(monitor);
  return count;
}

sc_result sc_storage_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * type)
{
  sc_result result;

  sc_element * el = null_ptr;

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  *type = el->flags.type;

error:
  return result;
}

sc_result sc_storage_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_write(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_element_mask) != (type & sc_type_element_mask))
  {
    result = SC_RESULT_ERROR_INVALID_PARAMS;
    goto error;
  }

  el->flags.type = type;

error:
  sc_monitor_release_write(monitor);
  return result;
}

sc_result sc_storage_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_begin_addr)
{
  *result_begin_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_arc_mask) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    goto error;
  }

  *result_begin_addr = el->arc.begin;

error:
  sc_monitor_release_read(monitor);
  return result;
}

sc_result sc_storage_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_end_addr)
{
  *result_end_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_arc_mask) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    goto error;
  }

  *result_end_addr = el->arc.end;

error:
  sc_monitor_release_read(monitor);
  return result;
}

sc_result sc_storage_get_arc_info(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_addr * result_begin_addr,
    sc_addr * result_end_addr)
{
  *result_begin_addr = SC_ADDR_EMPTY;
  *result_end_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_arc_mask) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    goto error;
  }

  *result_begin_addr = el->arc.begin;
  *result_end_addr = el->arc.end;

error:
  sc_monitor_release_read(monitor);
  return result;
}

sc_result sc_storage_set_link_content(sc_memory_context const * ctx, sc_addr addr, const sc_stream * stream)
{
  return sc_storage_set_link_content_ext(ctx, addr, stream, SC_TRUE);
}

sc_result sc_storage_set_link_content_ext(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string)
{
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_link) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
    goto error;
  }

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) == SC_FALSE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  sc_fs_memory_link_string_ext(SC_ADDR_LOCAL_TO_INT(addr), string, string_size, is_searchable_string);
  sc_mem_free(string);

  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty);
  sc_event_emit(ctx, addr, el->flags.access_levels, SC_EVENT_CONTENT_CHANGED, empty, empty);

error:
  return result;
}

sc_result sc_storage_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream)
{
  *stream = null_ptr;
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (!(el->flags.type & sc_type_link))
  {
    result = SC_RESULT_ERROR_INVALID_TYPE;
    goto error;
  }

  sc_uint32 string_size = 0;
  sc_char * string = null_ptr;
  sc_fs_memory_get_string_by_link_hash(SC_ADDR_LOCAL_TO_INT(addr), &string, &string_size);

  if (string == null_ptr)
    sc_string_empty(string);

  *stream = sc_stream_memory_new(string, string_size, SC_STREAM_FLAG_READ, SC_TRUE);

  return SC_RESULT_OK;

error:
  return result;
}

sc_result sc_storage_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes)
{
  *result_hashes = null_ptr;
  sc_result result;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
  {
    string_size = 0;
    sc_string_empty(string);
  }

  result = sc_fs_memory_get_link_hashes_by_string(string, string_size, result_hashes);
  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes,
    sc_uint32 max_length_to_search_as_prefix)
{
  *result_hashes = null_ptr;
  sc_result result;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  result =
      sc_fs_memory_get_link_hashes_by_substring(string, string_size, max_length_to_search_as_prefix, result_hashes);
  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_find_links_contents_by_content_substring(
    const sc_memory_context * ctx,
    const sc_stream * stream,
    sc_list ** result_strings,
    sc_uint32 max_length_to_search_as_prefix)
{
  *result_strings = null_ptr;
  sc_result result;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  result = sc_fs_memory_get_strings_by_substring(string, string_size, max_length_to_search_as_prefix, result_strings);
  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_get_elements_stat(sc_stat * stat)
{
  sc_mem_set(stat, 0, sizeof(sc_stat));

  sc_monitor_acquire_read(&storage->segments_monitor);
  sc_addr_seg count = storage->segments_count;
  sc_monitor_release_read(&storage->segments_monitor);

  for (sc_addr_seg i = 0; i < storage->segments_count; ++i)
  {
    sc_segment * segment = storage->segments[i];

    sc_monitor_acquire_read(&segment->monitor);
    sc_segment_collect_elements_stat(segment, stat);
    sc_monitor_release_read(&segment->monitor);
  }

  return SC_RESULT_OK;
}

sc_result sc_storage_save(sc_memory_context const * ctx)
{
  return sc_fs_memory_save(storage) ? SC_RESULT_OK : SC_RESULT_ERROR;
}
