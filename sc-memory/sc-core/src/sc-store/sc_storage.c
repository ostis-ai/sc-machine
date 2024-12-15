/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_storage.h"

#include "sc-core/sc_event_subscription.h"

#include "sc-core/sc_stream_memory.h"
#include "sc-core/sc-base/sc_allocator.h"
#include "sc-core/sc-container/sc_string.h"

#include "sc-core/sc_keynodes.h"

#include "sc_segment.h"
#include "sc_element.h"

#include "sc-fs-memory/sc_fs_memory.h"

#include "sc_storage_private.h"
#include "sc_memory_private.h"

sc_storage * storage = null_ptr;

sc_result sc_storage_initialize(sc_memory_params const * params)
{
  if (sc_fs_memory_initialize_ext(params) != SC_FS_MEMORY_OK)
    return SC_RESULT_ERROR;

  storage = sc_mem_new(sc_storage, 1);
  storage->max_segments_count = params->max_loaded_segments;
  storage->segments_count = 0;
  storage->last_not_engaged_segment_num = 0;
  storage->last_released_segment_num = 0;
  storage->segments = sc_mem_new(sc_segment *, params->max_loaded_segments);
  sc_monitor_init(&storage->segments_monitor);
  _sc_monitor_table_init(&storage->addr_monitors_table);

  sc_memory_info("Sc-memory configuration:");
  sc_message("\tClean on initialize: %s", params->clear ? "On" : "Off");
  sc_message("\tSc-element size: %zd", sizeof(sc_element));
  sc_message("\tSc-segment size: %zd", sizeof(sc_segment));
  sc_message("\tSc-segment elements count: %d", SC_SEGMENT_ELEMENTS_COUNT);
  sc_message("\tSc-storage size: %zd", sizeof(sc_storage));
  sc_message("\tMax segments count: %d", storage->max_segments_count);

  storage->processes_segments_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  sc_monitor_init(&storage->processes_monitor);

  sc_result result = SC_TRUE;
  if (params->clear == SC_FALSE)
  {
    sc_monitor_acquire_write(&storage->segments_monitor);
    result = sc_fs_memory_load(storage) == SC_FS_MEMORY_OK;
    sc_monitor_release_write(&storage->segments_monitor);
  }

  sc_storage_dump_manager_initialize(&storage->dump_manager, params);

  sc_event_subscription_manager_initialize(&storage->events_subscription_manager);
  sc_event_emission_manager_initialize(&storage->events_emission_manager, params);

  return result;
}

sc_result sc_storage_shutdown(sc_bool save_state)
{
  if (storage == null_ptr)
    goto error;

  sc_event_emission_manager_stop(storage->events_emission_manager);
  sc_event_emission_manager_shutdown(storage->events_emission_manager);
  storage->events_emission_manager = null_ptr;
  sc_event_subscription_manager_shutdown(storage->events_subscription_manager);
  storage->events_subscription_manager = null_ptr;

  sc_storage_dump_manager_shutdown(storage->dump_manager);

  if (save_state == SC_TRUE)
  {
    if (sc_fs_memory_save(storage) != SC_FS_MEMORY_OK)
      return SC_RESULT_ERROR;
  }

error:
  if (sc_fs_memory_shutdown() != SC_FS_MEMORY_OK)
    return SC_RESULT_ERROR;

  if (storage == null_ptr)
    return SC_RESULT_NO;

  sc_monitor_acquire_write(&storage->processes_monitor);

  if (storage->processes_segments_table != null_ptr)
  {
    sc_hash_table_destroy(storage->processes_segments_table);
    storage->processes_segments_table = null_ptr;
  }

  sc_monitor_release_write(&storage->processes_monitor);
  sc_monitor_destroy(&storage->processes_monitor);

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
  _sc_monitor_table_destroy(&storage->addr_monitors_table);
  sc_mem_free(storage);
  storage = null_ptr;

  return SC_RESULT_OK;
}

sc_bool sc_storage_is_initialized()
{
  return storage != null_ptr;
}

sc_storage * sc_storage_get()
{
  return storage;
}

sc_event_emission_manager * sc_storage_get_event_emission_manager()
{
  return storage ? storage->events_emission_manager : null_ptr;
}

sc_event_subscription_manager * sc_storage_get_event_subscription_manager()
{
  return storage ? storage->events_subscription_manager : null_ptr;
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

  if (storage == null_ptr || addr.seg == 0 || addr.offset == 0 || addr.seg > storage->max_segments_count
      || addr.offset > SC_SEGMENT_ELEMENTS_COUNT)
    goto error;

  sc_segment * segment = storage->segments[addr.seg - 1];
  if (segment == null_ptr)
    goto error;

  *el = &segment->elements[addr.offset];
  if (((*el)->flags.states & SC_STATE_ELEMENT_EXIST) != SC_STATE_ELEMENT_EXIST)
    goto error;

  result = SC_RESULT_OK;
error:
  return result;
}

sc_result sc_storage_free_element(sc_addr addr)
{
  sc_result result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  sc_element * element;
  if (sc_storage_get_element_by_addr(addr, &element) != SC_RESULT_OK)
    goto error;

  sc_monitor_acquire_read(&storage->segments_monitor);
  sc_segment * segment = storage->segments[addr.seg - 1];
  sc_monitor_release_read(&storage->segments_monitor);
  if (segment == null_ptr)
    goto error;

  sc_monitor_acquire_write(&segment->monitor);
  sc_addr_offset const last_released_offset = segment->last_released_offset;
  segment->elements[addr.offset] = (sc_element){(sc_element_flags){.type = last_released_offset}};
  segment->last_released_offset = addr.offset;
  sc_monitor_release_write(&segment->monitor);

  if (last_released_offset == 0)
  {
    sc_monitor_acquire_write(&storage->segments_monitor);
    segment->elements[0].flags.type = storage->last_released_segment_num;
    storage->last_released_segment_num = segment->num;
    sc_monitor_release_write(&storage->segments_monitor);
  }

  result = SC_RESULT_OK;
error:
  return result;
}

