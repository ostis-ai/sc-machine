/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.h"

#include "sc_storage.h"
#include "sc_event/sc_event_private.h"
#include "sc_event/sc_event_queue.h"

#include "../sc_memory_private.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_mutex.h"

#define TABLE_KEY(__Addr) GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(__Addr))

// Pointer to hash table that contains events
sc_hash_table * events_table = null_ptr;
sc_monitor events_table_monitor;

guint events_table_hash_func(gconstpointer pointer)
{
  return GPOINTER_TO_UINT(pointer);
}

gboolean events_table_equal_func(gconstpointer a, gconstpointer b)
{
  return (a == b);
}

//! Inserts specified event into events table
sc_result insert_event_into_table(sc_event * event)
{
  sc_hash_table_list * element_events_list = null_ptr;

  // the first, if table doesn't exist, then create it
  if (events_table == null_ptr)
  {
    events_table = sc_hash_table_init(events_table_hash_func, events_table_equal_func, null_ptr, null_ptr);
    sc_monitor_init(&events_table_monitor);
  }

  sc_monitor_acquire_write(&events_table_monitor);

  // if there are no events for specified sc-element, then create new events list
  element_events_list = (sc_hash_table_list *)sc_hash_table_get(events_table, TABLE_KEY(event->element));
  element_events_list = sc_hash_table_list_append(element_events_list, (sc_pointer)event);
  sc_hash_table_insert(events_table, TABLE_KEY(event->element), (sc_pointer)element_events_list);

  sc_monitor_release_write(&events_table_monitor);

  return SC_RESULT_OK;
}

//! Remove specified sc-event from events table
sc_result remove_event_from_table(sc_event * event)
{
  sc_hash_table_list * element_events_list = null_ptr;

  sc_monitor_acquire_write(&events_table_monitor);

  element_events_list = (sc_hash_table_list *)sc_hash_table_get(events_table, TABLE_KEY(event->element));
  if (element_events_list == null_ptr)
    goto error;

  // remove event from list of events for specified sc-element
  element_events_list = sc_hash_table_list_remove(element_events_list, (sc_const_pointer)event);
  if (element_events_list == null_ptr)
    sc_hash_table_remove(events_table, TABLE_KEY(event->element));
  else
    sc_hash_table_insert(events_table, TABLE_KEY(event->element), (sc_pointer)element_events_list);

  // if there are no more events in table, then delete it
  if (sc_hash_table_size(events_table) == 0)
  {
    sc_hash_table_destroy(events_table);
    events_table = null_ptr;
  }

  sc_monitor_release_write(&events_table_monitor);
  return SC_RESULT_OK;
error:
  sc_monitor_release_write(&events_table_monitor);
  return SC_RESULT_ERROR_INVALID_PARAMS;
}

sc_bool sc_event_ref(sc_event * evt)
{
  sc_bool res = SC_TRUE;

  sc_monitor_acquire_read(&evt->monitor);

  if (evt->ref_count & SC_EVENT_REQUEST_DESTROY)
    res = SC_FALSE;

  sc_monitor_release_read(&evt->monitor);
  return res;
}

sc_event * sc_event_new(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_event_type type,
    sc_pointer data,
    fEventCallback callback,
    fDeleteCallback delete_callback)
{
  if (SC_ADDR_IS_EMPTY(el))
    return null_ptr;

  sc_event * event = null_ptr;

  event = sc_mem_new(sc_event, 1);
  event->element = el;
  event->type = type;
  event->callback = callback;
  event->delete_callback = delete_callback;
  event->data = data;
  event->ref_count = 1;
  sc_monitor_init(&event->monitor);
  event->access_levels = ctx->access_levels;

  // register created event
  insert_event_into_table(event);

  return event;
}

sc_event * sc_event_new_ex(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_event_type type,
    sc_pointer data,
    fEventCallbackEx callback,
    fDeleteCallback delete_callback)
{
  if (SC_ADDR_IS_EMPTY(el))
    return null_ptr;

  sc_event * event = null_ptr;

  event = sc_mem_new(sc_event, 1);
  event->element = el;
  event->type = type;
  event->callback_ex = callback;
  event->delete_callback = delete_callback;
  event->data = data;
  event->ref_count = 1;
  sc_monitor_init(&event->monitor);
  event->access_levels = ctx->access_levels;

  // register created event
  insert_event_into_table(event);

  return event;
}

