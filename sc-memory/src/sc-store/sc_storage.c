/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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

#include "sc_storage.h"

#include "sc_defines.h"
#include "sc_segment.h"
#include "sc_element.h"
#include "sc_fs_storage.h"
#include "sc_link_helpers.h"
#include "sc_event.h"
#include "sc_config.h"
#include "sc_iterator.h"

#include <memory.h>
#include <glib.h>

// segments array
sc_segment **segments = 0;
// number of segments
sc_uint16 segments_num = 0;
// number of stored sc-elements
sc_uint32 stored_elements_count = 0;

sc_uint seg_id = 0;
sc_uint seg_queue[SEGS_QUEUE_SIZE];
gint seg_queue_heap = -1;
sc_uint storage_time_stamp = 1;
sc_bool is_initialized = SC_FALSE;

#if SC_INTERNAL_THREADS_SUPPORT
    GStaticMutex seg_queue_mutex = G_STATIC_MUTEX_INIT;
    #define LOCK_SEG_QUEUE g_static_mutex_lock(&seg_queue_mutex);
    #define UNLOCK_SEG_QUEUE g_static_mutex_unlock(&seg_queue_mutex);
#else
    #define LOCK_SEG_QUEUE
    #define UNLOCK_SEG_QUEUE
#endif


// ----------------------------------- SEGMENTS QUEUE --------------------------
sc_bool _sc_storage_get_segment_from_queue(sc_addr_seg *seg)
{
    sc_bool res = SC_FALSE;
    sc_segment *segment = 0;

    LOCK_SEG_QUEUE;

    if (seg_queue_heap > -1)
    {
        *seg = seg_queue[seg_queue_heap];
        segment = sc_storage_get_segment(*seg, SC_TRUE);
        g_assert( segment != 0 );

        if (sc_segment_has_empty_slot(segment) == SC_TRUE)
            res = SC_TRUE;
        else
            seg_queue_heap--;
    }

    UNLOCK_SEG_QUEUE;

    return res;
}

void _sc_storage_append_segment_to_queue(sc_addr_seg seg)
{
    LOCK_SEG_QUEUE;
    if (seg_queue_heap < (SEGS_QUEUE_SIZE - 1))
        seg_queue[++seg_queue_heap] = seg;

    UNLOCK_SEG_QUEUE;
}

void _sc_storage_update_segment_queue_impl(gpointer data,
                                           gpointer user_data)
{
    //! TODO: make synhronization on case, when segment removed, or unloaded
//    sc_segment *segment;
//    sc_addr_seg idx = 0;
//    while ( (seg_queue_heap <= SEGS_QUEUE_SIZE) && (idx < segments->len) )
//    {
//        segment = sc_storage_get_segment(idx, SC_TRUE);
//        g_assert( segment != (sc_segment*)0 );
//        if (sc_segment_have_empty_slot(segment) == SC_TRUE)
//            _sc_storage_append_segment_to_queue(idx);
//    }
}

// -----------------------------------------------------------------------------

/* Updates segment information:
 * - Calculate number of stored sc-elements
 * - Free unused cells in segments
 */
void sc_storage_update_segments()
{
    sc_uint32 idx = 0;
    sc_uint32 elements_count = 0;
    sc_uint32 element_free_count = 0;
    sc_uint32 oldest_time_stamp = 0;
    sc_segment *seg = 0;

    stored_elements_count = 0;

    oldest_time_stamp = sc_iterator_get_oldest_timestamp();
    if (oldest_time_stamp == 0)
        oldest_time_stamp = sc_storage_get_time_stamp();

    for (idx = 0; idx < segments_num; ++idx)
    {
        seg = segments[idx];
        if (seg == 0) continue; // @todo segments load
        elements_count = sc_segment_get_elements_count(seg);
        stored_elements_count += elements_count;
        // @todo oldest timestamp
        element_free_count = sc_segment_free_garbage(seg, oldest_time_stamp);

        if (elements_count < SEGMENT_SIZE || element_free_count > 0)
            _sc_storage_append_segment_to_queue(idx);
    }
}

// -----------------------------------------------------------------------------


