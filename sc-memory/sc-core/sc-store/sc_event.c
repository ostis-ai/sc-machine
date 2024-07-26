/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event.h"

#include "sc_storage.h"
#include "sc_storage_private.h"
#include "sc-event/sc_event_private.h"
#include "sc-event/sc_event_queue.h"

#include "../sc_memory_context_manager.h"
#include "../sc_memory_context_private.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_mutex.h"

/*! Structure representing an sc-event registration manager.
 * @note This structure manages the registration and removal of sc-events associated with sc-elements.
 */
struct _sc_event_registration_manager
{
  sc_hash_table * events_table;     ///< Hash table containing registered events.
  sc_monitor events_table_monitor;  ///< Monitor for synchronizing access to the events table.
};

#define TABLE_KEY(__Addr) GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(__Addr))

// Pointer to hash table that contains events

guint events_table_hash_func(gconstpointer pointer)
{
  return GPOINTER_TO_UINT(pointer);
}

gboolean events_table_equal_func(gconstpointer a, gconstpointer b)
{
  return (a == b);
}

/*! Adds the specified sc-event to the registration manager's events table.
 * @param manager Pointer to the sc-event registration manager.
 * @param event Pointer to the sc-event to be added.
 * @return Returns SC_RESULT_OK if the operation is successful, SC_RESULT_NO otherwise.
 */
sc_result _sc_event_registration_manager_add(sc_event_registration_manager * manager, sc_event * event)
{
  sc_hash_table_list * element_events_list = null_ptr;

  // the first, if table doesn't exist, then return error
  if (manager == null_ptr)
    return SC_RESULT_NO;

  sc_monitor_acquire_write(&manager->events_table_monitor);

  if (manager->events_table == null_ptr)
  {
    sc_monitor_release_write(&manager->events_table_monitor);
    return SC_RESULT_NO;
  }

  // if there are no events for specified sc-element, then create new events list
  element_events_list =
      (sc_hash_table_list *)sc_hash_table_get(manager->events_table, TABLE_KEY(event->subscription_addr));
  element_events_list = sc_hash_table_list_append(element_events_list, (sc_pointer)event);
  sc_hash_table_insert(manager->events_table, TABLE_KEY(event->subscription_addr), (sc_pointer)element_events_list);

  sc_monitor_release_write(&manager->events_table_monitor);

  return SC_RESULT_OK;
}

/*! Removes the specified sc-event from the registration manager's events table.
 * @param manager Pointer to the sc-event registration manager.
 * @param event Pointer to the sc-event to be removed.
 * @return Returns SC_RESULT_OK if the operation is successful, SC_RESULT_ERROR_INVALID_PARAMS otherwise.
 */
sc_result _sc_event_registration_manager_remove(sc_event_registration_manager * manager, sc_event * event)
{
  sc_hash_table_list * element_events_list = null_ptr;

  // the first, if table doesn't exist, then return error
  if (manager == null_ptr)
    return SC_RESULT_NO;

  sc_monitor_acquire_write(&manager->events_table_monitor);
  element_events_list =
      (sc_hash_table_list *)sc_hash_table_get(manager->events_table, TABLE_KEY(event->subscription_addr));
  if (element_events_list == null_ptr)
    goto error;

  if (manager->events_table == null_ptr)
    goto error;

  // remove event from list of events for specified sc-element
  element_events_list = sc_hash_table_list_remove(element_events_list, (sc_const_pointer)event);
  if (element_events_list == null_ptr)
    sc_hash_table_remove(manager->events_table, TABLE_KEY(event->subscription_addr));
  else
    sc_hash_table_insert(manager->events_table, TABLE_KEY(event->subscription_addr), (sc_pointer)element_events_list);

  sc_monitor_release_write(&manager->events_table_monitor);
  return SC_RESULT_OK;
error:
  sc_monitor_release_write(&manager->events_table_monitor);
  return SC_RESULT_ERROR_INVALID_PARAMS;
}

void sc_event_registration_manager_initialize(sc_event_registration_manager ** manager)
{
  (*manager) = sc_mem_new(sc_event_registration_manager, 1);
  (*manager)->events_table = sc_hash_table_init(events_table_hash_func, events_table_equal_func, null_ptr, null_ptr);
  sc_monitor_init(&(*manager)->events_table_monitor);
}

