/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <glib.h>
#include "sc_event.h"
#include "sc_storage.h"
#include "sc_event/sc_event_private.h"
#include "sc_event/sc_event_queue.h"
#include "../sc_memory_private.h"

GMutex events_table_mutex;
#define EVENTS_TABLE_LOCK g_mutex_lock(&events_table_mutex);
#define EVENTS_TABLE_UNLOCK g_mutex_unlock(&events_table_mutex);


// Pointer to hash table that contains events
GHashTable *events_table = 0;
sc_event_queue *event_queue = 0;

guint events_table_hash_func(gconstpointer pointer)
{
    const sc_addr *addr = (const sc_addr*)pointer;
    return SC_ADDR_LOCAL_TO_INT(*addr);
}

gboolean events_table_equal_func(gconstpointer a, gconstpointer b)
{
    const sc_addr *addr1 = (const sc_addr*)a;
    const sc_addr *addr2 = (const sc_addr*)b;
    return SC_ADDR_IS_EQUAL(*addr1, *addr2);
}

//! Inserts specified event into events table
sc_result insert_event_into_table(sc_event *event)
{
    GSList *element_events_list = 0;

    EVENTS_TABLE_LOCK

    // first of all, if table doesn't exist, then create it
    if (events_table == null_ptr)
        events_table = g_hash_table_new(events_table_hash_func, events_table_equal_func);

    // if there are no events for specified sc-element, then create new events list
    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&event->element);
    element_events_list = g_slist_append(element_events_list, (gpointer)event);
    g_hash_table_insert(events_table, (gpointer)&event->element, (gpointer)element_events_list);

    EVENTS_TABLE_UNLOCK

    return SC_RESULT_OK;
}

//! Remove specified sc-event from events table
sc_result remove_event_from_table(sc_event *event)
{
    GSList *element_events_list = 0;
    g_assert(events_table != null_ptr);

    EVENTS_TABLE_LOCK

    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&event->element);
    if (element_events_list == null_ptr)
    {
        EVENTS_TABLE_UNLOCK
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    // remove event from list of events for specified sc-element
    element_events_list = g_slist_remove(element_events_list, (gconstpointer)event);
    if (element_events_list == null_ptr)
    {
        g_hash_table_remove(events_table, (gconstpointer)&event->element);
    }
    else
    {
        g_hash_table_insert(events_table, (gpointer)&event->element, (gpointer)element_events_list);
    }

    // if there are no more events in table, then delete it
    if (g_hash_table_size(events_table) == 0)
    {
        g_hash_table_destroy(events_table);
        events_table = null_ptr;
    }

    EVENTS_TABLE_UNLOCK

    return SC_RESULT_OK;
}

/// -----------------------------------------
sc_bool _sc_event_try_emit(sc_memory_context const * ctx, sc_event * evt)
{
    sc_bool res = SC_TRUE;

    sc_event_lock(evt, ctx);
    if (evt->ref_count & SC_EVENT_REQUEST_DESTROY)
    {
        res = SC_FALSE;
    }
    else
    {
        g_assert(evt->ref_count < SC_EVENT_REF_COUNT_MASK);
        evt->ref_count++;
    }

    sc_event_unlock(evt, ctx);
    return res;
}

sc_bool sc_event_try_free(sc_memory_context const * ctx, sc_event * evt)
{
    sc_event_lock(evt, ctx);
    if (evt->ref_count & SC_EVENT_REQUEST_DESTROY)
    {
        if ((evt->ref_count & SC_EVENT_REQUEST_DESTROY) == 0)
            g_critical("Invalid state of event: There are no flag SC_EVENT_REQUEST_DESTROY in event with 0 references. Need to debug");

        evt->ref_count--;
        sc_uint32 const count = evt->ref_count & SC_EVENT_REF_COUNT_MASK;
        if (count == 0)
        {
            sc_storage_element_unref(ctx, evt->element);
            if (evt->delete_callback != null_ptr)
                evt->delete_callback(evt);
            g_free(evt);

            return SC_TRUE;
        }
    }

    sc_event_unlock(evt, ctx);
    return SC_FALSE;
}

// TODO: remove in 0.4.0
sc_event* sc_event_new(sc_memory_context const * ctx, sc_addr el, sc_event_type type, sc_pointer data, fEventCallback callback, fDeleteCallback delete_callback)
{
    if (SC_ADDR_IS_EMPTY(el))
        return null_ptr;

    sc_access_levels levels;
    sc_event *event = null_ptr;
    if (sc_storage_get_access_levels(ctx, el, &levels) != SC_RESULT_OK || !sc_access_lvl_check_read(ctx->access_levels, levels))
        return 0;

    sc_storage_element_ref(ctx, el);

    event = g_new0(sc_event, 1);
    event->element = el;
    event->type = type;
    event->callback = callback;
    event->delete_callback = delete_callback;
    event->data = data;
    event->ctx = ctx;
    event->ctx_lock = null_ptr;
    event->ref_count = 1;

    g_assert(callback != null_ptr);

    // register created event
    if (insert_event_into_table(event) != SC_RESULT_OK)
    {
        g_free(event);
        return null_ptr;
    }

    return event;
}

