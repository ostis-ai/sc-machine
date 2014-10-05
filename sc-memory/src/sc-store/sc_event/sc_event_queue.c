/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sc_event_queue.h"
#include "sc_event.h"
#include "sc_event_private.h"
#include "../sc_memory_private.h"


gpointer sc_event_queue_thread_loop(gpointer data)
{
    sc_event_queue *queue = (sc_event_queue*)data;

    g_rec_mutex_lock(&queue->mutex);
    queue->running = SC_TRUE;
    g_rec_mutex_unlock(&queue->mutex);

    sc_bool running = SC_TRUE;
    sc_event *event = 0;
    sc_addr arg;

    while (running == SC_TRUE || g_queue_get_length(queue->queue) > 0)
    {
        g_rec_mutex_lock(&queue->mutex);
        running = queue->running;
        sc_event_queue_item *item = (sc_event_queue_item*)g_queue_pop_head(queue->queue);
        g_rec_mutex_unlock(&queue->mutex);

        event = 0;

        // find first not null event in queue
        while (item != 0)
        {
            // if pointer to event is null, then event removed, we need to skip it
            if (item->event)
            {
                event = item->event;
                arg = item->arg;

                sc_access_levels arg_access;
                if (sc_storage_get_access_levels(s_memory_default_ctx, arg, &arg_access) != SC_RESULT_OK)
                    arg_access = sc_access_lvl_make_max;

                g_free(item);
                break;
            }
            else
            {
                g_free(item);
                g_rec_mutex_lock(&queue->mutex);
                item = (sc_event_queue_item*)g_queue_pop_head(queue->queue);
                g_rec_mutex_unlock(&queue->mutex);
            }
        }

        g_rec_mutex_lock(&queue->proc_mutex);
        queue->event_process = event;

        if (queue->event_process)
            queue->event_process->callback(queue->event_process, arg);

        queue->event_process = 0;
        g_rec_mutex_unlock(&queue->proc_mutex);

        g_usleep(1000); // sleep for an one millisecond
    }

    return 0;
}

sc_event_queue* sc_event_queue_new()
{
    sc_event_queue *queue = g_new0(sc_event_queue, 1);
    queue->running = SC_FALSE;
    g_rec_mutex_init(&queue->mutex);
    g_rec_mutex_init(&queue->proc_mutex);
    queue->thread = g_thread_new("sc_event_queue thread", sc_event_queue_thread_loop, (gpointer)queue);
    queue->queue = g_queue_new();

    return queue;
}


void sc_event_queue_destroy_wait(sc_event_queue *queue)
{
    g_assert(queue != 0);

    sc_bool is_running = SC_FALSE;

    g_rec_mutex_lock(&queue->mutex);
    is_running = queue->running;
    g_rec_mutex_unlock(&queue->mutex);

    if (is_running)
    {
        g_rec_mutex_lock(&queue->mutex);
        queue->running = SC_FALSE;
        GThread *thread = queue->thread;
        queue->thread = 0;
        g_rec_mutex_unlock(&queue->mutex);

        g_thread_join(thread);
    }

}

void sc_event_queue_append(sc_event_queue *queue, sc_event *event, sc_addr arg)
{
    g_assert(queue != 0);
    g_rec_mutex_lock(&queue->mutex);

    sc_event_queue_item *item = g_new0(sc_event_queue_item, 1);
    item->arg = arg;
    item->event = event;
    g_queue_push_tail(queue->queue, (gpointer)item);

    g_rec_mutex_unlock(&queue->mutex);
}

void _sc_event_queue_item_remove(gpointer _item, gpointer _event)
{
    sc_event_queue_item *item = (sc_event_queue_item*)_item;
    sc_event *event = (sc_event*)_event;

    if (item->event == event)
        item->event = 0;
}

void _sc_event_queue_item_remove_by_addr(gpointer _item, gpointer _addr)
{
    sc_event_queue_item *item = (sc_event_queue_item*)_item;

    if ((SC_ADDR_LOCAL_TO_INT(item->arg) == GPOINTER_TO_UINT(_addr)) && (sc_event_get_type(item->event)!= SC_EVENT_REMOVE_ELEMENT))
        item->event = 0;
}

void sc_event_queue_remove(sc_event_queue *queue, sc_event *event)
{
    g_assert(queue != 0);
    g_rec_mutex_lock(&queue->mutex);

    g_rec_mutex_lock(&queue->proc_mutex);
    if (queue->event_process == event)
        queue->event_process = 0;
    g_rec_mutex_unlock(&queue->proc_mutex);

    g_queue_foreach(queue->queue, _sc_event_queue_item_remove, (gpointer)event);

    g_rec_mutex_unlock(&queue->mutex);

}

void sc_event_queue_remove_element(sc_event_queue *queue, sc_addr addr)
{
    g_assert(queue != 0);
    g_rec_mutex_lock(&queue->mutex);

    g_queue_foreach(queue->queue, _sc_event_queue_item_remove_by_addr, (gpointer)SC_ADDR_LOCAL_TO_INT(addr));

    g_rec_mutex_unlock(&queue->mutex);
}