sc_segment * _sc_storage_get_last_not_engaged_segment()
{
  sc_segment * segment = null_ptr;
  sc_addr_seg segment_num;

  do
  {
    segment_num = storage->last_not_engaged_segment_num;
    segment = segment_num == 0 ? null_ptr : storage->segments[segment_num - 1];

    if (segment != null_ptr)
    {
      storage->last_not_engaged_segment_num = segment->elements[0].flags.states;
      segment->elements[0].flags.states = 0;
    }
  }
  while (segment != null_ptr
         && (segment->last_engaged_offset + 1 == SC_SEGMENT_ELEMENTS_COUNT && segment->last_released_offset == 0));

  return segment;
}

sc_segment * _sc_storage_get_new_segment()
{
  sc_segment * segment = null_ptr;
  if (storage->segments_count == storage->max_segments_count)
    goto error;

  segment = storage->segments[storage->segments_count] = sc_segment_new(storage->segments_count + 1);
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

void _sc_storage_check_segment_type(sc_segment ** segment)
{
  sc_monitor_acquire_read(&(*segment)->monitor);
  sc_addr_offset last_released_offset = (*segment)->last_released_offset;
  sc_addr_offset last_engaged_offset = (*segment)->last_engaged_offset;
  sc_monitor_release_read(&(*segment)->monitor);

  if (last_released_offset != 0)
    return;
  else if (last_engaged_offset + 1 == SC_SEGMENT_ELEMENTS_COUNT)
    *segment = null_ptr;
}

sc_segment * _sc_storage_get_segment()
{
  sc_segment * segment = null_ptr;
  sc_thread * thread = sc_thread_self();

  sc_monitor_acquire_read(&storage->processes_monitor);

  if (storage->processes_segments_table != null_ptr)
    segment = (sc_segment *)sc_hash_table_get(storage->processes_segments_table, thread);

  sc_monitor_release_read(&storage->processes_monitor);

  if (segment != null_ptr)
    _sc_storage_check_segment_type(&segment);

  if (segment == null_ptr)
  {
    sc_monitor_acquire_write(&storage->processes_monitor);
    sc_monitor_acquire_write(&storage->segments_monitor);

    segment = _sc_storage_get_last_not_engaged_segment();
    if (segment == null_ptr)
    {
      segment = _sc_storage_get_new_segment();
      if (segment == null_ptr)
        segment = _sc_storage_get_last_free_segment();
    }

    if (segment != null_ptr)
    {
      if (storage->processes_segments_table != null_ptr)
        sc_hash_table_insert(storage->processes_segments_table, thread, segment);
    }

    sc_monitor_release_write(&storage->segments_monitor);
    sc_monitor_release_write(&storage->processes_monitor);
  }

  return segment;
}

sc_element * _sc_storage_get_element(sc_addr * addr)
{
  sc_element * element = null_ptr;
  sc_addr_offset element_offset;

  sc_segment * segment = _sc_storage_get_segment();
  if (segment == null_ptr)
    goto error;

  sc_monitor_acquire_write(&segment->monitor);

  if (segment->last_engaged_offset + 1 != SC_SEGMENT_ELEMENTS_COUNT)
  {
    element_offset = ++segment->last_engaged_offset;
    element = &segment->elements[element_offset];

    *addr = (sc_addr){segment->num, element_offset};
  }
  else if (segment->last_released_offset != 0)
  {
    element_offset = segment->last_released_offset;
    element = &segment->elements[element_offset];
    segment->last_released_offset = element->flags.type;
    element->flags.type = 0;

    *addr = (sc_addr){segment->num, element_offset};
  }

  sc_monitor_release_write(&segment->monitor);

error:
  return element;
}

sc_element * _sc_storage_get_released_element(sc_addr * addr)
{
  sc_segment * segment = null_ptr;
  sc_element * element = null_ptr;
  sc_addr_offset element_offset = 0;

  sc_monitor_acquire_write(&storage->segments_monitor);

  sc_addr_seg segment_num = 0;
new_segment:
{
  segment_num = storage->last_released_segment_num;
  if (segment_num == 0 || segment_num > storage->max_segments_count)
    goto error;
}

  segment = storage->segments[segment_num - 1];

  element_offset = segment->last_released_offset;
  if (segment->last_released_offset == 0)
  {
    storage->last_released_segment_num = segment->elements[0].flags.type;
    segment->elements[0].flags.type = 0;
    goto new_segment;
  }
  else
  {
    element = &segment->elements[element_offset];
    segment->last_released_offset = element->flags.type;
    element->flags.type = 0;
  }

  if (segment->last_released_offset == 0)
  {
    storage->last_released_segment_num = segment->elements[0].flags.type;
    segment->elements[0].flags.type = 0;
  }

error:
  sc_monitor_release_write(&storage->segments_monitor);

  *addr = (sc_addr){segment_num, element_offset};

  return element;
}

sc_element * sc_storage_allocate_new_element(sc_memory_context const * ctx, sc_addr * addr)
{
  *addr = SC_ADDR_EMPTY;
  sc_element * element = null_ptr;

  element = _sc_storage_get_element(addr);
  if (element == null_ptr)
  {
    element = _sc_storage_get_released_element(addr);
    if (element == null_ptr)
      sc_memory_error(
          "Max segments count is %d. SC-memory is full. Please, extends or swap sc-memory",
          storage->max_segments_count);
  }

  if (element != null_ptr)
    element->flags.states |= SC_STATE_ELEMENT_EXIST;

  return element;
}

void sc_storage_start_new_process()
{
  if (storage == null_ptr)
    return;

  sc_thread * thread = sc_thread_self();
  sc_monitor_acquire_write(&storage->processes_monitor);
  if (storage->processes_segments_table == null_ptr)
    goto end;

  sc_hash_table_insert(storage->processes_segments_table, thread, null_ptr);

end:
  sc_monitor_release_write(&storage->processes_monitor);
}

void sc_storage_end_new_process()
{
  if (storage == null_ptr)
    return;

  sc_thread * thread = sc_thread_self();
  sc_monitor_acquire_write(&storage->processes_monitor);
  if (storage->processes_segments_table == null_ptr)
    goto end;

  sc_segment * segment = sc_hash_table_get(storage->processes_segments_table, thread);
  if (segment != null_ptr
      && (segment->last_engaged_offset + 1 != SC_SEGMENT_ELEMENTS_COUNT || segment->last_released_offset != 0))
  {
    sc_monitor_acquire_write(&storage->segments_monitor);

    sc_addr_seg const last_not_engaged_segment_num = storage->last_not_engaged_segment_num;
    segment->elements[0].flags.states = last_not_engaged_segment_num;
    storage->last_not_engaged_segment_num = segment->num;

    sc_monitor_release_write(&storage->segments_monitor);
  }
  sc_hash_table_insert(storage->processes_segments_table, thread, null_ptr);

end:
  sc_monitor_release_write(&storage->processes_monitor);
}

sc_result _sc_storage_element_erase(sc_addr addr)
{
  sc_result result;

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_write(monitor);

  sc_element * element;
  result = sc_storage_get_element_by_addr(addr, &element);
  if (result != SC_RESULT_OK || (element->flags.states & SC_STATE_REQUEST_ERASURE) == SC_STATE_REQUEST_ERASURE)
  {
    sc_monitor_release_write(monitor);
    return result;
  }

  element->flags.states |= SC_STATE_REQUEST_ERASURE;
  sc_type type = element->flags.type;

  sc_monitor_release_write(monitor);

  if (sc_type_has_subtype(type, sc_type_node_link))
    sc_fs_memory_unlink_string(SC_ADDR_LOCAL_TO_INT(addr));
  else if (sc_type_has_subtype_in_mask(type, sc_type_connector_mask))
  {
    sc_bool const is_edge = sc_type_has_subtype(type, sc_type_common_edge);

    sc_addr begin_addr = element->arc.begin;
    sc_addr end_addr = element->arc.end;

    sc_bool const is_not_loop = SC_ADDR_IS_NOT_EQUAL(begin_addr, end_addr);

    sc_monitor * beg_monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, begin_addr);
    sc_monitor * end_monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, end_addr);

    sc_monitor_acquire_write_n(2, beg_monitor, end_monitor);

    // outgoing sc-arcs
    sc_addr prev_out_connector_addr = element->arc.prev_begin_out_arc;
    sc_monitor * prev_out_arc_monitor = null_ptr;
    if (SC_ADDR_IS_NOT_EQUAL(begin_addr, prev_out_connector_addr)
        && SC_ADDR_IS_NOT_EQUAL(end_addr, prev_out_connector_addr))
      prev_out_arc_monitor =
          sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, prev_out_connector_addr);

    sc_addr next_out_connector_addr = element->arc.next_begin_out_arc;
    sc_monitor * next_out_arc_monitor = null_ptr;
    if (SC_ADDR_IS_NOT_EQUAL(begin_addr, next_out_connector_addr)
        && SC_ADDR_IS_NOT_EQUAL(end_addr, next_out_connector_addr))
      next_out_arc_monitor =
          sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, next_out_connector_addr);

    // incoming sc-arcs
    sc_addr prev_in_connector_addr = element->arc.prev_end_in_arc;
    sc_monitor * prev_in_arc_monitor = null_ptr;
    if (SC_ADDR_IS_NOT_EQUAL(begin_addr, prev_in_connector_addr)
        && SC_ADDR_IS_NOT_EQUAL(end_addr, prev_in_connector_addr))
      prev_in_arc_monitor =
          sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, prev_in_connector_addr);

    sc_addr next_in_arc = element->arc.next_end_in_arc;
    sc_monitor * next_in_arc_monitor = null_ptr;
    if (SC_ADDR_IS_NOT_EQUAL(begin_addr, next_in_arc) && SC_ADDR_IS_NOT_EQUAL(end_addr, next_in_arc))
      next_in_arc_monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, next_in_arc);

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
    sc_addr prev_in_arc_from_structure = element->arc.prev_in_arc_from_structure;
    sc_monitor * prev_in_arc_from_structure_monitor = null_ptr;
    if (SC_ADDR_IS_NOT_EQUAL(begin_addr, prev_in_arc_from_structure)
        && SC_ADDR_IS_NOT_EQUAL(end_addr, prev_in_arc_from_structure))
      prev_in_arc_from_structure_monitor =
          sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, prev_in_arc_from_structure);

    sc_addr next_in_arc_from_structure_addr = element->arc.next_in_arc_from_structure;
    sc_monitor * next_in_arc_from_structure_monitor = null_ptr;
    if (SC_ADDR_IS_NOT_EQUAL(begin_addr, next_in_arc_from_structure_addr)
        && SC_ADDR_IS_NOT_EQUAL(end_addr, next_in_arc_from_structure_addr))
      next_in_arc_from_structure_monitor =
          sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, next_in_arc_from_structure_addr);
