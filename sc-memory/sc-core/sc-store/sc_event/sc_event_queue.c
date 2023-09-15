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
#include "../sc-base/sc_mutex.h"

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

void sc_event_pool_worker(gpointer data, gpointer user_data)
{
  sc_event_pool_worker_data * work_data = (sc_event_pool_worker_data *)data;

  if (work_data->evt->callback != null_ptr)
  {
    work_data->evt->callback(work_data->evt, work_data->edge_addr);
  }
  else if (work_data->evt->callback_ex != null_ptr)
  {
    work_data->evt->callback_ex(work_data->evt, work_data->edge_addr, work_data->other_addr);
  }

  sc_event_unref(work_data->evt);
  sc_event_pool_worker_data_destroy(work_data);
}

sc_event_queue * sc_event_queue_new_ext(sc_uint32 max_events_and_agents_threads)
{
  sc_event_queue * queue = sc_mem_new(sc_event_queue, 1);
  queue->running = SC_TRUE;
  g_mutex_init(&queue->mutex);

  max_events_and_agents_threads = sc_boundary(max_events_and_agents_threads, 1, g_get_num_processors());
  {
    sc_message("[sc-events] Configuration:");
    sc_message("\tMax events and agents threads: %d", max_events_and_agents_threads);
  }

  queue->thread_pool =
      g_thread_pool_new(sc_event_pool_worker, null_ptr, (sc_int32)max_events_and_agents_threads, SC_FALSE, null_ptr);

  return queue;
}

sc_event_queue * sc_event_queue_new()
{
  return sc_event_queue_new_ext(g_get_num_processors());
}

void sc_event_queue_stop_processing(sc_event_queue * queue)
{
  if (queue == null_ptr)
    return;

  sc_bool is_running = SC_FALSE;

  g_mutex_lock(&queue->mutex);
  is_running = queue->running;
  g_mutex_unlock(&queue->mutex);

  if (is_running)
  {
    g_mutex_lock(&queue->mutex);
    queue->running = SC_FALSE;
    g_mutex_unlock(&queue->mutex);
  }
}

void sc_event_queue_destroy_wait(sc_event_queue * queue)
{
  if (queue == null_ptr)
    return;

  if (queue->thread_pool)
  {
    g_mutex_lock(&queue->mutex);

    g_thread_pool_free(queue->thread_pool, SC_FALSE, SC_TRUE);
    queue->thread_pool = null_ptr;
    g_mutex_unlock(&queue->mutex);
  }

  sc_mem_free(queue);
}

void sc_event_queue_append(sc_event_queue * queue, sc_event * evt, sc_addr edge, sc_addr other_el)
{
  if (queue->running == SC_TRUE)
  {
    sc_event_pool_worker_data * data = sc_event_pool_worker_data_new(evt, edge, other_el);
    g_thread_pool_push(queue->thread_pool, data, null_ptr);
  }
}