void sc_event_registration_manager_shutdown(sc_event_registration_manager * manager)
{
  sc_monitor_destroy(&manager->events_table_monitor);
  sc_hash_table_destroy(manager->events_table);
  sc_mem_free(manager);
}

sc_event * sc_event_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_pointer data,
    sc_event_callback callback,
    sc_event_delete_function delete_callback)
{
  sc_unused(ctx);

  if (SC_ADDR_IS_EMPTY(subscription_addr))
    return null_ptr;

  sc_event * event = null_ptr;

  event = sc_mem_new(sc_event, 1);
  event->subscription_addr = subscription_addr;
  event->type = type;
  event->event_element_type = 0;
  event->callback = callback;
  event->callback_ext = null_ptr;
  event->callback_with_user = null_ptr;
  event->delete_callback = delete_callback;
  event->data = data;
  event->ref_count = 1;
  sc_monitor_init(&event->monitor);

  // register created event
  sc_event_registration_manager * manager = sc_storage_get_event_registration_manager();
  _sc_event_registration_manager_add(manager, event);

  return event;
}

sc_event * sc_event_new_ex(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_pointer data,
    sc_event_callback_ext callback,
    sc_event_delete_function delete_callback)
{
  sc_unused(ctx);

  if (SC_ADDR_IS_EMPTY(subscription_addr))
    return null_ptr;

  sc_event * event = null_ptr;

  event = sc_mem_new(sc_event, 1);
  event->subscription_addr = subscription_addr;
  event->type = type;
  event->event_element_type = 0;
  event->callback = null_ptr;
  event->callback_ext = callback;
  event->callback_with_user = null_ptr;
  event->delete_callback = delete_callback;
  event->data = data;
  event->ref_count = 1;
  sc_monitor_init(&event->monitor);

  // register created event
  sc_event_registration_manager * manager = sc_storage_get_event_registration_manager();
  _sc_event_registration_manager_add(manager, event);

  return event;
}

sc_event * sc_event_with_user_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_type event_element_type,
    sc_pointer data,
    sc_event_callback_with_user callback,
    sc_event_delete_function delete_callback)
{
  sc_unused(ctx);

  if (SC_ADDR_IS_EMPTY(subscription_addr))
    return null_ptr;

  sc_event * event = null_ptr;

  event = sc_mem_new(sc_event, 1);
  event->subscription_addr = subscription_addr;
  event->type = type;
  event->event_element_type = event_element_type;
  event->callback = null_ptr;
  event->callback_ext = null_ptr;
  event->callback_with_user = callback;
  event->delete_callback = delete_callback;
  event->data = data;
  event->ref_count = 1;
  sc_monitor_init(&event->monitor);

  // register created event
  sc_event_registration_manager * manager = sc_storage_get_event_registration_manager();
  _sc_event_registration_manager_add(manager, event);

  return event;
}

sc_result sc_event_destroy(sc_event * event)
{
  if (event == null_ptr)
    return SC_RESULT_NO;

  sc_event_registration_manager * registration_manager = sc_storage_get_event_registration_manager();
  sc_event_emission_manager * emission_manager = sc_storage_get_event_emission_manager();

  sc_monitor_acquire_write(&event->monitor);
  if (_sc_event_registration_manager_remove(registration_manager, event) != SC_RESULT_OK)
  {
    sc_monitor_release_write(&event->monitor);
    return SC_RESULT_ERROR;
  }

  if (event->delete_callback != null_ptr)
    event->delete_callback(event);

  event->ref_count = SC_EVENT_REQUEST_DESTROY;
  event->subscription_addr = SC_ADDR_EMPTY;
  event->type = SC_ADDR_EMPTY;
  event->event_element_type = 0;
  event->callback = null_ptr;
  event->callback_ext = null_ptr;
  event->callback_with_user = null_ptr;
  event->delete_callback = null_ptr;
  event->data = null_ptr;

  sc_storage * storage = sc_storage_get();
  if (storage != null_ptr)
  {
    sc_monitor_acquire_write(&emission_manager->pool_monitor);
    sc_queue_push(&emission_manager->deletable_events, event);
    sc_monitor_release_write(&emission_manager->pool_monitor);
  }
  sc_monitor_release_write(&event->monitor);

  return SC_RESULT_OK;
}