#endif

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
    sc_monitor_acquire_write_n(
        6,
        prev_out_arc_monitor,
        next_out_arc_monitor,
        prev_in_arc_monitor,
        next_in_arc_monitor,
        prev_in_arc_from_structure_monitor,
        next_in_arc_from_structure_monitor);
#else
    sc_monitor_acquire_write_n(3, prev_out_arc_monitor, next_out_arc_monitor, prev_in_arc_monitor, next_in_arc_monitor);
#endif

    if (SC_ADDR_IS_NOT_EMPTY(prev_out_connector_addr))
    {
      sc_element * prev_el_arc;
      result = sc_storage_get_element_by_addr(prev_out_connector_addr, &prev_el_arc);
      if (result == SC_RESULT_OK)
        prev_el_arc->arc.next_begin_out_arc = next_out_connector_addr;
    }

    if (SC_ADDR_IS_NOT_EMPTY(next_out_connector_addr))
    {
      sc_element * next_el_arc;
      result = sc_storage_get_element_by_addr(next_out_connector_addr, &next_el_arc);
      if (result == SC_RESULT_OK)
        next_el_arc->arc.prev_begin_out_arc = prev_out_connector_addr;
    }

    sc_element * b_el;
    result = sc_storage_get_element_by_addr(begin_addr, &b_el);
    if (result == SC_RESULT_OK)
    {
      if (SC_ADDR_IS_EQUAL(addr, b_el->first_out_arc))
        b_el->first_out_arc = next_out_connector_addr;

      --b_el->outgoing_arcs_count;

      if (is_edge && is_not_loop)
      {
        if (SC_ADDR_IS_EQUAL(addr, b_el->first_in_arc))
          b_el->first_in_arc = next_in_arc;

        --b_el->incoming_arcs_count;
      }
    }

    if (SC_ADDR_IS_NOT_EMPTY(prev_in_connector_addr))
    {
      sc_element * prev_el_arc;
      result = sc_storage_get_element_by_addr(prev_in_connector_addr, &prev_el_arc);
      if (result == SC_RESULT_OK)
        prev_el_arc->arc.next_end_in_arc = next_in_arc;
    }

    if (SC_ADDR_IS_NOT_EMPTY(next_in_arc))
    {
      sc_element * next_el_arc;
      result = sc_storage_get_element_by_addr(next_in_arc, &next_el_arc);
      if (result == SC_RESULT_OK)
        next_el_arc->arc.prev_end_in_arc = prev_in_connector_addr;
    }

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
    if (SC_ADDR_IS_NOT_EMPTY(prev_in_arc_from_structure))
    {
      sc_element * prev_el_arc;
      result = sc_storage_get_element_by_addr(prev_in_arc_from_structure, &prev_el_arc);
      if (result == SC_RESULT_OK)
        prev_el_arc->arc.next_in_arc_from_structure = next_in_arc_from_structure_addr;
    }

    if (SC_ADDR_IS_NOT_EMPTY(next_in_arc_from_structure_addr))
    {
      sc_element * next_el_arc;
      result = sc_storage_get_element_by_addr(next_in_arc_from_structure_addr, &next_el_arc);
      if (result == SC_RESULT_OK)
        next_el_arc->arc.prev_in_arc_from_structure = prev_in_arc_from_structure;
    }
