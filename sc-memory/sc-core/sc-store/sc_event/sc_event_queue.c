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
} sc_event_pool_worker_data;

sc_event_pool_worker_data * sc_event_pool_worker_data_new(sc_event * evt, sc_addr edge_addr, sc_addr other_addr)
{
  sc_event_pool_worker_data * data = sc_mem_new(sc_event_pool_worker_data, 1);
  data->evt = evt;
  data->edge_addr = edge_addr;
  data->other_addr = other_addr;

  return data;
}

void sc_event_pool_worker_data_destroy(sc_event_pool_worker_data * data)
{
  sc_mem_free(data);
}

void sc_event_pool_worker(sc_pointer data, sc_pointer user_data)
{
  sc_event_pool_worker_data * work_data = (sc_event_pool_worker_data *)data;

  sc_event * event = work_data->evt;
  if (event == null_ptr)
    goto destroy;

  sc_monitor_acquire_read(&event->monitor);
  if (event->ref_count == SC_EVENT_REQUEST_DESTROY)
  {
    sc_monitor_release_read(&event->monitor);
    goto destroy;
  }

  fEventCallback callback = event->callback;
  fEventCallbackEx callbackEx = event->callback_ex;
  sc_monitor_release_read(&event->monitor);

  sc_storage_start_new_process();

  if (callback != null_ptr)
    callback(event, work_data->edge_addr);
  else if (callbackEx != null_ptr)
    callbackEx(event, work_data->edge_addr, work_data->other_addr);

  sc_storage_end_new_process();

destroy:
  sc_event_pool_worker_data_destroy(work_data);
}

sc_event_queue * sc_event_queue_new_ext(sc_uint32 max_events_and_agents_threads)
{
  sc_event_queue * queue = sc_mem_new(sc_event_queue, 1);
  sc_queue_init(&queue->deletable_events);
  queue->processes_segments_table = sc_hash_table_init(g_direct_hash, g_direct_equal, null_ptr, null_ptr);
  sc_monitor_init(&queue->monitor);
  sc_monitor_init(&queue->processes_monitor);
  queue->running = SC_TRUE;

  max_events_and_agents_threads = sc_boundary(max_events_and_agents_threads, 1, g_get_num_processors());
  {
    sc_message("[sc-events] Configuration:");
    sc_message("\tMax events and agents threads: %d", max_events_and_agents_threads);
  }

  queue->thread_pool =
      g_thread_pool_new(sc_event_pool_worker, queue, (sc_int32)max_events_and_agents_threads, SC_FALSE, null_ptr);

  return queue;
}

void sc_event_queue_stop_processing(sc_event_queue * queue)
{
  if (queue == null_ptr)
    return;

  sc_bool is_running = SC_FALSE;

  sc_monitor_acquire_read(&queue->monitor);
  is_running = queue->running;
  sc_monitor_release_read(&queue->monitor);

  if (is_running)
  {
    sc_monitor_acquire_write(&queue->monitor);
    queue->running = SC_FALSE;
    sc_monitor_release_write(&queue->monitor);
  }
}

void sc_event_queue_destroy_wait(sc_event_queue * queue)
{
  if (queue == null_ptr)
    return;

  sc_monitor_acquire_write(&queue->monitor);
  sc_monitor_acquire_write(&queue->processes_monitor);

  if (queue->processes_segments_table)
  {
    sc_hash_table_destroy(queue->processes_segments_table);
    queue->processes_segments_table = null_ptr;
  }

  sc_monitor_release_write(&queue->processes_monitor);

  if (queue->thread_pool)
  {
    g_thread_pool_free(queue->thread_pool, SC_FALSE, SC_TRUE);
    queue->thread_pool = null_ptr;
  }

  while (!sc_queue_empty(queue->deletable_events))
  {
    sc_event * event = sc_queue_pop(queue->deletable_events);
    sc_monitor_destroy(&event->monitor);
    sc_mem_free(event);
  }
  sc_queue_destroy(queue->deletable_events);

  sc_monitor_release_write(&queue->monitor);

  sc_monitor_destroy(&queue->processes_monitor);
  sc_monitor_destroy(&queue->monitor);
  sc_mem_free(queue);
}

void sc_event_queue_append(sc_event_queue * queue, sc_event * evt, sc_addr edge, sc_addr other_el)
{
  if (queue == null_ptr || queue->running == SC_FALSE)
    return;

  sc_monitor_acquire_write(&queue->monitor);

  sc_bool is_running = queue->running;
  if (is_running == SC_FALSE)
  {
    sc_monitor_release_write(&queue->monitor);
    return;
  }

  sc_event_pool_worker_data * data = sc_event_pool_worker_data_new(evt, edge, other_el);
  g_thread_pool_push(queue->thread_pool, data, null_ptr);

  sc_monitor_release_write(&queue->monitor);
}