sc_event* sc_event_new_ex(sc_memory_context const * ctx, sc_addr el, sc_event_type type, sc_pointer data, fEventCallbackEx callback, fDeleteCallback delete_callback)
{
    if (SC_ADDR_IS_EMPTY(el))
        return null_ptr;

    sc_access_levels levels;
    sc_event *event = null_ptr;
    if (sc_storage_get_access_levels(ctx, el, &levels) != SC_RESULT_OK || !sc_access_lvl_check_read(ctx->access_levels, levels))
        return 0;

    sc_storage_element_ref(ctx, el);

    event = g_new0(sc_event, 1);
    event->element = el;
    event->type = type;
    event->callback_ex = callback;
    event->delete_callback = delete_callback;
    event->data = data;
    event->ctx = ctx;
    event->ctx_lock = null_ptr;
    event->ref_count = 1;

    g_assert(callback != null_ptr);

    // register created event
    if (insert_event_into_table(event) != SC_RESULT_OK)
    {
        g_free(event);
        return null_ptr;
    }

    return event;
}

sc_result sc_event_destroy(sc_event * evt)
{
    sc_event_lock(evt, evt->ctx);
    if (evt->ref_count & SC_EVENT_REQUEST_DESTROY)
    {
        goto exit;
    }

    if (remove_event_from_table(evt) != SC_RESULT_OK)
        return SC_RESULT_ERROR;
        
    evt->ref_count |= SC_EVENT_REQUEST_DESTROY;
    evt->callback = null_ptr;
    evt->callback_ex = null_ptr;
    sc_event_unlock(evt, evt->ctx);

exit:
    {
        sc_event_try_free(evt->ctx, evt);
    }
    
    return SC_RESULT_OK;
}

sc_result sc_event_notify_element_deleted(sc_addr element)
{
    GSList *element_events_list = 0;
    sc_event *evt = 0;

    EVENTS_TABLE_LOCK
    // do nothing, if there are no registered events
    if (events_table == null_ptr)
        goto result;

    // lookup for all registered to specified sc-elemen events
    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&element);
    if (element_events_list)
    {
        while (element_events_list != null_ptr)
        {
            evt = (sc_event*)element_events_list->data;

            // mark event for deletion
            sc_event_lock(evt, evt->ctx);
            evt->ref_count |= SC_EVENT_REQUEST_DESTROY;
            sc_event_unlock(evt, evt->ctx);

            sc_event_try_free(evt->ctx, evt);

            element_events_list = g_slist_delete_link(element_events_list, element_events_list);
        }
        g_hash_table_remove(events_table, (gconstpointer)&element);
        g_slist_free(element_events_list);
    }

    result:
    {
        EVENTS_TABLE_UNLOCK;
    }

    return SC_RESULT_OK;
}

sc_result sc_event_emit(sc_memory_context const * ctx, sc_addr el, sc_access_levels el_access, sc_event_type type, sc_addr edge, sc_addr other_el)
{
    GSList *element_events_list = 0;
    sc_event *event = 0;

    g_assert(SC_ADDR_IS_NOT_EMPTY(el));

    EVENTS_TABLE_LOCK;

    // if table is empty, then do nothing
    if (events_table == null_ptr)
        goto result;

    // lookup for all registered to specified sc-elemen events
    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&el);
    while (element_events_list != null_ptr)
    {
        event = (sc_event*)element_events_list->data;

        if (event->type == type &&
            sc_access_lvl_check_read(event->ctx->access_levels, el_access) &&
            _sc_event_try_emit(ctx, event) == SC_TRUE)
        {
            g_assert(event->callback != null_ptr || event->callback_ex != null_ptr);
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

sc_event_type sc_event_get_type(const sc_event *event)
{
    g_assert(event != 0);
    return event->type;
}

sc_pointer sc_event_get_data(const sc_event *event)
{
    g_assert(event != 0);
    return event->data;
}

sc_addr sc_event_get_element(const sc_event *event)
{
    g_assert(event != 0);
    return event->element;
}

void sc_event_lock(sc_event * evt, sc_memory_context const * ctx)
{
    while (TRUE)
    {
        sc_memory_context * locked_ctx = g_atomic_pointer_get(&evt->ctx_lock);
        if (locked_ctx == ctx)
            return;

        if (locked_ctx != null_ptr)
            continue;

        if (g_atomic_pointer_compare_and_exchange(&evt->ctx_lock, locked_ctx, ctx) == FALSE)
        {
            g_usleep(rand() % 10);
        }
        else
        {
            break;
        }
    }
}

void sc_event_unlock(sc_event * evt, sc_memory_context const * ctx)
{
    sc_memory_context * locked_ctx = g_atomic_pointer_get(&evt->ctx_lock);
    if (locked_ctx != ctx)
        g_critical("Invalid state of event lock");

    g_atomic_pointer_set(&evt->ctx_lock, 0);
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
    event_queue = 0;
}

void sc_events_stop_processing()
{
    sc_event_queue_destroy_wait(event_queue);
}