#endif

    sc_element * e_el;
    result = sc_storage_get_element_by_addr(end_addr, &e_el);
    if (result == SC_RESULT_OK)
    {
      if (SC_ADDR_IS_EQUAL(addr, e_el->first_in_arc))
        e_el->first_in_arc = next_in_arc;

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
      if (SC_ADDR_IS_EQUAL(addr, e_el->first_in_arc_from_structure))
        e_el->first_in_arc_from_structure = next_in_arc_from_structure_addr;
#endif

      --e_el->incoming_arcs_count;

      if (is_edge && is_not_loop)
      {
        if (SC_ADDR_IS_EQUAL(addr, e_el->first_out_arc))
          e_el->first_out_arc = next_out_connector_addr;

        --e_el->outgoing_arcs_count;
      }
    }

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
    sc_monitor_release_write_n(
        6,
        prev_out_arc_monitor,
        next_out_arc_monitor,
        prev_in_arc_monitor,
        next_in_arc_monitor,
        prev_in_arc_from_structure_monitor,
        next_in_arc_from_structure_monitor);
#else
    sc_monitor_release_write_n(4, prev_out_arc_monitor, next_out_arc_monitor, prev_in_arc_monitor, next_in_arc_monitor);
#endif
    sc_monitor_release_write_n(2, beg_monitor, end_monitor);
  }

  sc_monitor_acquire_write(monitor);
  sc_storage_free_element(addr);
  sc_monitor_release_write(monitor);

  // erase registered events before deletion
  sc_event_notify_element_deleted(addr);

  return result;
}

