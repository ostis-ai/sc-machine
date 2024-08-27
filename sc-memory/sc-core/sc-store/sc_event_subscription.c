/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.h"

#include "sc_storage.h"
#include "sc_storage_private.h"
#include "sc-event/sc_event_private.h"
#include "sc-event/sc_event_queue.h"

#include "../sc_memory_context_manager.h"
#include "../sc_memory_context_private.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_mutex.h"

/*! Structure representing an sc-event_subscription registration manager.
 * @note This structure manages the registration and removal of sc-events associated with sc-elements.
 */
struct _sc_event_subscription_manager
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

/*! Adds the specified sc-event_subscription to the registration manager's events table.
 * @param manager Pointer to the sc-event_subscription registration manager.
 * @param event_subscription Pointer to the sc-event_subscription to be added.
 * @return Returns SC_RESULT_OK if the operation is successful, SC_RESULT_NO otherwise.
 */
sc_result _sc_event_subscription_manager_add(
    sc_event_subscription_manager * manager,
    sc_event_subscription * event_subscription)
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
      (sc_hash_table_list *)sc_hash_table_get(manager->events_table, TABLE_KEY(event_subscription->subscription_addr));
  element_events_list = sc_hash_table_list_append(element_events_list, (sc_pointer)event_subscription);
  sc_hash_table_insert(
      manager->events_table, TABLE_KEY(event_subscription->subscription_addr), (sc_pointer)element_events_list);

  sc_monitor_release_write(&manager->events_table_monitor);

  return SC_RESULT_OK;
}

/*! Removes the specified sc-event_subscription from the registration manager's events table.
 * @param manager Pointer to the sc-event_subscription registration manager.
 * @param event_subscription Pointer to the sc-event_subscription to be removed.
 * @return Returns SC_RESULT_OK if the operation is successful, SC_RESULT_ERROR_INVALID_PARAMS otherwise.
 */
sc_result _sc_event_subscription_manager_remove(
    sc_event_subscription_manager * manager,
    sc_event_subscription * event_subscription)
{
  sc_hash_table_list * element_events_list = null_ptr;

  // the first, if table doesn't exist, then return error
  if (manager == null_ptr)
    return SC_RESULT_NO;

  sc_monitor_acquire_write(&manager->events_table_monitor);
  element_events_list =
      (sc_hash_table_list *)sc_hash_table_get(manager->events_table, TABLE_KEY(event_subscription->subscription_addr));
  if (element_events_list == null_ptr)
    goto error;

  if (manager->events_table == null_ptr)
    goto error;

  // remove event_subscription from list of events for specified sc-element
  element_events_list = sc_hash_table_list_remove(element_events_list, (sc_const_pointer)event_subscription);
  if (element_events_list == null_ptr)
    sc_hash_table_remove(manager->events_table, TABLE_KEY(event_subscription->subscription_addr));
  else
    sc_hash_table_insert(
        manager->events_table, TABLE_KEY(event_subscription->subscription_addr), (sc_pointer)element_events_list);

  sc_monitor_release_write(&manager->events_table_monitor);
  return SC_RESULT_OK;
error:
  sc_monitor_release_write(&manager->events_table_monitor);
  return SC_RESULT_ERROR_INVALID_PARAMS;
}

void sc_event_subscription_manager_initialize(sc_event_subscription_manager ** manager)
{
  (*manager) = sc_mem_new(sc_event_subscription_manager, 1);
  (*manager)->events_table = sc_hash_table_init(events_table_hash_func, events_table_equal_func, null_ptr, null_ptr);
  sc_monitor_init(&(*manager)->events_table_monitor);
}

void sc_event_subscription_manager_shutdown(sc_event_subscription_manager * manager)
{
  sc_monitor_destroy(&manager->events_table_monitor);
  sc_hash_table_destroy(manager->events_table);
  sc_mem_free(manager);
}

