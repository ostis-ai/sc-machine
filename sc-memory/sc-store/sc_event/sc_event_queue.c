/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_queue.h"
#include "../sc_event.h"
#include "sc_event_private.h"
#include "../sc_storage.h"
#include "../../sc_memory_private.h"
#include "../../sc_memory.h"

#pragma pack(push, 1)
typedef struct
{
    sc_memory_context * ctx;
    sc_event * evt;
    sc_addr edge_addr;
    sc_addr other_addr;
} sc_event_pool_worker_data;
#pragma pack(pop)

sc_event_pool_worker_data * sc_event_pool_worker_data_new(sc_event * evt, sc_addr edge_addr, sc_addr other_addr)
{
    sc_event_pool_worker_data * data = g_new0(sc_event_pool_worker_data, 1);
    data->ctx = sc_memory_context_new(sc_access_lvl_make_max);
    data->evt = evt;
    data->edge_addr = edge_addr;
    data->other_addr = other_addr;

    return data;
}

void sc_event_pool_worker_data_destroy(sc_event_pool_worker_data * data)
{
    g_assert(data != null_ptr);
    sc_memory_context_free(data->ctx);
    g_free(data);
}


void sc_event_pool_worker(gpointer data, gpointer user_data)
{
    g_assert(data != null_ptr);
    sc_event_pool_worker_data * work_data = (sc_event_pool_worker_data*)data;
    
    g_assert(work_data->evt != null_ptr);
    if (work_data->evt->callback != null_ptr)    // TODO: cleanup in 0.4.0
    {
        work_data->evt->callback(work_data->evt, work_data->edge_addr);
    }
    else if (work_data->evt->callback_ex != null_ptr)
    {
        work_data->evt->callback_ex(work_data->evt, work_data->edge_addr, work_data->other_addr);
    }

    sc_event_try_free(work_data->ctx, work_data->evt);
    sc_event_pool_worker_data_destroy(work_data);
}

sc_event_queue * sc_event_queue_new()
{
    sc_event_queue *queue = g_new0(sc_event_queue, 1);
    queue->running = SC_TRUE;
    g_mutex_init(&queue->mutex);
    queue->thread_pool = g_thread_pool_new(sc_event_pool_worker, (gpointer)0, g_get_num_processors() * 2, FALSE, 0);

    return queue;
}

void sc_event_queue_destroy_wait(sc_event_queue *queue)
{
    g_assert(queue != 0);

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

    if (queue->thread_pool)
    {
        g_mutex_lock(&queue->mutex);
        g_thread_pool_free(queue->thread_pool, FALSE, TRUE);
        queue->thread_pool = 0;
        g_mutex_unlock(&queue->mutex);
    }
}

void sc_event_queue_append(sc_event_queue * queue, sc_event * evt, sc_addr edge, sc_addr other_el)
{
    g_assert(queue != 0);
    
    g_mutex_lock(&queue->mutex);
    if (queue->running == SC_TRUE)
    {
        sc_event_pool_worker_data * data = sc_event_pool_worker_data_new(evt, edge, other_el);
        g_thread_pool_push(queue->thread_pool, data, null_ptr);
    }
    g_mutex_unlock(&queue->mutex);
}

void sc_event_queue_remove(sc_event_queue *queue, sc_event *event)
{
    g_assert(queue != 0);
    g_mutex_lock(&queue->mutex);
    
    g_mutex_unlock(&queue->mutex);
}