sc_result sc_storage_element_erase(sc_memory_context const * ctx, sc_addr addr)
{
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  sc_hash_table * cache_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);

  sc_queue iter_queue;
  sc_queue_init(&iter_queue);
  sc_pointer p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr));
  sc_queue_push(&iter_queue, p_addr);

  sc_queue addrs_with_not_emitted_erase_events;
  sc_queue_init(&addrs_with_not_emitted_erase_events);
  while (!sc_queue_empty(&iter_queue))
  {
    p_addr = sc_queue_pop(&iter_queue);

    sc_addr element_addr;
    element_addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT((sc_pointer_to_sc_addr_hash)p_addr);
    element_addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT((sc_pointer_to_sc_addr_hash)p_addr);

    sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, element_addr);
    sc_monitor_acquire_read(monitor);
    result = sc_storage_get_element_by_addr(element_addr, &el);
    if (result != SC_RESULT_OK)
    {
      sc_monitor_release_read(monitor);
      continue;
    }

    sc_type const type = el->flags.type;
    sc_addr const begin_addr = el->arc.begin;
    sc_addr const end_addr = el->arc.end;

    sc_result erase_incoming_connector_result = SC_RESULT_NO;
    sc_result erase_outgoing_connector_result = SC_RESULT_NO;
    sc_result erase_incoming_arc_result = SC_RESULT_NO;
    sc_result erase_outgoing_arc_result = SC_RESULT_NO;
    sc_result erase_element_result = SC_RESULT_NO;

    if ((el->flags.states & SC_STATE_IS_ERASABLE) != SC_STATE_IS_ERASABLE)
    {
      if ((type & sc_type_connector_mask) != 0)
      {
        erase_incoming_connector_result = sc_event_emit(
            ctx,
            begin_addr,
            sc_event_before_erase_connector_addr,
            element_addr,
            type,
            end_addr,
            sc_storage_element_erase,
            element_addr);
        erase_outgoing_connector_result = sc_event_emit(
            ctx,
            end_addr,
            sc_event_before_erase_connector_addr,
            element_addr,
            type,
            begin_addr,
            sc_storage_element_erase,
            element_addr);
      }

      if (sc_type_has_subtype(type, sc_type_common_edge))
      {
        erase_incoming_arc_result = sc_event_emit(
            ctx,
            begin_addr,
            sc_event_before_erase_edge_addr,
            element_addr,
            type,
            end_addr,
            sc_storage_element_erase,
            element_addr);
        erase_outgoing_arc_result = sc_event_emit(
            ctx,
            end_addr,
            sc_event_before_erase_edge_addr,
            element_addr,
            type,
            begin_addr,
            sc_storage_element_erase,
            element_addr);
      }
      else if (sc_type_has_subtype_in_mask(type, sc_type_arc_mask))
      {
        erase_outgoing_arc_result = sc_event_emit(
            ctx,
            begin_addr,
            sc_event_before_erase_outgoing_arc_addr,
            element_addr,
            type,
            end_addr,
            sc_storage_element_erase,
            element_addr);
        erase_incoming_arc_result = sc_event_emit(
            ctx,
            end_addr,
            sc_event_before_erase_incoming_arc_addr,
            element_addr,
            type,
            begin_addr,
            sc_storage_element_erase,
            element_addr);
      }

      erase_element_result = sc_event_emit(
          ctx,
          element_addr,
          sc_event_before_erase_element_addr,
          SC_ADDR_EMPTY,
          0,
          SC_ADDR_EMPTY,
          sc_storage_element_erase,
          element_addr);

      el->flags.states |= SC_STATE_IS_ERASABLE;
    }

    if (erase_incoming_connector_result == SC_RESULT_OK || erase_outgoing_connector_result == SC_RESULT_OK
        || erase_incoming_arc_result == SC_RESULT_OK || erase_outgoing_arc_result == SC_RESULT_OK
        || erase_element_result == SC_RESULT_OK)
    {
      sc_monitor_release_read(monitor);
      continue;
    }

    sc_queue_push(&addrs_with_not_emitted_erase_events, p_addr);

    sc_addr connector_addr = el->first_out_arc;
    while (SC_ADDR_IS_NOT_EMPTY(connector_addr))
    {
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(connector_addr));

      sc_element * connector = sc_hash_table_get(cache_table, p_addr);
      if (connector == null_ptr)
      {
        result = sc_storage_get_element_by_addr(connector_addr, &connector);
        if (result != SC_RESULT_OK)
          break;

        sc_hash_table_insert(cache_table, p_addr, connector);
        sc_queue_push(&iter_queue, p_addr);
      }

      connector_addr = connector->arc.next_begin_out_arc;
    }

    connector_addr = el->first_in_arc;
    while (SC_ADDR_IS_NOT_EMPTY(connector_addr))
    {
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(connector_addr));

      sc_element * connector = sc_hash_table_get(cache_table, p_addr);
      if (connector == null_ptr)
      {
        result = sc_storage_get_element_by_addr(connector_addr, &connector);
        if (result != SC_RESULT_OK)
          break;

        sc_hash_table_insert(cache_table, p_addr, connector);
        sc_queue_push(&iter_queue, p_addr);
      }

      connector_addr = connector->arc.next_end_in_arc;
    }

    sc_monitor_release_read(monitor);
  }

  sc_queue_destroy(&iter_queue);
  sc_hash_table_destroy(cache_table);

  while (!sc_queue_empty(&addrs_with_not_emitted_erase_events))
  {
    sc_addr_hash addr_int = (sc_pointer_to_sc_addr_hash)sc_queue_pop(&addrs_with_not_emitted_erase_events);
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_int);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_int);

    _sc_storage_element_erase(addr);
  }

  sc_queue_destroy(&addrs_with_not_emitted_erase_events);

  result = SC_RESULT_OK;
error:
  return result;
}

sc_addr sc_storage_node_new(sc_memory_context const * ctx, sc_type type)
{
  sc_result result;
  return sc_storage_node_new_ext(ctx, type, &result);
}

