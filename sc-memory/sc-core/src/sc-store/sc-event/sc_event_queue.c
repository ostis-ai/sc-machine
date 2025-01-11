/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_queue.h"

#include <unistd.h>

#include "sc-core/sc_event_subscription.h"
#include "sc_event_private.h"

#include "sc-store/sc_storage.h"
#include "sc-store/sc_storage_private.h"
#include "sc_memory_private.h"
#include "sc-core/sc_memory.h"
#include "sc-core/sc_keynodes.h"

#include "sc-core/sc-base/sc_allocator.h"

/*! Structure representing elementary sc-event.
 * @note This structure holds information required for processing events in a worker thread.
 */
typedef struct
{
  sc_event_subscription * event_subscription;  ///< A pointer to the sc-event subscription associated with the event.
  sc_addr user_addr;                           ///< A sc-address representing user that initiated this sc-event
  sc_addr connector_addr;               ///< A sc-address representing the sc-connector associated with the event.
  sc_type connector_type;               ///< A sc-type of the sc-connector associated with the event.
  sc_addr other_addr;                   ///< A sc-address representing the other element associated with the event.
  sc_event_do_after_callback callback;  ///< A pointer to function that is executed after the execution of a function
                                        ///< that was called on the initiated event.
  sc_addr event_addr;                   ///< An argument of callback.
} sc_event;

sc_event * _sc_event_new(
    sc_event_subscription * event_subscription,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr)
{
  sc_event * event = sc_mem_new(sc_event, 1);
  event->event_subscription = event_subscription;
  event->user_addr = user_addr;
  event->connector_addr = connector_addr;
  event->connector_type = connector_type;
  event->other_addr = other_addr;
  event->callback = callback;
  event->event_addr = event_addr;

  return event;
}

void _sc_event_emission_pool_worker_data_destroy(sc_event * data)
{
  sc_mem_free(data);
}

/*! Function that represents the work performed by a worker in the sc-event emission pool.
 * @param data Pointer to the sc_event containing information about the work.
 * @param user_data Pointer to the sc_event_emission_manager managing the sc-event emission.
 */
void _sc_event_emission_pool_worker(sc_pointer data, sc_pointer user_data)
{
  sc_event * event = (sc_event *)data;
  sc_event_emission_manager * manager = user_data;

  sc_event_subscription * event_subscription = event->event_subscription;
  if (event_subscription == null_ptr)
    goto destroy;

  sc_monitor_acquire_read(&manager->destroy_monitor);

  if (manager->running == SC_FALSE)
    goto end;

  sc_monitor_acquire_read(&event_subscription->monitor);
  if (sc_event_subscription_is_deletable(event_subscription))
  {
    sc_monitor_release_read(&event_subscription->monitor);
    goto destroy;
  }

  sc_event_callback callback = event_subscription->callback;
  sc_event_callback_with_user callback_ext2 = event_subscription->callback_with_user;

  sc_storage_start_new_process();

  if (callback != null_ptr)
    callback(event_subscription, event->connector_addr);
  else if (callback_ext2 != null_ptr)
    callback_ext2(
        event_subscription, event->user_addr, event->connector_addr, event->connector_type, event->other_addr);

  sc_storage_end_new_process();

  if (SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_connector_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_incoming_arc_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_outgoing_arc_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_edge_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_element_addr))
  {
    sc_monitor_acquire_write(&manager->emitted_erase_events_monitor);
    sc_addr key = SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_element_addr)
                      ? event->event_addr
                      : event->connector_addr;
    sc_uint32 count = (sc_uint32)(sc_uint64)sc_hash_table_get(
        manager->emitted_erase_events, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(key)));
    if (count != 0)
    {
      --count;
      if (count == 0)
        sc_hash_table_remove(manager->emitted_erase_events, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(key)));
      else
        sc_hash_table_insert(
            manager->emitted_erase_events, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(key)), GUINT_TO_POINTER(count));
    }
    sc_monitor_release_write(&manager->emitted_erase_events_monitor);
  }

  sc_monitor_release_read(&event_subscription->monitor);

end:
  sc_monitor_release_read(&manager->destroy_monitor);
destroy:
{
  sc_monitor_acquire_write(&manager->emitted_erase_events_monitor);
  --manager->current_emitted_events_count;
  sc_monitor_release_write(&manager->emitted_erase_events_monitor);

  if (event->callback != null_ptr)
  {
    sc_memory_context * ctx = sc_memory_context_new_ext(event->user_addr);
    event->callback(ctx, event->event_addr);
    sc_memory_context_free(ctx);
  }

  _sc_event_emission_pool_worker_data_destroy(event);
}
}

guint emitted_events_hash_func(gconstpointer pointer)
{
  return GPOINTER_TO_UINT(pointer);
}

gboolean emitted_events_equal_func(gconstpointer a, gconstpointer b)
{
  return (a == b);
}