sc_result sc_event_destroy(sc_event * evt)
{
  sc_monitor_acquire_write(&evt->monitor);
  if (remove_event_from_table(evt) != SC_RESULT_OK)
  {
    sc_monitor_release_write(&evt->monitor);
    return SC_RESULT_ERROR;
  }

  if (evt->delete_callback != null_ptr)
    evt->delete_callback(evt);

  evt->ref_count = SC_EVENT_REQUEST_DESTROY;
  evt->element = SC_ADDR_EMPTY;
  evt->type = 0;
  evt->callback_ex = null_ptr;
  evt->delete_callback = null_ptr;
  evt->data = null_ptr;
  evt->access_levels = 0;

  sc_storage * storage = sc_storage_get();
  if (storage != null_ptr)
  {
    sc_monitor_acquire_write(&storage->event_queue->monitor);
    sc_queue_push(storage->event_queue->deletable_events, evt);
    sc_monitor_release_write(&storage->event_queue->monitor);
  }
  sc_monitor_release_write(&evt->monitor);

  return SC_RESULT_OK;
}

sc_result sc_event_notify_element_deleted(sc_addr element)
{
  sc_hash_table_list * element_events_list = null_ptr;
  sc_event * event = null_ptr;

  // do nothing, if there are no registered events
  if (events_table == null_ptr)
    goto result;

  // sc_set_lookup for all registered to specified sc-element events
  if (events_table != null_ptr)
  {
    sc_monitor_acquire_read(&events_table_monitor);
    element_events_list = (sc_hash_table_list *)sc_hash_table_get(events_table, TABLE_KEY(element));
    sc_monitor_release_read(&events_table_monitor);
  }

  if (element_events_list != null_ptr)
  {
    sc_monitor_acquire_write(&events_table_monitor);
    sc_hash_table_remove(events_table, TABLE_KEY(element));
    sc_monitor_release_write(&events_table_monitor);

    while (element_events_list != null_ptr)
    {
      event = (sc_event *)element_events_list->data;

      // mark event for deletion
      sc_monitor_acquire_write(&event->monitor);

      sc_monitor_acquire_write(&sc_storage_get()->event_queue->monitor);
      sc_queue_push(sc_storage_get()->event_queue->deletable_events, event);
      sc_monitor_release_write(&sc_storage_get()->event_queue->monitor);

      sc_monitor_release_write(&event->monitor);

      element_events_list = sc_hash_table_list_remove_sublist(element_events_list, element_events_list);
    }
    sc_hash_table_list_destroy(element_events_list);
  }

result:
  return SC_RESULT_OK;
}

sc_result sc_event_emit(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_access_levels el_access,
    sc_event_type type,
    sc_addr edge,
    sc_addr other_el)
{
  if (ctx->flags & SC_CONTEXT_FLAG_PENDING_EVENTS)
  {
    sc_event_emit_params * params = sc_mem_new(sc_event_emit_params, 1);
    params->el = el;
    params->el_access = el_access;
    params->type = type;
    params->edge = edge;
    params->other_el = other_el;

    sc_memory_context_pend_event(ctx, params);
    return SC_RESULT_OK;
  }

  return sc_event_emit_impl(ctx, el, el_access, type, edge, other_el);
}

sc_result sc_event_emit_impl(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_access_levels el_access,
    sc_event_type type,
    sc_addr edge,
    sc_addr other_el)
{
  sc_hash_table_list * element_events_list = null_ptr;
  sc_event * event = null_ptr;

  if (SC_ADDR_IS_EMPTY(el))
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  // if table is empty, then do nothing
  if (events_table == null_ptr)
    goto result;

  // sc_set_lookup for all registered to specified sc-element events
  sc_monitor_acquire_read(&events_table_monitor);
  if (events_table != null_ptr)
    element_events_list = (sc_hash_table_list *)sc_hash_table_get(events_table, TABLE_KEY(el));
  sc_monitor_release_read(&events_table_monitor);

  while (element_events_list != null_ptr)
  {
    event = (sc_event *)element_events_list->data;

    if (event->type == type && sc_event_ref(event) == SC_TRUE)
      sc_event_queue_append(sc_storage_get()->event_queue, event, edge, other_el);

    element_events_list = element_events_list->next;
  }

result:
  return SC_RESULT_OK;
}

sc_pointer sc_event_get_data(sc_event const * event)
{
  return event->data;
}

sc_addr sc_event_get_element(sc_event const * event)
{
  return event->element;
}

// --------
sc_event_queue * sc_events_initialize_ext(sc_uint32 const max_events_and_agents_threads)
{
  return sc_event_queue_new_ext(max_events_and_agents_threads);
}

void sc_events_shutdown(sc_event_queue * event_queue)
{
  sc_event_queue_destroy_wait(event_queue);
}

void sc_events_stop_processing(sc_event_queue * event_queue)
{
  sc_event_queue_stop_processing(event_queue);
}