sc_addr sc_storage_node_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result)
{
  sc_addr addr = SC_ADDR_EMPTY;

  if (sc_type_is_not_node(type) && (!sc_type_is(type, sc_type_const) && !sc_type_is(type, sc_type_var)))
  {
    *result = SC_RESULT_ERROR_ELEMENT_IS_NOT_NODE;
    return addr;
  }

  sc_element * element = sc_storage_allocate_new_element(ctx, &addr);
  if (element == null_ptr)
  {
    *result = SC_RESULT_ERROR_FULL_MEMORY;
    return addr;
  }

  element->flags.type = sc_type_node | type;
  *result = SC_RESULT_OK;
  return addr;
}

sc_addr sc_storage_link_new(sc_memory_context const * ctx, sc_type type)
{
  sc_result result;
  return sc_storage_link_new_ext(ctx, type, &result);
}

sc_addr sc_storage_link_new_ext(sc_memory_context const * ctx, sc_type type, sc_result * result)
{
  sc_addr addr = SC_ADDR_EMPTY;

  if (sc_type_is_not_node_link(type))
  {
    *result = SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
    return addr;
  }

  sc_element * element = sc_storage_allocate_new_element(ctx, &addr);
  if (element == null_ptr)
  {
    *result = SC_RESULT_ERROR_FULL_MEMORY;
    return addr;
  }

  element->flags.type = sc_type_node_link | type;
  *result = SC_RESULT_OK;
  return addr;
}

void _sc_storage_make_elements_incident_to_arc(
    sc_addr connector_addr,
    sc_element * arc_el,
    sc_addr beg_addr,
    sc_element * beg_el,
    sc_addr end_addr,
    sc_element * end_el,
    sc_bool is_reverse,
    sc_bool is_loop)
{
  sc_element *first_out_arc = null_ptr, *first_in_arc = null_ptr;

  sc_addr first_out_connector_addr = beg_el->first_out_arc;
  sc_addr first_in_connector_addr = end_el->first_in_arc;

  sc_monitor * first_out_arc_monitor = null_ptr;
  sc_monitor * first_in_arc_monitor = null_ptr;

  if (SC_ADDR_IS_NOT_EQUAL(first_out_connector_addr, beg_addr)
      && SC_ADDR_IS_NOT_EQUAL(first_out_connector_addr, end_addr))
    first_out_arc_monitor =
        sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, first_out_connector_addr);
  if (SC_ADDR_IS_NOT_EQUAL(first_in_connector_addr, beg_addr)
      && SC_ADDR_IS_NOT_EQUAL(first_in_connector_addr, end_addr))
    first_in_arc_monitor =
        sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, first_in_connector_addr);

  sc_monitor_acquire_write_n(2, first_out_arc_monitor, first_in_arc_monitor);

  if (SC_ADDR_IS_NOT_EMPTY(first_out_connector_addr))
    sc_storage_get_element_by_addr(first_out_connector_addr, &first_out_arc);

  if (SC_ADDR_IS_NOT_EMPTY(first_in_connector_addr))
    sc_storage_get_element_by_addr(first_in_connector_addr, &first_in_arc);

  // set next outgoing sc-arc for our generated arc
  if (is_reverse)
  {
    arc_el->arc.next_end_out_arc = first_out_connector_addr;
    arc_el->arc.next_begin_in_arc = first_in_connector_addr;
  }
  else
  {
    arc_el->arc.next_begin_out_arc = first_out_connector_addr;
    arc_el->arc.next_end_in_arc = first_in_connector_addr;

    if (is_loop)
    {
      arc_el->arc.next_end_out_arc = first_out_connector_addr;
      arc_el->arc.next_begin_in_arc = first_in_connector_addr;
    }

    if (first_out_arc)
      first_out_arc->arc.prev_begin_out_arc = connector_addr;

    if (first_in_arc)
      first_in_arc->arc.prev_end_in_arc = connector_addr;
  }

  sc_monitor_release_write_n(2, first_out_arc_monitor, first_in_arc_monitor);

  // set our arc as first output/input at begin/end elements
  beg_el->first_out_arc = connector_addr;
  end_el->first_in_arc = connector_addr;

  ++beg_el->outgoing_arcs_count;
  ++end_el->incoming_arcs_count;
}

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
void _sc_storage_update_structure_arcs(
    sc_addr connector_addr,
    sc_element * arc_el,
    sc_addr beg_addr,
    sc_addr end_addr,
    sc_element * end_el)
{
  sc_element * first_in_accessed_arc = null_ptr;
  sc_addr first_in_accessed_connector_addr = end_el->first_in_arc_from_structure;
  sc_monitor * first_in_accessed_arc_monitor = null_ptr;

  if (SC_ADDR_IS_NOT_EQUAL(first_in_accessed_connector_addr, beg_addr)
      && SC_ADDR_IS_NOT_EQUAL(first_in_accessed_connector_addr, end_addr))
    first_in_accessed_arc_monitor =
        sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, first_in_accessed_connector_addr);

  sc_monitor_acquire_write(first_in_accessed_arc_monitor);

  if (SC_ADDR_IS_NOT_EMPTY(first_in_accessed_connector_addr))
    sc_storage_get_element_by_addr(first_in_accessed_connector_addr, &first_in_accessed_arc);

  arc_el->arc.next_in_arc_from_structure = first_in_accessed_connector_addr;

  if (first_in_accessed_arc)
    first_in_accessed_arc->arc.prev_in_arc_from_structure = connector_addr;

  sc_monitor_release_write(first_in_accessed_arc_monitor);

  end_el->first_in_arc_from_structure = connector_addr;
}
#endif

sc_addr sc_storage_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg_addr, sc_addr end_addr)
{
  sc_result result;
  return sc_storage_arc_new_ext(ctx, type, beg_addr, end_addr, &result);
}