sc_bool sc_storage_initialize(const char *path)
{
    g_assert( segments == (sc_segment**)0 );
    g_assert( !is_initialized );

    segments = g_new0(sc_segment*, SC_ADDR_SEG_MAX);

    sc_fs_storage_initialize(path);
    sc_fs_storage_read_from_path(segments, &segments_num);

    storage_time_stamp = 1;

    /*  seg_queue_update_pool = g_thread_pool_new(_sc_storage_update_segment_queue_impl,
                        (gpointer)0,
                        SEG_QUEUE_UPDATE_THREADS_MAX,
                        SC_FALSE,
                        (GError**)0);
  */
    is_initialized = SC_TRUE;
    sc_storage_update_segments();

    return SC_TRUE;
}

void sc_storage_shutdown()
{
    sc_uint idx = 0;
    g_assert( segments != (sc_segment**)0 );

    /*g_thread_pool_free( seg_queue_update_pool,
              SC_TRUE, SC_FALSE);
  seg_queue_update_pool = (GThreadPool*)0;
  */
    sc_fs_storage_shutdown(segments);

    for (idx = 0; idx < SC_ADDR_SEG_MAX; idx++)
    {
        if (segments[idx] == nullptr) continue; // skip segments, that are not loaded
        g_free(segments[idx]);
    }

    g_free(segments);
    segments = (sc_segment**)0;

    is_initialized = SC_FALSE;
}

sc_bool sc_storage_is_initialized()
{
    return is_initialized;
}

sc_segment* sc_storage_get_segment(sc_addr_seg seg, sc_bool force_load)
{
    //! TODO: Make support of force loading
    g_assert( seg < SC_ADDR_SEG_MAX );
    return segments[seg];
}

sc_element* sc_storage_get_element(sc_addr addr, sc_bool force_load)
{
    sc_segment *segment = 0;
    sc_element *res = 0;

    if (addr.seg >= SC_ADDR_SEG_MAX) return (sc_element*)0;

    segment = segments[addr.seg];

    if (segment == 0)
    {
        if (force_load)
        {
            //! TODO: make force load
        }else
            return (sc_element*)0;
    }else
    {
        res = sc_segment_get_element(segment, addr.offset);
    }

    return res;//sc_segment_get_element(segment, uri.id);
}

sc_bool sc_storage_is_element(sc_addr addr)
{
    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    if (el == 0) return SC_FALSE;
    if (el->type == 0) return SC_FALSE;
    if (el->delete_time_stamp > 0) return SC_FALSE;

    return SC_TRUE;
}

void sc_storage_update_segment_queue()
{

}

sc_element* sc_storage_append_el_into_segments(sc_element *element, sc_addr *addr)
{
    sc_segment *segment = 0;

    g_assert( addr != 0 );
    SC_ADDR_MAKE_EMPTY(*addr);

    if (sc_iterator_has_any_timestamp())
        storage_time_stamp++;

    // try to collect and delete garbage
    if (segments_num >= sc_config_get_max_loaded_segments())
        sc_storage_update_segments();

    if (_sc_storage_get_segment_from_queue(&addr->seg) == SC_TRUE)
    {
        segment = sc_storage_get_segment(addr->seg, SC_TRUE);
        return sc_segment_append_element(segment, element, &addr->offset);
    }

    //! @todo maximum segments reached
    if (segments_num >= sc_config_get_max_loaded_segments())
        return nullptr;

    // if element still not added, then create new segment and append element into it
    segment = sc_segment_new(segments_num);
    addr->seg = segments_num;
    segments[segments_num++] = segment;

    _sc_storage_append_segment_to_queue(addr->seg);

    return sc_segment_append_element(segment, element, &addr->offset);
}

sc_addr sc_storage_element_new(sc_type type)
{
    sc_element el;
    sc_addr addr;
    sc_element *res = 0;

    memset(&el, 0, sizeof(el));
    el.type = type;
    el.create_time_stamp = storage_time_stamp;

    res = sc_storage_append_el_into_segments(&el, &addr);
    g_assert(res != 0);
    return addr;
}

