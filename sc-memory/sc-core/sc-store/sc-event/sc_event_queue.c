/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_queue.h"

#include "../sc_event.h"
#include "sc_event_private.h"

#include "../sc_storage.h"
#include "../sc_storage_private.h"
#include "../../sc_memory_private.h"

#include "../sc-base/sc_allocator.h"

/*! Structure representing data for a worker in the event emission pool.
 * @note This structure holds information required for processing events in a worker thread.
 */
typedef struct
{
  sc_event * event;        ///< Pointer to the sc-event associated with the worker.
  sc_addr user_addr;       ///< sc-address representing user that initiated this sc-event
  sc_addr connector_addr;  ///< sc-address representing the sc-connector associated with the event.
  sc_type connector_type;  ///< sc-type of the sc-connector associated with the event.
  sc_addr other_addr;      ///< sc-address representing the other element associated with the event.
} sc_event_emission_pool_worker_data;

/*! Function that creates a new instance of sc_event_emission_pool_worker_data.
 * @param event Pointer to the sc-event associated with the worker.
 * @param connector_addr sc-address representing the sc-connector associated with the event.
 * @param other_addr sc-address representing the other element associated with the event.
 * @returns Returns a pointer to the newly created sc_event_emission_pool_worker_data.
 */
sc_event_emission_pool_worker_data * _sc_event_emission_pool_worker_data_new(
    sc_event * event,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr)
{
  sc_event_emission_pool_worker_data * data = sc_mem_new(sc_event_emission_pool_worker_data, 1);
  data->event = event;
  data->user_addr = user_addr;
  data->connector_addr = connector_addr;
  data->connector_type = connector_type;
  data->other_addr = other_addr;

  return data;
}

/*! Function that destroys an instance of sc_event_emission_pool_worker_data.
 * @param data Pointer to the sc_event_emission_pool_worker_data to be destroyed.
 */
void _sc_event_emission_pool_worker_data_destroy(sc_event_emission_pool_worker_data * data)
{
  sc_mem_free(data);
}

/*! Function that represents the work performed by a worker in the event emission pool.
 * @param data Pointer to the sc_event_emission_pool_worker_data containing information about the work.
 * @param user_data Pointer to the sc_event_emission_manager managing the event emission.
 */
void _sc_event_emission_pool_worker(sc_pointer data, sc_pointer user_data)
{
  sc_event_emission_pool_worker_data * work_data = (sc_event_emission_pool_worker_data *)data;
  sc_event_emission_manager * queue = user_data;

  sc_event * event = work_data->event;
  if (event == null_ptr)
    goto destroy;

  sc_monitor_acquire_read(&queue->destroy_monitor);

  if (queue->running == SC_FALSE)
    goto end;

  sc_monitor_acquire_read(&event->monitor);
  if (sc_event_is_deletable(event))
  {
    sc_monitor_release_read(&event->monitor);
    goto end;
  }

  sc_event_callback callback = event->callback;
  sc_event_callback_ext callback_ext = event->callback_ext;
  sc_event_callback_with_user callback_ext2 = event->callback_with_user;

  sc_storage_start_new_process();

  if (callback != null_ptr)
    callback(event, work_data->connector_addr);
  else if (callback_ext != null_ptr)
    callback_ext(event, work_data->connector_addr, work_data->other_addr);
  else if (callback_ext2 != null_ptr)
    callback_ext2(
        event, work_data->user_addr, work_data->connector_addr, work_data->connector_type, work_data->other_addr);

  sc_storage_end_new_process();

  sc_monitor_release_read(&event->monitor);

end:
  sc_monitor_release_read(&queue->destroy_monitor);
destroy:
  _sc_event_emission_pool_worker_data_destroy(work_data);
}

void sc_event_emission_manager_initialize(sc_event_emission_manager ** manager, sc_memory_params const * params)
{
  *manager = sc_mem_new(sc_event_emission_manager, 1);
  sc_queue_init(&(*manager)->deletable_events);

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
  sc_monitor_init(&(*manager)->destroy_monitor);

  sc_monitor_init(&(*manager)->pool_monitor);
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

  sc_bool is_running = SC_FALSE;

  sc_monitor_acquire_read(&manager->destroy_monitor);
  is_running = manager->running;
  sc_monitor_release_read(&manager->destroy_monitor);

  if (is_running)
  {
    sc_monitor_acquire_write(&manager->destroy_monitor);
    manager->running = SC_FALSE;
    sc_monitor_release_write(&manager->destroy_monitor);
  }
}

void sc_event_emission_manager_shutdown(sc_event_emission_manager * manager)
{
  if (manager == null_ptr)
    return;

  sc_monitor_acquire_write(&manager->pool_monitor);
  if (manager->thread_pool)
  {
    g_thread_pool_free(manager->thread_pool, SC_FALSE, SC_TRUE);
    manager->thread_pool = null_ptr;
  }

  while (!sc_queue_empty(manager->deletable_events))
  {
    sc_event * event = sc_queue_pop(manager->deletable_events);
    sc_monitor_destroy(&event->monitor);
    sc_mem_free(event);
  }
  sc_queue_destroy(manager->deletable_events);

  sc_monitor_release_write(&manager->pool_monitor);

  sc_monitor_destroy(&manager->pool_monitor);
  sc_monitor_destroy(&manager->destroy_monitor);
  sc_mem_free(manager);
}

void _sc_event_emission_manager_add(
    sc_event_emission_manager * manager,
    sc_event * event,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr)
{
  if (manager == null_ptr)
    return;

  sc_event_emission_pool_worker_data * data =
      _sc_event_emission_pool_worker_data_new(event, user_addr, connector_addr, connector_type, other_addr);

  sc_monitor_acquire_write(&manager->pool_monitor);
  g_thread_pool_push(manager->thread_pool, data, null_ptr);
  sc_monitor_release_write(&manager->pool_monitor);
}