void sc_event_emission_manager_initialize(sc_event_emission_manager ** manager, sc_memory_params const * params)
{
  *manager = sc_mem_new(sc_event_emission_manager, 1);
  sc_queue_init(&(*manager)->deletable_events_subscriptions);

  (*manager)->limit_max_threads_by_max_physical_cores = params->limit_max_threads_by_max_physical_cores;
  (*manager)->max_events_and_agents_threads =
      (*manager)->limit_max_threads_by_max_physical_cores
          ? sc_boundary(params->max_events_and_agents_threads, 1, g_get_num_processors())
          : sc_max(1, params->max_events_and_agents_threads);
  {
    sc_memory_info("Sc-event managers configuration:");
    sc_message(
        "\tLimit max threads by max physical cores: %s",
        (*manager)->limit_max_threads_by_max_physical_cores ? "On" : "Off");
    sc_message("\tMax events and agents threads: %d", (*manager)->max_events_and_agents_threads);
  }

  (*manager)->running = SC_TRUE;

  sc_monitor_init(&(*manager)->pool_monitor);
  (*manager)->current_emitted_events_count = 0;
  (*manager)->emitted_erase_events =
      sc_hash_table_init(emitted_events_hash_func, emitted_events_equal_func, null_ptr, null_ptr);
  sc_monitor_init(&(*manager)->emitted_erase_events_monitor);
  (*manager)->thread_pool = g_thread_pool_new(
      _sc_event_emission_pool_worker,
      *manager,
      (sc_int32)(*manager)->max_events_and_agents_threads,
      SC_FALSE,
      null_ptr);
}

void sc_event_emission_manager_stop(sc_event_emission_manager * manager)
{
  if (manager == null_ptr)
    return;

  if (manager->running)
    manager->running = SC_FALSE;
}

void sc_event_emission_manager_shutdown(sc_event_emission_manager * manager)
{
  if (manager == null_ptr)
    return;

  // Acquire write lock once for all operations
  sc_monitor_acquire_write(&manager->destroy_monitor);
  sc_monitor_acquire_write(&manager->pool_monitor);
  if (manager->thread_pool)
  {
    g_thread_pool_free(manager->thread_pool, SC_FALSE, SC_TRUE);
    manager->thread_pool = null_ptr;
  }

  // Wait for current events to finish with a timeout
  sc_int32 const MAX_WAIT_ITERATIONS = 1000;
  sc_int32 wait_iterations = 0;
  while (manager->current_emitted_events_count > 0 && wait_iterations < MAX_WAIT_ITERATIONS)
  {
    sc_monitor_release_write(&manager->destroy_monitor);
    usleep(10);
    sc_monitor_acquire_write(&manager->destroy_monitor);
    wait_iterations++;
  }

  while (!sc_queue_empty(&manager->deletable_events_subscriptions))
  {
    sc_event_subscription * event_subscription = sc_queue_pop(&manager->deletable_events_subscriptions);
    sc_monitor_destroy(&event_subscription->monitor);
    sc_mem_free(event_subscription);
  }
  sc_queue_destroy(&manager->deletable_events_subscriptions);

  sc_monitor_release_write(&manager->pool_monitor);

  sc_monitor_destroy(&manager->pool_monitor);
  sc_monitor_destroy(&manager->destroy_monitor);
  sc_hash_table_destroy(manager->emitted_erase_events);
  sc_mem_free(manager);
}

void _sc_event_emission_manager_add(
    sc_event_emission_manager * manager,
    sc_event_subscription * event_subscription,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr)
{
  sc_event * event =
      _sc_event_new(event_subscription, user_addr, connector_addr, connector_type, other_addr, callback, event_addr);

  sc_monitor_acquire_write(&manager->pool_monitor);
  if (manager->thread_pool == null_ptr)
  {
    sc_monitor_release_write(&manager->pool_monitor);
    return;
  }

  if (SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_connector_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_incoming_arc_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_outgoing_arc_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_edge_addr)
      || SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_element_addr))
  {
    sc_monitor_acquire_write(&manager->emitted_erase_events_monitor);
    sc_addr key_addr = SC_ADDR_IS_EQUAL(event_subscription->event_type_addr, sc_event_before_erase_element_addr)
                           ? event_subscription->subscription_addr
                           : connector_addr;
    sc_uint32 count = (sc_uint32)(sc_uint64)sc_hash_table_get(
        manager->emitted_erase_events, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(key_addr)));
    ++count;
    sc_hash_table_insert(
        manager->emitted_erase_events, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(key_addr)), GUINT_TO_POINTER(count));
    sc_monitor_release_write(&manager->emitted_erase_events_monitor);
  }

  sc_monitor_acquire_write(&manager->emitted_erase_events_monitor);
  ++manager->current_emitted_events_count;
  sc_monitor_release_write(&manager->emitted_erase_events_monitor);

  g_thread_pool_push(manager->thread_pool, event, null_ptr);
  sc_monitor_release_write(&manager->pool_monitor);
}