sc_result sc_storage_element_free(sc_addr addr)
{
    sc_element *el, *el2;
    sc_addr _addr;
    sc_uint addr_int;

    GSList *remove_list = 0;

    el = el2 = 0;

    if (sc_storage_is_element(addr) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_iterator_has_any_timestamp())
        storage_time_stamp++;

    remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)));

    while (remove_list != 0)
    {
        // get sc-addr for removing
        addr_int = GPOINTER_TO_UINT(remove_list->data);
        _addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_int);
        _addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_int);

        // go to next sc-addr in list
        remove_list = g_slist_delete_link(remove_list, remove_list);

        el = sc_storage_get_element(_addr, SC_TRUE);
        g_assert(el != 0 && el->type != 0);

        // remove registered events before deletion
        sc_event_notify_element_deleted(_addr);

        el->delete_time_stamp = storage_time_stamp;

        if (el->type & sc_type_arc_mask)
        {
            sc_event_emit(el->arc.begin, SC_EVENT_REMOVE_OUTPUT_ARC, _addr);
            sc_event_emit(el->arc.end, SC_EVENT_REMOVE_INPUT_ARC, _addr);
        }

        // Iterate all connectors for deleted element and append them into remove_list
        _addr = el->first_out_arc;
        while (SC_ADDR_IS_NOT_EMPTY(_addr))
        {
            el2 = sc_storage_get_element(_addr, SC_TRUE);

            // do not append elements, that have delete_time_stamp != 0
            if (el2->delete_time_stamp == 0)
                remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr)));

            _addr = el2->arc.next_out_arc;
        }

        _addr = el->first_in_arc;
        while (SC_ADDR_IS_NOT_EMPTY(_addr))
        {
            el2 = sc_storage_get_element(_addr, SC_TRUE);

            // do not append elements, that have delete_time_stamp != 0
            if (el2->delete_time_stamp == 0)
                remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr)));


            _addr = el2->arc.next_in_arc;
        }

        // clean temp addr
        SC_ADDR_MAKE_EMPTY(_addr);
    }

    return SC_RESULT_OK;
}

sc_addr sc_storage_node_new(sc_type type )
{
    sc_element el;
    sc_addr addr;

    g_assert( !(sc_type_arc_mask & type) );
    memset(&el, 0, sizeof(el));

    el.type = sc_type_node | type;

    sc_storage_append_el_into_segments(&el, &addr);
    return addr;
}

sc_addr sc_storage_link_new()
{
    sc_element el;
    sc_addr addr;

    memset(&el, 0, sizeof(el));
    el.type = sc_type_link;
    sc_storage_append_el_into_segments(&el, &addr);
    return addr;
}

sc_addr sc_storage_arc_new(sc_type type,
                           sc_addr beg,
                           sc_addr end)
{
    sc_addr addr;
    sc_element el, *beg_el, *end_el, *tmp_el;
#if USE_TWO_ORIENTED_ARC_LIST
    sc_element *tmp_arc;
#endif

    memset(&el, 0, sizeof(el));
    g_assert( !(sc_type_node & type) );
    el.type = (type & sc_type_arc_mask) ? type : (sc_type_arc_common | type);

    el.arc.begin = beg;
    el.arc.end = end;

    // get new element
    tmp_el = sc_storage_append_el_into_segments(&el, &addr);

    g_assert(tmp_el != 0);

    // get begin and end elements
    beg_el = sc_storage_get_element(beg, SC_TRUE);
    end_el = sc_storage_get_element(end, SC_TRUE);

    // emit events
    sc_event_emit(beg, SC_EVENT_ADD_OUTPUT_ARC, addr);
    sc_event_emit(end, SC_EVENT_ADD_INPUT_ARC, addr);
//    if (type & sc_type_edge_common)
//    {
//        sc_event_emit(end, SC_EVENT_ADD_OUTPUT_ARC, addr);
//        sc_event_emit(beg, SC_EVENT_ADD_INPUT_ARC, addr);
//    }

    // check values
    g_assert(beg_el != nullptr && end_el != nullptr);
    g_assert(beg_el->type != 0 && end_el->type != 0);

    // set next output arc for our created arc
    tmp_el->arc.next_out_arc = beg_el->first_out_arc;
    tmp_el->arc.next_in_arc = end_el->first_in_arc;

#if USE_TWO_ORIENTED_ARC_LIST
    if (SC_ADDR_IS_NOT_EMPTY(beg_el->first_out_arc))
    {
        tmp_arc = sc_storage_get_element(beg_el->first_out_arc, SC_TRUE);
        tmp_arc->arc.prev_out_arc = addr;
    }

    if (SC_ADDR_IS_NOT_EMPTY(end_el->first_in_arc))
    {
        tmp_arc = sc_storage_get_element(end_el->first_in_arc, SC_TRUE);
        tmp_arc->arc.prev_in_arc = addr;
    }

#endif

    // set our arc as first output/input at begin/end elements
    beg_el->first_out_arc = addr;
    end_el->first_in_arc = addr;

    return addr;
}

