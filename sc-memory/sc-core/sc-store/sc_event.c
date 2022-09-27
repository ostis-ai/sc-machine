/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.h"

#include "sc_event/sc_event_private.h"
#include "sc_event/sc_event_queue.h"

#include "sc_storage.h"
#include "../sc_memory_private.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_atomic.h"
#include "sc-base/sc_assert_utils.h"
#include "sc-base/sc_message.h"

GMutex events_table_mutex;
#define EVENTS_TABLE_LOCK g_mutex_lock(&events_table_mutex)
#define EVENTS_TABLE_UNLOCK g_mutex_unlock(&events_table_mutex)

#define TABLE_KEY(__Addr) GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(__Addr))

// Pointer to hash table that contains events
GHashTable * events_table = null_ptr;
sc_event_queue * event_queue = null_ptr;

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
  GSList * element_events_list = null_ptr;

  EVENTS_TABLE_LOCK;

  // the first, if table doesn't exist, then create it
  if (events_table == null_ptr)
    events_table = g_hash_table_new(events_table_hash_func, events_table_equal_func);

  // if there are no events for specified sc-element, then create new events list
  element_events_list = (GSList *)g_hash_table_lookup(events_table, TABLE_KEY(event->element));
  element_events_list = g_slist_append(element_events_list, (gpointer)event);
  g_hash_table_insert(events_table, TABLE_KEY(event->element), (gpointer)element_events_list);

  EVENTS_TABLE_UNLOCK;

  return SC_RESULT_OK;
}