sc_addr sc_storage_arc_new_ext(
    sc_memory_context const * ctx,
    sc_type type,
    sc_addr beg_addr,
    sc_addr end_addr,
    sc_result * result)
{
  sc_addr connector_addr = SC_ADDR_EMPTY;

  if (sc_type_is_not_connector(type))
  {
    *result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    return connector_addr;
  }

  if (SC_ADDR_IS_EMPTY(beg_addr) || SC_ADDR_IS_EMPTY(end_addr))
  {
    *result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
    return connector_addr;
  }

  sc_element *beg_el = null_ptr, *end_el = null_ptr;

  sc_element * arc_el = sc_storage_allocate_new_element(ctx, &connector_addr);
  if (arc_el == null_ptr)
  {
    *result = SC_RESULT_ERROR_FULL_MEMORY;
    return connector_addr;
  }

  arc_el->flags.type = type;
  arc_el->arc.begin = beg_addr;
  arc_el->arc.end = end_addr;

  sc_bool is_edge = sc_type_has_subtype(type, sc_type_common_edge);
  sc_bool is_not_loop = SC_ADDR_IS_NOT_EQUAL(beg_addr, end_addr);

  // try to lock begin and end elements
  sc_monitor * beg_monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, beg_addr);
  sc_monitor * end_monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, end_addr);
  sc_monitor_acquire_write_n(2, beg_monitor, end_monitor);

  *result = sc_storage_get_element_by_addr(beg_addr, &beg_el);
  if (*result != SC_RESULT_OK)
    goto error;

  *result = sc_storage_get_element_by_addr(end_addr, &end_el);
  if (*result != SC_RESULT_OK)
    goto error;

  // lock arcs to change output/input list
  _sc_storage_make_elements_incident_to_arc(
      connector_addr, arc_el, beg_addr, beg_el, end_addr, end_el, SC_FALSE, !is_not_loop);
  if (is_edge && is_not_loop)
    _sc_storage_make_elements_incident_to_arc(
        connector_addr, arc_el, end_addr, end_el, beg_addr, beg_el, SC_TRUE, SC_FALSE);

#ifdef SC_OPTIMIZE_SEARCHING_INCOMING_CONNECTORS_FROM_STRUCTURES
  if (sc_type_is_structure_and_arc(beg_el->flags.type, type))
    _sc_storage_update_structure_arcs(connector_addr, arc_el, beg_addr, end_addr, end_el);
#endif

  // emit events
  if (is_edge && is_not_loop)
  {
    sc_event_emit(
        ctx, end_addr, sc_event_after_generate_edge_addr, connector_addr, type, beg_addr, null_ptr, SC_ADDR_EMPTY);
    sc_event_emit(
        ctx, beg_addr, sc_event_after_generate_edge_addr, connector_addr, type, end_addr, null_ptr, SC_ADDR_EMPTY);
  }
  else
  {
    sc_event_emit(
        ctx,
        beg_addr,
        sc_event_after_generate_outgoing_arc_addr,
        connector_addr,
        type,
        end_addr,
        null_ptr,
        SC_ADDR_EMPTY);
    sc_event_emit(
        ctx,
        end_addr,
        sc_event_after_generate_incoming_arc_addr,
        connector_addr,
        type,
        beg_addr,
        null_ptr,
        SC_ADDR_EMPTY);
  }

  sc_event_emit(
      ctx, end_addr, sc_event_after_generate_connector_addr, connector_addr, type, beg_addr, null_ptr, SC_ADDR_EMPTY);
  sc_event_emit(
      ctx, beg_addr, sc_event_after_generate_connector_addr, connector_addr, type, end_addr, null_ptr, SC_ADDR_EMPTY);

  sc_monitor_release_write_n(2, beg_monitor, end_monitor);

  *result = SC_RESULT_OK;
  return connector_addr;
error:
  sc_storage_free_element(connector_addr);
  sc_monitor_release_write_n(2, beg_monitor, end_monitor);
  return SC_ADDR_EMPTY;
}

sc_uint32 sc_storage_get_element_outgoing_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result)
{
  sc_uint32 count = 0;

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  sc_element * el = null_ptr;
  *result = sc_storage_get_element_by_addr(addr, &el);
  if (*result != SC_RESULT_OK)
    goto error;

  count = el->outgoing_arcs_count;

error:
  sc_monitor_release_read(monitor);
  return count;
}