sc_result sc_storage_get_element_type(sc_addr addr, sc_type *result)
{
    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    if (el == 0)
        return SC_RESULT_ERROR;

    *result = el->type;

    return SC_RESULT_OK;
}

sc_result sc_storage_get_arc_begin(sc_addr addr, sc_addr *result)
{
    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    if (el->type & sc_type_arc_mask)
    {
        *result = el->arc.begin;
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR_INVALID_TYPE;
}

sc_result sc_storage_get_arc_end(sc_addr addr, sc_addr *result)
{
    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    if (el->type & sc_type_arc_mask)
    {
        *result = el->arc.end;
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR_INVALID_TYPE;
}

sc_result sc_storage_set_link_content(sc_addr addr, const sc_stream *stream)
{
    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    sc_check_sum check_sum;
    sc_result result = SC_RESULT_ERROR;

    g_assert(stream != nullptr);

    if (el == nullptr)
        return SC_RESULT_ERROR_INVALID_PARAMS;

    if (!(el->type & sc_type_link))
        return SC_RESULT_ERROR_INVALID_TYPE;

    // calculate checksum for data
    if (sc_link_calculate_checksum(stream, &check_sum) == SC_TRUE)
    {
        result = sc_fs_storage_write_content(addr, &check_sum, stream);
        memcpy(el->content.data, check_sum.data, check_sum.len);
        el->content.len = check_sum.len;

        g_assert(check_sum.len > 0);

        sc_event_emit(addr, SC_EVENT_CHANGE_LINK_CONTENT, addr);
        result = SC_RESULT_OK;
    }

    g_assert(result == SC_RESULT_OK);

    return result;
}

sc_result sc_storage_get_link_content(sc_addr addr, sc_stream **stream)
{
    sc_element *el = sc_storage_get_element(addr, SC_TRUE);
    sc_check_sum checksum;

    if (el == nullptr)
        return SC_RESULT_ERROR_INVALID_PARAMS;

    if (!(el->type & sc_type_link))
        return SC_RESULT_ERROR_INVALID_TYPE;


    // prepare checksum
    checksum.len = el->content.len;
    memcpy(checksum.data, el->content.data, checksum.len);

    return sc_fs_storage_get_checksum_content(&checksum, stream);
}

sc_result sc_storage_find_links_with_content(const sc_stream *stream, sc_addr **result, sc_uint32 *result_count)
{
    g_assert(stream != 0);
    sc_check_sum check_sum;
    if (sc_link_calculate_checksum(stream, &check_sum) == SC_TRUE)
        return sc_fs_storage_find_links_with_content(&check_sum, result, result_count);

    return SC_RESULT_ERROR;
}


sc_result sc_storage_get_elements_stat(sc_stat *stat)
{
    sc_uint s_idx, e_idx;
    sc_segment *segment;
    sc_type type;
    sc_uint32 delete_stamp;
    g_assert( stat != (sc_stat*)0 );

    memset(stat, 0, sizeof(sc_stat));

    //! TODO: add loading of segment

    // iterate all elements and calculate statistics
    for (s_idx = 0; s_idx < segments_num; s_idx++)
    {
        segment = segments[s_idx];
        g_assert( segment != (sc_segment*)0 );
        for (e_idx = 0; e_idx < SEGMENT_SIZE; e_idx++)
        {
            type = segment->elements[e_idx].type;
            delete_stamp = segment->elements[e_idx].delete_time_stamp;
            if (type == 0)
            {
                stat->empty_count++;
            }
            else
            {
                if (type & sc_type_node)
                {
                    stat->node_count++;
                    if (delete_stamp == 0)
                        stat->node_live_count++;
                }
                else
                {
                    if (type & sc_type_arc_mask)
                    {
                        stat->arc_count++;
                        if (delete_stamp == 0)
                            stat->arc_live_count++;
                    }else
                    {
                        if (type & sc_type_link)
                        {
                            stat->link_count++;
                            if (delete_stamp == 0)
                                stat->link_live_count++;
                        }
                    }
                }
            }
        }
    }
}

sc_uint sc_storage_get_time_stamp()
{
    return storage_time_stamp;
}

unsigned int sc_storage_get_segments_count()
{
    return segments_num;
}