sc_result sc_event_notify_element_deleted(sc_addr element)
{
  sc_hash_table_list * element_events_list = null_ptr;
  sc_event * event = null_ptr;

  sc_event_registration_manager * registration_manager = sc_storage_get_event_registration_manager();
  sc_event_emission_manager * emission_manager = sc_storage_get_event_emission_manager();

  // do nothing, if there are no registered events
  if (registration_manager == null_ptr || registration_manager->events_table == null_ptr)
    goto result;

  // sc_set_lookup for all registered to specified sc-element events
  if (registration_manager != null_ptr)
  {
    sc_monitor_acquire_read(&registration_manager->events_table_monitor);
    element_events_list =
        (sc_hash_table_list *)sc_hash_table_get(registration_manager->events_table, TABLE_KEY(element));
    sc_monitor_release_read(&registration_manager->events_table_monitor);
  }

  if (element_events_list != null_ptr)
  {
    sc_monitor_acquire_write(&registration_manager->events_table_monitor);
    sc_hash_table_remove(registration_manager->events_table, TABLE_KEY(element));
    sc_monitor_release_write(&registration_manager->events_table_monitor);

    while (element_events_list != null_ptr)
    {
      event = (sc_event *)element_events_list->data;

      // mark event for deletion
      sc_monitor_acquire_write(&event->monitor);

      sc_monitor_acquire_write(&emission_manager->pool_monitor);
      sc_queue_push(&emission_manager->deletable_events, event);
      sc_monitor_release_write(&emission_manager->pool_monitor);

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
    sc_addr subscription_addr,
    sc_event_type type,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr)
{
  if (ctx == null_ptr)
    return SC_RESULT_NO;

  if (_sc_memory_context_are_events_blocking(ctx))
    return SC_RESULT_OK;

  if (_sc_memory_context_are_events_pending(ctx))
  {
    _sc_memory_context_pend_event(ctx, type, subscription_addr, connector_addr, connector_type, other_addr);
    return SC_RESULT_OK;
  }

  return sc_event_emit_impl(ctx, subscription_addr, type, connector_addr, connector_type, other_addr);
}

sc_result sc_event_emit_impl(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr)
{
  sc_hash_table_list * element_events_list = null_ptr;
  sc_event * event = null_ptr;

  if (SC_ADDR_IS_EMPTY(subscription_addr))
    return SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  sc_event_registration_manager * registration_manager = sc_storage_get_event_registration_manager();
  sc_event_emission_manager * emission_manager = sc_storage_get_event_emission_manager();

  // if table is empty, then do nothing
  if (registration_manager == null_ptr || registration_manager->events_table == null_ptr)
    goto result;

  // sc_set_lookup for all registered to specified sc-element events
  sc_monitor_acquire_read(&registration_manager->events_table_monitor);
  if (registration_manager != null_ptr)
    element_events_list =
        (sc_hash_table_list *)sc_hash_table_get(registration_manager->events_table, TABLE_KEY(subscription_addr));
  sc_monitor_release_read(&registration_manager->events_table_monitor);

  while (element_events_list != null_ptr)
  {
    event = (sc_event *)element_events_list->data;

    if (SC_ADDR_IS_EQUAL(event->type, type)
        && ((event->event_element_type & connector_type) == event->event_element_type))
      _sc_event_emission_manager_add(
          emission_manager, event, ctx->user_addr, connector_addr, connector_type, other_addr);

    element_events_list = element_events_list->next;
  }

result:
  return SC_RESULT_OK;
}

sc_bool sc_event_is_deletable(sc_event const * event)
{
  return event->ref_count == SC_EVENT_REQUEST_DESTROY;
}

sc_pointer sc_event_get_data(sc_event const * event)
{
  return event->data;
}

sc_addr sc_event_get_element(sc_event const * event)
{
  return event->subscription_addr;
}