sc_event_subscription * sc_event_subscription_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_pointer data,
    sc_event_callback callback,
    sc_event_subscription_delete_function delete_callback)
{
  sc_unused(ctx);

  if (!sc_storage_is_element(ctx, subscription_addr))
    return null_ptr;

  if (!sc_storage_is_element(ctx, event_type_addr))
    return null_ptr;

  sc_event_subscription * event_subscription = sc_mem_new(sc_event_subscription, 1);
  event_subscription->subscription_addr = subscription_addr;
  event_subscription->event_type_addr = event_type_addr;
  event_subscription->event_element_type = 0;
  event_subscription->callback = callback;
  event_subscription->callback_with_user = null_ptr;
  event_subscription->delete_callback = delete_callback;
  event_subscription->data = data;
  event_subscription->ref_count = 1;
  sc_monitor_init(&event_subscription->monitor);

  // register created event_subscription
  sc_event_subscription_manager * manager = sc_storage_get_event_subscription_manager();
  _sc_event_subscription_manager_add(manager, event_subscription);

  return event_subscription;
}

sc_event_subscription * sc_event_subscription_with_user_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_type event_element_type,
    sc_pointer data,
    sc_event_callback_with_user callback,
    sc_event_subscription_delete_function delete_callback)
{
  sc_unused(ctx);

  if (!sc_storage_is_element(ctx, subscription_addr))
    return null_ptr;

  if (!sc_storage_is_element(ctx, event_type_addr))
    return null_ptr;

  sc_event_subscription * event_subscription = sc_mem_new(sc_event_subscription, 1);
  event_subscription->subscription_addr = subscription_addr;
  event_subscription->event_type_addr = event_type_addr;
  event_subscription->event_element_type = event_element_type;
  event_subscription->callback = null_ptr;
  event_subscription->callback_with_user = callback;
  event_subscription->delete_callback = delete_callback;
  event_subscription->data = data;
  event_subscription->ref_count = 1;
  sc_monitor_init(&event_subscription->monitor);

  // register created event_subscription
  sc_event_subscription_manager * manager = sc_storage_get_event_subscription_manager();
  _sc_event_subscription_manager_add(manager, event_subscription);

  return event_subscription;
}

sc_result sc_event_subscription_destroy(sc_event_subscription * event_subscription)
{
  if (event_subscription == null_ptr)
    return SC_RESULT_NO;

  sc_event_subscription_manager * subscription_manager = sc_storage_get_event_subscription_manager();
  sc_event_emission_manager * emission_manager = sc_storage_get_event_emission_manager();

  sc_monitor_acquire_write(&event_subscription->monitor);
  if (_sc_event_subscription_manager_remove(subscription_manager, event_subscription) != SC_RESULT_OK)
  {
    sc_monitor_release_write(&event_subscription->monitor);
    return SC_RESULT_ERROR;
  }

  if (event_subscription->delete_callback != null_ptr)
    event_subscription->delete_callback(event_subscription);

  event_subscription->ref_count = SC_EVENT_REQUEST_DESTROY;
  event_subscription->subscription_addr = SC_ADDR_EMPTY;
  event_subscription->event_type_addr = SC_ADDR_EMPTY;
  event_subscription->event_element_type = 0;
  event_subscription->callback = null_ptr;
  event_subscription->callback_with_user = null_ptr;
  event_subscription->delete_callback = null_ptr;
  event_subscription->data = null_ptr;

  sc_storage * storage = sc_storage_get();
  if (storage != null_ptr)
  {
    sc_monitor_acquire_write(&emission_manager->pool_monitor);
    sc_queue_push(&emission_manager->deletable_events_subscriptions, event_subscription);
    sc_monitor_release_write(&emission_manager->pool_monitor);
  }
  sc_monitor_release_write(&event_subscription->monitor);

  return SC_RESULT_OK;
}