sc_uint32 sc_storage_get_element_incoming_arcs_count(sc_memory_context const * ctx, sc_addr addr, sc_result * result)
{
  sc_uint32 count = 0;

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  sc_element * el = null_ptr;
  *result = sc_storage_get_element_by_addr(addr, &el);
  if (*result != SC_RESULT_OK)
    goto error;

  count = el->incoming_arcs_count;

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

#define _sc_types_have_not_compatible_mask(type, new_type, mask) \
  ({ \
    sc_type const subtype = type & mask; \
    sc_type const new_subtype = new_type & mask; \
    subtype != sc_type_unknown && subtype != new_subtype; \
  })

sc_bool sc_storage_is_type_extendable_to(sc_type type, sc_type new_type)
{
  if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_element_mask))
    return SC_FALSE;
  if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_constancy_mask))
    return SC_FALSE;

  if (sc_type_is_node_link(type))
  {
    if (sc_type_is_not_node_link(new_type))
      return SC_FALSE;

    type = type & ~sc_type_node_link;
    new_type = new_type & ~sc_type_node_link;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_node_link_mask))
      return SC_FALSE;
  }
  else if (sc_type_is_node(type))
  {
    if (sc_type_is_not_node(new_type))
      return SC_FALSE;

    type = type & ~sc_type_node;
    new_type = new_type & ~sc_type_node;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_node_mask))
      return SC_FALSE;
  }
  else if (sc_type_is_connector(type))
  {
    if (sc_type_is_not_connector(new_type))
      return SC_FALSE;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_connector_mask))
    {
      if (sc_type_is_common_edge(type))
      {
        if (!sc_type_is_common_edge(new_type))
          return SC_FALSE;
      }
      else if (sc_type_is_arc(type))
      {
        if (!sc_type_is_arc(new_type))
          return SC_FALSE;

        if (sc_type_is_common_arc(type))
        {
          if (!sc_type_is_common_arc(new_type))
            return SC_FALSE;
        }
        else if (sc_type_is_membership_arc(type))
        {
          if (!sc_type_is_membership_arc(new_type))
            return SC_FALSE;
        }
      }
    }

    type = type & ~sc_type_connector_mask;
    new_type = new_type & ~sc_type_connector_mask;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_actuality_mask))
      return SC_FALSE;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_permanency_mask))
      return SC_FALSE;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_positivity_mask))
      return SC_FALSE;

    if (_sc_types_have_not_compatible_mask(type, new_type, sc_type_fuz_arc))
      return SC_FALSE;
  }

  return SC_TRUE;
}

sc_result sc_storage_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_write(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (!sc_storage_is_type_extendable_to(el->flags.type, type))
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

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (sc_type_is_not_connector(el->flags.type))
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

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (sc_type_is_not_connector(el->flags.type))
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

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (sc_type_is_not_connector(el->flags.type))
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

sc_result sc_storage_set_link_content(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string)
{
  sc_result result;

  sc_element * el = null_ptr;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) == SC_FALSE)
  {
    sc_mem_free(string);
    return SC_RESULT_ERROR_STREAM_IO;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_write(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (sc_type_is_not_node_link(el->flags.type))
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
    goto error;
  }

  if (sc_fs_memory_link_string_ext(SC_ADDR_LOCAL_TO_INT(addr), string, string_size, is_searchable_string)
      != SC_FS_MEMORY_OK)
  {
    result = SC_RESULT_ERROR_FILE_MEMORY_IO;
    goto error;
  }

  sc_event_emit(
      ctx, addr, sc_event_before_change_link_content_addr, SC_ADDR_EMPTY, 0, SC_ADDR_EMPTY, null_ptr, SC_ADDR_EMPTY);

  sc_monitor_release_write(monitor);
  sc_mem_free(string);

  return SC_RESULT_OK;
error:
  sc_monitor_release_write(monitor);
  sc_mem_free(string);

  return result;
}

sc_result sc_storage_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream)
{
  *stream = null_ptr;
  sc_result result;

  sc_element * el = null_ptr;
  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;

  sc_monitor * monitor = sc_monitor_table_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (sc_type_is_not_node_link(el->flags.type))
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
    goto error;
  }

  sc_fs_memory_status const fs_memory_status =
      sc_fs_memory_get_string_by_link_hash(SC_ADDR_LOCAL_TO_INT(addr), &string, &string_size);
  if (fs_memory_status != SC_FS_MEMORY_OK && fs_memory_status != SC_FS_MEMORY_NO_STRING)
  {
    result = SC_RESULT_ERROR_FILE_MEMORY_IO;
    goto error;
  }

  sc_monitor_release_read(monitor);

  if (string == null_ptr)
    sc_string_empty(string);

  *stream = sc_stream_memory_new(string, string_size, SC_STREAM_FLAG_READ, SC_TRUE);

  return SC_RESULT_OK;
error:
  sc_monitor_release_read(monitor);

  *stream = null_ptr;

  return result;
}

sc_result sc_storage_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  sc_result result = SC_RESULT_OK;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_STREAM_IO;
    goto error;
  }

  if (string == null_ptr)
  {
    string_size = 0;
    sc_string_empty(string);
  }

  sc_fs_memory_status const fs_memory_status =
      sc_fs_memory_get_link_hashes_by_string(string, string_size, data, callback);
  if (fs_memory_status != SC_FS_MEMORY_OK && fs_memory_status != SC_FS_MEMORY_NO_STRING)
    result = SC_RESULT_ERROR_FILE_MEMORY_IO;

  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr))
{
  sc_result result = SC_RESULT_OK;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_STREAM_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  sc_fs_memory_status const fs_memory_status =
      sc_fs_memory_get_link_hashes_by_substring(string, string_size, max_length_to_search_as_prefix, data, callback);
  if (fs_memory_status != SC_FS_MEMORY_OK && fs_memory_status != SC_FS_MEMORY_NO_STRING)
    result = SC_RESULT_ERROR_FILE_MEMORY_IO;

  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_find_links_contents_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_uint32 max_length_to_search_as_prefix,
    void * data,
    void (*callback)(void * data, sc_addr const link_addr, sc_char const * link_content))
{
  sc_result result = SC_RESULT_OK;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_STREAM_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  sc_fs_memory_status const fs_memory_status =
      sc_fs_memory_get_strings_by_substring(string, string_size, max_length_to_search_as_prefix, data, callback);
  if (fs_memory_status != SC_FS_MEMORY_OK && fs_memory_status != SC_FS_MEMORY_NO_STRING)
    result = SC_RESULT_ERROR_FILE_MEMORY_IO;
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

  for (sc_addr_seg i = 0; i < count; ++i)
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
  return sc_fs_memory_save(storage) == SC_FS_MEMORY_OK ? SC_RESULT_OK : SC_RESULT_ERROR;
}
