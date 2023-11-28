/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_queue.h"
#include "../sc_event.h"
#include "sc_event_private.h"

#include "../sc-base/sc_allocator.h"
#include "../sc-base/sc_message.h"
#include "../sc_storage.h"

typedef struct
{
  sc_event * evt;
  sc_addr edge_addr;
  sc_addr other_addr;
} sc_event_emission_pool_worker_data;

sc_event_emission_pool_worker_data * _sc_event_emission_pool_worker_data_new(
    sc_event * evt,
    sc_addr edge_addr,
    sc_addr other_addr)
{
  sc_event_emission_pool_worker_data * data = sc_mem_new(sc_event_emission_pool_worker_data, 1);
  data->evt = evt;
  data->edge_addr = edge_addr;
  data->other_addr = other_addr;

  return data;
}

void _sc_event_emission_pool_worker_data_destroy(sc_event_emission_pool_worker_data * data)
{
  sc_mem_free(data);
}

void _sc_event_emission_pool_worker(sc_pointer data, sc_pointer user_data)
{
  sc_event_emission_pool_worker_data * work_data = (sc_event_emission_pool_worker_data *)data;
  sc_event_emission_manager * queue = user_data;

  sc_event * event = work_data->evt;
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

  fEventCallback callback = event->callback;
  fEventCallbackEx callbackEx = event->callback_ex;

  sc_storage_start_new_process();

  if (callback != null_ptr)
    callback(event, work_data->edge_addr);
  else if (callbackEx != null_ptr)
    callbackEx(event, work_data->edge_addr, work_data->other_addr);

  sc_storage_end_new_process();

  sc_monitor_release_read(&event->monitor);

end:
  sc_monitor_release_read(&queue->destroy_monitor);
destroy:
  _sc_event_emission_pool_worker_data_destroy(work_data);
}

void sc_event_emission_manager_initialize(sc_event_emission_manager ** manager, sc_uint32 max_events_and_agents_threads)
{
  *manager = sc_mem_new(sc_event_emission_manager, 1);
  sc_queue_init(&(*manager)->deletable_events);

  max_events_and_agents_threads = sc_boundary(max_events_and_agents_threads, 1, g_get_num_processors());
  {
    sc_message("[sc-events] Configuration:");
    sc_message("\tMax events and agents threads: %d", max_events_and_agents_threads);
  }

  (*manager)->running = SC_TRUE;
  sc_monitor_init(&(*manager)->destroy_monitor);

  sc_monitor_init(&(*manager)->pool_monitor);
  (*manager)->thread_pool = g_thread_pool_new(
      _sc_event_emission_pool_worker, *manager, (sc_int32)max_events_and_agents_threads, SC_FALSE, null_ptr);
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

void _sc_event_emission_manager_add(sc_event_emission_manager * manager, sc_event * evt, sc_addr edge, sc_addr other_el)
{
  if (manager == null_ptr)
    return;

  sc_event_emission_pool_worker_data * data = _sc_event_emission_pool_worker_data_new(evt, edge, other_el);

  sc_monitor_acquire_write(&manager->pool_monitor);
  g_thread_pool_push(manager->thread_pool, data, null_ptr);
  sc_monitor_release_write(&manager->pool_monitor);
}