sc_result sc_event_notify_element_deleted(sc_addr element)
{
  sc_hash_table_list * element_events_list = null_ptr;
  sc_event_subscription * event_subscription = null_ptr;

  sc_event_subscription_manager * subscription_manager = sc_storage_get_event_subscription_manager();
  sc_event_emission_manager * emission_manager = sc_storage_get_event_emission_manager();

  // do nothing, if there are no registered events
  if (subscription_manager == null_ptr || subscription_manager->events_table == null_ptr)
    goto result;

  // lookup for all registered to specified sc-element events
  if (subscription_manager != null_ptr)
  {
    sc_monitor_acquire_write(&subscription_manager->events_table_monitor);
    element_events_list =
        (sc_hash_table_list *)sc_hash_table_get(subscription_manager->events_table, TABLE_KEY(element));
    if (element_events_list != null_ptr)
      sc_hash_table_remove(subscription_manager->events_table, TABLE_KEY(element));
    sc_monitor_release_write(&subscription_manager->events_table_monitor);
  }

  if (element_events_list != null_ptr)
  {
    while (element_events_list != null_ptr)
    {
      event_subscription = (sc_event_subscription *)element_events_list->data;

      // mark event_subscription for deletion
      sc_monitor_acquire_write(&event_subscription->monitor);

      sc_monitor_acquire_write(&emission_manager->pool_monitor);
      sc_queue_push(&emission_manager->deletable_events_subscriptions, event_subscription);
      sc_monitor_release_write(&emission_manager->pool_monitor);

      sc_monitor_release_write(&event_subscription->monitor);

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
    sc_event_type event_type_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr)
{
  if (ctx == null_ptr)
    return SC_RESULT_NO;

  if (_sc_memory_context_are_events_blocking(ctx))
    return SC_RESULT_NO;

  if (_sc_memory_context_are_events_pending(ctx))
  {
    _sc_memory_context_pend_event(ctx, event_type_addr, subscription_addr, connector_addr, connector_type, other_addr);
    return SC_RESULT_OK;
  }

  return sc_event_emit_impl(
      ctx, subscription_addr, event_type_addr, connector_addr, connector_type, other_addr, callback, event_addr);
}

sc_result sc_event_emit_impl(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr)
{
  sc_hash_table_list * element_events_list = null_ptr;
  sc_event_subscription * event_subscription = null_ptr;

  sc_event_subscription_manager * subscription_manager = sc_storage_get_event_subscription_manager();
  sc_event_emission_manager * emission_manager = sc_storage_get_event_emission_manager();

  // if table is empty, then do nothing
  sc_result result = SC_RESULT_NO;
  if (subscription_manager == null_ptr || subscription_manager->events_table == null_ptr)
    goto result;

  // lookup for all registered to specified sc-element events
  sc_monitor_acquire_read(&subscription_manager->events_table_monitor);
  if (subscription_manager != null_ptr)
    element_events_list =
        (sc_hash_table_list *)sc_hash_table_get(subscription_manager->events_table, TABLE_KEY(subscription_addr));

  while (element_events_list != null_ptr)
  {
    event_subscription = (sc_event_subscription *)element_events_list->data;

    if (SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, event_type_addr)
        && ((event_subscription->event_element_type & connector_type) == event_subscription->event_element_type))
    {
      _sc_event_emission_manager_add(
          emission_manager,
          event_subscription,
          ctx->user_addr,
          connector_addr,
          connector_type,
          other_addr,
          callback,
          event_addr);

      result = SC_RESULT_OK;
    }

    element_events_list = element_events_list->next;
  }
  sc_monitor_release_read(&subscription_manager->events_table_monitor);

result:
  return result;
}

sc_bool sc_event_subscription_is_deletable(sc_event_subscription const * event_subscription)
{
  return event_subscription->ref_count == SC_EVENT_REQUEST_DESTROY;
}

sc_pointer sc_event_subscription_get_data(sc_event_subscription const * event_subscription)
{
  return event_subscription->data;
}

sc_addr sc_event_subscription_get_event_type(sc_event_subscription const * event_subscription)
{
  return event_subscription->event_type_addr;
}

sc_addr sc_event_subscription_get_element(sc_event_subscription const * event_subscription)
{
  return event_subscription->subscription_addr;
}