//! Remove specified sc-event from events table
sc_result remove_event_from_table(sc_event * event)
{
  GSList * element_events_list = null_ptr;
  sc_assert(events_table != null_ptr);

  EVENTS_TABLE_LOCK;

  element_events_list = (GSList *)g_hash_table_lookup(events_table, TABLE_KEY(event->element));
  if (element_events_list == null_ptr)
  {
    EVENTS_TABLE_UNLOCK;
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  // remove event from list of events for specified sc-element
  element_events_list = g_slist_remove(element_events_list, (gconstpointer)event);
  if (element_events_list == null_ptr)
  {
    g_hash_table_remove(events_table, TABLE_KEY(event->element));
  }
  else
  {
    g_hash_table_insert(events_table, TABLE_KEY(event->element), (gpointer)element_events_list);
  }

  // if there are no more events in table, then delete it
  if (g_hash_table_size(events_table) == 0)
  {
    g_hash_table_destroy(events_table);
    events_table = null_ptr;
  }

  EVENTS_TABLE_UNLOCK;
  return SC_RESULT_OK;
}

/// -----------------------------------------
sc_bool _sc_event_try_emit(sc_event * evt)
{
  sc_bool res = SC_TRUE;

  sc_event_lock(evt);

  if (evt->ref_count & SC_EVENT_REQUEST_DESTROY)
  {
    res = SC_FALSE;
  }
  else
  {
    sc_assert(evt->ref_count < SC_EVENT_REF_COUNT_MASK);
    evt->ref_count++;
  }

  sc_event_unlock(evt);
  return res;
}

sc_bool sc_event_unref(sc_event * evt)
{
  sc_event_lock(evt);
  --evt->ref_count;
  sc_event_unlock(evt);

  return SC_FALSE;
}

// TODO: remove in 0.4.0
sc_event * sc_event_new(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_event_type type,
    sc_pointer data,
    fEventCallback callback,
    fDeleteCallback delete_callback)
{
  sc_assert(callback != null_ptr);

  if (SC_ADDR_IS_EMPTY(el))
    return null_ptr;

  sc_access_levels levels;
  sc_event * event = null_ptr;
  if (sc_storage_get_access_levels(ctx, el, &levels) != SC_RESULT_OK ||
      !sc_access_lvl_check_read(ctx->access_levels, levels))
    return null_ptr;

  sc_storage_element_ref(el);

  event = sc_mem_new(sc_event, 1);
  event->element = el;
  event->type = type;
  event->callback = callback;
  event->delete_callback = delete_callback;
  event->data = data;
  event->thread_lock = null_ptr;
  event->ref_count = 1;
  event->access_levels = ctx->access_levels;

  // register created event
  if (insert_event_into_table(event) != SC_RESULT_OK)
  {
    sc_mem_free(event);
    return null_ptr;
  }

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
  sc_assert(callback != null_ptr);

  if (SC_ADDR_IS_EMPTY(el))
    return null_ptr;

  sc_access_levels levels;
  sc_event * event = null_ptr;
  if (sc_storage_get_access_levels(ctx, el, &levels) != SC_RESULT_OK ||
      !sc_access_lvl_check_read(ctx->access_levels, levels))
    return null_ptr;

  sc_storage_element_ref(el);

  event = sc_mem_new(sc_event, 1);
  event->element = el;
  event->type = type;
  event->callback_ex = callback;
  event->delete_callback = delete_callback;
  event->data = data;
  event->thread_lock = null_ptr;
  event->ref_count = 1;
  event->access_levels = ctx->access_levels;

  // register created event
  if (insert_event_into_table(event) != SC_RESULT_OK)
  {
    sc_mem_free(event);
    return null_ptr;
  }

  return event;
}

sc_result sc_event_destroy(sc_event * evt)
{
  sc_event_lock(evt);
  if (evt->ref_count & SC_EVENT_REQUEST_DESTROY)
  {
    sc_event_unlock(evt);
    goto unref;
  }

  if (remove_event_from_table(evt) != SC_RESULT_OK)
  {
    sc_event_unlock(evt);
    return SC_RESULT_ERROR;
  }

  evt->ref_count |= SC_EVENT_REQUEST_DESTROY;
  evt->callback = null_ptr;
  evt->callback_ex = null_ptr;
  evt->delete_callback = null_ptr;
  sc_event_unlock(evt);

unref:
{
  sc_event_unref(evt);
}

  // wait while will be available for destroy
  while (SC_TRUE)
  {
    sc_event_lock(evt);
    sc_uint32 const refs = evt->ref_count;
    if (refs == SC_EVENT_REQUEST_DESTROY)  // no refs
    {
      sc_storage_element_unref(evt->element);
      if (evt->delete_callback != null_ptr)
        evt->delete_callback(evt);

      sc_event_unlock(evt);
      sc_mem_free(evt);
      break;
    }
    sc_event_unlock(evt);

    g_usleep(10000);
  }

  return SC_RESULT_OK;
}

sc_result sc_event_notify_element_deleted(sc_addr element)
{
  EVENTS_TABLE_LOCK;

  GSList * element_events_list = null_ptr;
  sc_event * evt = null_ptr;

  // do nothing, if there are no registered events
  if (events_table == null_ptr)
    goto result;

  // lookup for all registered to specified sc-element events
  element_events_list = (GSList *)g_hash_table_lookup(events_table, TABLE_KEY(element));
  if (element_events_list)
  {
    g_hash_table_remove(events_table, TABLE_KEY(element));

    while (element_events_list != null_ptr)
    {
      evt = (sc_event *)element_events_list->data;

      // mark event for deletion
      sc_event_lock(evt);
      evt->ref_count |= SC_EVENT_REQUEST_DESTROY;
      sc_event_unlock(evt);

      element_events_list = g_slist_delete_link(element_events_list, element_events_list);
    }
    g_slist_free(element_events_list);
  }

result:
{
  EVENTS_TABLE_UNLOCK;
}

  return SC_RESULT_OK;
}

sc_result sc_event_emit(
    sc_memory_context * ctx,
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
  GSList * element_events_list = null_ptr;
  sc_event * event = null_ptr;

  sc_assert(SC_ADDR_IS_NOT_EMPTY(el));

  EVENTS_TABLE_LOCK;

  // if table is empty, then do nothing
  if (events_table == null_ptr)
    goto result;

  // lookup for all registered to specified sc-element events
  element_events_list = (GSList *)g_hash_table_lookup(events_table, TABLE_KEY(el));

  while (element_events_list != null_ptr)
  {
    event = (sc_event *)element_events_list->data;

    if (event->type == type && sc_access_lvl_check_read(event->access_levels, el_access) &&
        _sc_event_try_emit(event) == SC_TRUE)
    {
      sc_assert(event->callback != null_ptr || event->callback_ex != null_ptr);
      sc_event_queue_append(event_queue, event, edge, other_el);
    }

    element_events_list = element_events_list->next;
  }

result:
{
  EVENTS_TABLE_UNLOCK;
}

  return SC_RESULT_OK;
}

sc_event_type sc_event_get_type(const sc_event * event)
{
  sc_assert(event != null_ptr);
  return event->type;
}

sc_pointer sc_event_get_data(const sc_event * event)
{
  sc_assert(event != null_ptr);
  return event->data;
}

sc_addr sc_event_get_element(const sc_event * event)
{
  sc_assert(event != null_ptr);
  return event->element;
}

void sc_event_lock(sc_event * evt)
{
  sc_pointer thread = sc_thread();
  while (SC_TRUE)
  {
    sc_memory_context * locked_thread = sc_atomic_pointer_get((void **)&evt->thread_lock);
    if (locked_thread == thread)
      return;

    if ((locked_thread != null_ptr) ||
        sc_atomic_pointer_compare_and_exchange((void **)&evt->thread_lock, (void *)locked_thread, (void *)thread) ==
            SC_FALSE)
    {
      g_usleep(rand() % 10);
    }
    else
    {
      break;
    }
  }
}

void sc_event_unlock(sc_event * evt)
{
  sc_pointer thread = sc_thread();
  sc_memory_context * locked_thread = sc_atomic_pointer_get((void **)&evt->thread_lock);
  if (locked_thread != thread)
    sc_critical("Invalid state of event lock");

  sc_atomic_pointer_set((void **)&evt->thread_lock, null_ptr);
}

// --------
sc_bool sc_events_initialize()
{
  event_queue = sc_event_queue_new();
  return SC_TRUE;
}

void sc_events_shutdown()
{
  sc_event_queue_destroy_wait(event_queue);
  event_queue = null_ptr;
}

void sc_events_stop_processing()
{
  sc_event_queue_stop_processing(event_queue);
}
