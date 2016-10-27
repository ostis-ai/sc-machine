/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
#include "sc_stream_memory.h"

#include "sc_event/sc_event_private.h"
#include "../sc_memory_private.h"

#include <memory.h>
#include <glib.h>

// segments array
sc_segment **segments = 0;
// number of segments
sc_uint32 segments_num = 0;

const sc_uint16 s_max_storage_lock_attempts = 100;
const sc_uint16 s_max_storage_cache_attempts = 10;

sc_bool is_initialized = SC_FALSE;

sc_memory_context *segments_cache_lock_ctx = 0;
sc_int32 segments_cache_count = 0;
sc_segment* segments_cache[SC_SEGMENT_CACHE_SIZE]; // cache of segments that have empty elements

GMutex s_mutex_free;
GMutex s_mutex_save;

#define CONCURRENCY_TO_CACHE_IDX(x) ((x) % SC_SEGMENT_CACHE_SIZE)

void _sc_segment_cache_lock(const sc_memory_context * ctx)
{
    while (g_atomic_pointer_compare_and_exchange(&segments_cache_lock_ctx, 0, ctx) == FALSE) {}
}

void _sc_segment_cache_unlock(const sc_memory_context *ctx)
{
    g_assert(g_atomic_pointer_get(&segments_cache_lock_ctx) == ctx);
    g_atomic_pointer_set(&segments_cache_lock_ctx, 0);
}

void _sc_segment_cache_append(const sc_memory_context * ctx, sc_segment * seg)
{
    sc_int32 i, idx = CONCURRENCY_TO_CACHE_IDX(ctx->id);
    for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
    {
        if (g_atomic_pointer_compare_and_exchange(&segments_cache[(idx + i) % SC_SEGMENT_CACHE_SIZE], 0, seg) == TRUE)
        {
            g_atomic_int_inc(&segments_cache_count);
            break;
        }
    }
}

void _sc_segment_cache_remove(const sc_memory_context *ctx, sc_segment *seg)
{
    sc_int32 i, idx = CONCURRENCY_TO_CACHE_IDX(ctx->id);
    for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
    {
        if (g_atomic_pointer_compare_and_exchange(&segments_cache[(idx + i) % SC_SEGMENT_CACHE_SIZE], seg, 0) == TRUE)
        {
            g_atomic_int_add(&segments_cache_count, -1);
            break;
        }
    }
}

void _sc_segment_cache_clear()
{
    sc_int32 i;
    for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
        g_atomic_pointer_set(&segments_cache[i], 0);
}

void _sc_segment_cache_update(const sc_memory_context *ctx)
{
    // trying to push segments to cache
    sc_int32 i;
    for (i = 0; i < g_atomic_int_get(&segments_num); ++i)
    {
        sc_segment *s = g_atomic_pointer_get(&(segments[i]));
        // need to check pointer, because segments_num increments earlier, then segments appends into array
        if (s != null_ptr)
        {
            if (sc_segment_has_empty_slot(s))
                _sc_segment_cache_append(ctx, s);
        }

        if (g_atomic_int_get(&segments_cache_count) == SC_SEGMENT_CACHE_SIZE)
            break;
    }
}

sc_segment* _sc_segment_cache_get(const sc_memory_context *ctx)
{
    _sc_segment_cache_lock(ctx);

    g_mutex_lock(&s_mutex_save);

    sc_segment *seg = 0;
    if (g_atomic_int_get(&segments_cache_count) > 0)
    {
        sc_int32 i, idx = CONCURRENCY_TO_CACHE_IDX(ctx->id);
        for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
        {
            seg = g_atomic_pointer_get(&segments_cache[(idx + i) % SC_SEGMENT_CACHE_SIZE]);
            if (seg != null_ptr)
                goto result;
        }
    }

    // try to update cache
    _sc_segment_cache_update(ctx);

    // if element still not added, then create new segment and append element into it
    sc_int32 seg_num = g_atomic_int_add(&segments_num, 1);
    seg = sc_segment_new(seg_num);
    segments[seg_num] = seg;
    _sc_segment_cache_append(ctx, seg);

    result:
    {
        _sc_segment_cache_unlock(ctx);
        g_mutex_unlock(&s_mutex_save);
    }

    return seg;
}


// -----------------------------------------------------------------------------

sc_bool sc_storage_initialize(const char *path, sc_bool clear)
{
    g_assert( segments == (sc_segment**)0 );
    g_assert( !is_initialized );

    segments = g_new0(sc_segment*, SC_ADDR_SEG_MAX);

    sc_bool res = sc_fs_storage_initialize(path, clear);
    if (res == SC_FALSE)
        return SC_FALSE;

    if (clear == SC_FALSE)
    {
        if (sc_fs_storage_read_from_path(segments, &segments_num) == SC_FALSE)
            return SC_FALSE;
    }

    is_initialized = SC_TRUE;

    memset(&(segments_cache[0]), 0, sizeof(sc_segment*) * SC_SEGMENT_CACHE_SIZE);

    return SC_TRUE;
}

void sc_storage_shutdown(sc_bool save_state)
{
    sc_uint idx = 0;
    g_assert( segments != (sc_segment**)0 );


    sc_fs_storage_shutdown(segments, save_state);

    for (idx = 0; idx < SC_ADDR_SEG_MAX; idx++)
    {
        if (segments[idx] == null_ptr) continue; // skip segments, that are not loaded
        sc_segment_free(segments[idx]);
    }

    g_free(segments);
    segments = (sc_segment**)0;
    segments_num = 0;

    is_initialized = SC_FALSE;
    _sc_segment_cache_clear();
}

sc_bool sc_storage_is_initialized()
{
    return is_initialized;
}

sc_bool sc_storage_is_element(const sc_memory_context *ctx, sc_addr addr)
{
    sc_element *el = 0;
    sc_bool res = SC_TRUE;
    
    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_FALSE;
        
    if (el->flags.type == 0 || (el->flags.type & sc_flag_request_deletion))
        res = SC_FALSE;

    sc_storage_element_unlock(ctx, addr);

    return res;
}

sc_element* sc_storage_append_el_into_segments(const sc_memory_context *ctx, sc_element *element, sc_addr *addr)
{
    sc_segment * seg = (sc_segment*)0x1;

    g_assert( addr != 0 );
    SC_ADDR_MAKE_EMPTY(*addr);

    if (g_atomic_int_get(&segments_num) >= sc_config_get_max_loaded_segments())
        return null_ptr;

    /// @todo store segment with empty slots
    // try to find segment with empty slots
    while (seg != 0)
    {
        sc_segment *seg = _sc_segment_cache_get(ctx);

        if (seg == null_ptr)
            break;

        sc_element *el = sc_segment_lock_empty_element(ctx, seg, &addr->offset);
        if (el != null_ptr)
        {
            addr->seg = seg->num;
            *el = *element;
            el->flags.access_levels = sc_access_lvl_min(ctx->access_levels, el->flags.access_levels);
            return el;
        }else
            _sc_segment_cache_remove(ctx, seg);
    }

    return null_ptr;
}

sc_addr sc_storage_element_new_access(const sc_memory_context *ctx, sc_type type, sc_access_levels access_levels)
{
    sc_element el;
    sc_addr addr;
    sc_element *res = 0;

    memset(&el, 0, sizeof(el));
    el.flags.type = type;
    el.flags.access_levels = access_levels;

    res = sc_storage_append_el_into_segments(ctx, &el, &addr);
    sc_storage_element_unlock(ctx, addr);
    g_assert(res != 0);
    return addr;
}

sc_result sc_storage_element_free(const sc_memory_context *ctx, sc_addr addr)
{
    GHashTable *remove_table = 0, *lock_table = 0;
    GSList *remove_list = 0;
    sc_result result = SC_RESULT_OK;

    g_mutex_lock(&s_mutex_free);

    // first of all we need to collect and lock all elements
    sc_element *el;
    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
    {
        g_mutex_unlock(&s_mutex_free);
        return SC_RESULT_ERROR;
    }

    g_assert(el != 0);
    if (el->flags.type == 0 || el->flags.type & sc_flag_request_deletion)
    {
        g_mutex_unlock(&s_mutex_free);
        sc_storage_element_unlock(ctx, addr);
        return SC_RESULT_ERROR;
    }

    remove_table = g_hash_table_new(g_direct_hash, g_direct_equal);
    lock_table = g_hash_table_new(g_direct_hash, g_direct_equal);
    g_hash_table_insert(remove_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)), el);
    g_hash_table_insert(lock_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)), el);

    remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)));
    while (remove_list != 0)
    {
        // get sc-addr for removing
        sc_uint32 addr_int = GPOINTER_TO_UINT(remove_list->data);
        sc_addr _addr;
        _addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_int);
        _addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_int);

        gpointer p_addr = GUINT_TO_POINTER(addr_int);

        // go to next sc-addr in list
        remove_list = g_slist_delete_link(remove_list, remove_list);

        if (!sc_access_lvl_check_write(ctx->access_levels, el->flags.access_levels))
        {
            result = SC_RESULT_ERROR_NO_WRITE_RIGHTS;
            goto unlock;
        }

        el = g_hash_table_lookup(lock_table, p_addr);
        if (el == null_ptr)
        {
            STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, _addr, &el));

            g_assert(el->flags.type != 0);
            g_hash_table_insert(remove_table, p_addr, el);
            g_hash_table_insert(lock_table, p_addr, el);
        }

        if (el->flags.type & sc_type_arc_mask)
        {
            // lock begin and end elements of arc
            sc_element *el2 = 0;
            p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.begin));
            if ((el2 = g_hash_table_lookup(lock_table, p_addr)) == null_ptr)
            {
                STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.begin, &el2));
                g_hash_table_insert(lock_table, p_addr, el2);
            }

            p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.end));
            if ((el2 = g_hash_table_lookup(lock_table, p_addr)) == null_ptr)
            {
                el2 = 0;
                STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.end, &el2));
                g_hash_table_insert(lock_table, p_addr, el2);
            }

            // lock next/prev arcs in out/in lists
            if (SC_ADDR_IS_NOT_EMPTY(el->arc.prev_out_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.prev_out_arc));
                if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
                {
                    el2 = 0;
                    STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.prev_out_arc, &el2));
                    g_assert(el2 != 0);
                    g_hash_table_insert(lock_table, p_addr, el2);
                }
            }

            if (SC_ADDR_IS_NOT_EMPTY(el->arc.prev_in_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.prev_in_arc));
                if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
                {
                    el2 = 0;
                    STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.prev_in_arc, &el2));
                    g_assert(el2 != 0);
                    g_hash_table_insert(lock_table, p_addr, el2);
                }
            }

            if (SC_ADDR_IS_NOT_EMPTY(el->arc.next_out_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.next_out_arc));
                if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
                {
                    el2 = 0;
                    STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.next_out_arc, &el2));
                    g_assert(el2 != 0);
                    g_hash_table_insert(lock_table, p_addr, el2);
                }
            }

            if (SC_ADDR_IS_NOT_EMPTY(el->arc.next_in_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.next_in_arc));
                if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
                {
                    el2 = 0;
                    STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.next_in_arc, &el2));
                    g_assert(el2 != 0);
                    g_hash_table_insert(lock_table, p_addr, el2);
                }
            }
        }

        // Iterate all connectors for deleted element and append them into remove_list
        _addr = el->first_out_arc;
        while (SC_ADDR_IS_NOT_EMPTY(_addr))
        {
            gpointer p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));
            sc_element *el2 = g_hash_table_lookup(remove_table, p_addr);

            if (el2 == null_ptr)
            {
                el2 = g_hash_table_lookup(lock_table, p_addr);
                if (el2 == null_ptr)
                {
                    sc_storage_element_lock(ctx, _addr, &el2);
                    g_hash_table_insert(lock_table, p_addr, el2);
                }

                g_assert(el2 != null_ptr);
                g_hash_table_insert(remove_table, p_addr, el2);

                remove_list = g_slist_append(remove_list, p_addr);
            }

            _addr = el2->arc.next_out_arc;
        }

        _addr = el->first_in_arc;
        while (SC_ADDR_IS_NOT_EMPTY(_addr))
        {
            gpointer p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));
            sc_element *el2 = g_hash_table_lookup(remove_table, p_addr);

            if (el2 == null_ptr)
            {
                el2 = g_hash_table_lookup(lock_table, p_addr);
                if (el2 == null_ptr)
                {
                    sc_storage_element_lock(ctx, _addr, &el2);
                    g_hash_table_insert(lock_table, p_addr, el2);
                }

                g_assert(el2 != null_ptr);
                g_hash_table_insert(remove_table, p_addr, el2);

                remove_list = g_slist_append(remove_list, p_addr);
            }

            _addr = el2->arc.next_in_arc;
        }

        // clean temp addr
        SC_ADDR_MAKE_EMPTY(_addr);
    }

    // now we need to erase all elements
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, remove_table);
    gpointer key, value;
    while (g_hash_table_iter_next(&iter, &key, &value) == TRUE)
    {
        el = value;
        sc_uint32 uint_addr = GPOINTER_TO_UINT(key);
        gpointer p_addr;
        addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(uint_addr);
        addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(uint_addr);
        sc_access_levels el_access = el->flags.access_levels;

        if (el->flags.type & sc_flag_request_deletion)
            continue;

        if (el->flags.type & sc_type_link)
        {
            sc_check_sum sum;

            if (el->flags.type & sc_flag_link_self_container)
                sc_link_self_container_calculate_checksum(el, &sum);
            else
            {
                memcpy(&sum.data[0], el->content.data, SC_CHECKSUM_LEN);
                sum.len = SC_CHECKSUM_LEN;
            }

            STORAGE_CHECK_CALL(sc_fs_storage_remove_content_addr(addr, &sum));
        }
        else if (el->flags.type & sc_type_arc_mask)
        {
            // output arcs
            sc_addr prev_arc = el->arc.prev_out_arc;
            sc_addr next_arc = el->arc.next_out_arc;

            if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(prev_arc));
                sc_element *prev_el_arc = g_hash_table_lookup(lock_table, p_addr);
                g_assert(prev_el_arc != null_ptr);
                prev_el_arc->arc.next_out_arc = next_arc;

            }

            if (SC_ADDR_IS_NOT_EMPTY(next_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(next_arc));
                sc_element *next_el_arc = g_hash_table_lookup(lock_table, p_addr);
                g_assert(next_el_arc != null_ptr);
                next_el_arc->arc.prev_out_arc = prev_arc;
            }

            sc_element *b_el = g_hash_table_lookup(lock_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.begin)));
            sc_bool need_unlock = SC_FALSE;
            if (b_el == null_ptr)
            {
                STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.begin, &b_el));
                need_unlock = SC_TRUE;
            }
            if (SC_ADDR_IS_EQUAL(addr, b_el->first_out_arc))
                b_el->first_out_arc = next_arc;

            sc_event_emit(el->arc.begin, b_el->flags.access_levels, SC_EVENT_REMOVE_OUTPUT_ARC, addr);

            if (need_unlock)
                sc_storage_element_unlock(ctx, el->arc.begin);

            // input arcs
            prev_arc = el->arc.prev_in_arc;
            next_arc = el->arc.next_in_arc;

            if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(prev_arc));
                sc_element *prev_el_arc = g_hash_table_lookup(lock_table, p_addr);
                g_assert(prev_el_arc != null_ptr);
                prev_el_arc->arc.next_in_arc = next_arc;
            }

            if (SC_ADDR_IS_NOT_EMPTY(next_arc))
            {
                p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(next_arc));
                sc_element *next_el_arc = g_hash_table_lookup(lock_table, p_addr);
                g_assert(next_el_arc != null_ptr);
                next_el_arc->arc.prev_in_arc = prev_arc;
            }

            need_unlock = SC_FALSE;
            sc_element *e_el = g_hash_table_lookup(lock_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.end)));
            if (e_el == null_ptr)
            {
                STORAGE_CHECK_CALL(sc_storage_element_lock(ctx, el->arc.end, &e_el));
                need_unlock = SC_TRUE;
            }
            if (SC_ADDR_IS_EQUAL(addr, e_el->first_in_arc))
                e_el->first_in_arc = next_arc;

            sc_event_emit(el->arc.end, e_el->flags.access_levels, SC_EVENT_REMOVE_INPUT_ARC, addr);

            if (need_unlock)
                sc_storage_element_unlock(ctx, el->arc.end);
        }

        if (sc_element_get_refs(sc_storage_get_element_meta(ctx, addr)) == 0)
        {
            sc_storage_erase_element_from_segment(addr);
            _sc_segment_cache_append(ctx, g_atomic_pointer_get(&segments[addr.seg]));
        }
        else
        {
            el->flags.type |= sc_flag_request_deletion;
        }

        sc_event_emit(addr, el_access, SC_EVENT_REMOVE_ELEMENT, addr);

        // remove registered events before deletion
        sc_event_notify_element_deleted(addr);
    }

    unlock:

    // now unlock elements
    g_hash_table_iter_init(&iter, lock_table);
    while (g_hash_table_iter_next(&iter, &key, &value) == TRUE)
    {
        sc_uint32 uint_addr = GPOINTER_TO_UINT(key);
        addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(uint_addr);
        addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(uint_addr);

        sc_storage_element_unlock(ctx, addr);
    }

    g_mutex_unlock(&s_mutex_free);

    g_slist_free(remove_list);
    g_hash_table_destroy(remove_table);
    g_hash_table_destroy(lock_table);

    return result;
}

sc_addr sc_storage_node_new(const sc_memory_context *ctx, sc_type type)
{
    return sc_storage_node_new_ext(ctx, type, ctx->access_levels);
}

sc_addr sc_storage_node_new_ext(const sc_memory_context *ctx, sc_type type, sc_access_levels access_levels)
{
    sc_element el;
    sc_addr addr;

    g_assert( !(sc_type_arc_mask & type) );
    memset(&el, 0, sizeof(el));

    el.flags.type = sc_flags_remove(sc_type_node | type);
    el.flags.access_levels = access_levels;

    sc_element *locked_el = sc_storage_append_el_into_segments(ctx, &el, &addr);
    if (locked_el == null_ptr)
    {
        SC_ADDR_MAKE_EMPTY(addr);
    }
    else
        STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, addr));
    return addr;
}

sc_addr sc_storage_link_new(const sc_memory_context *ctx)
{
    return sc_storage_link_new_ext(ctx, ctx->access_levels);
}

sc_addr sc_storage_link_new_ext(const sc_memory_context *ctx, sc_access_levels access_levels)
{
    sc_element el;
    sc_addr addr;

    memset(&el, 0, sizeof(el));
    el.flags.type = sc_type_link;
    el.flags.access_levels = access_levels;

    sc_element *locked_el = sc_storage_append_el_into_segments(ctx, &el, &addr);
    if (locked_el == null_ptr)
    {
        SC_ADDR_MAKE_EMPTY(addr);
    }
    else
        STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, addr));
    return addr;
}

sc_addr sc_storage_arc_new(const sc_memory_context *ctx, sc_type type, sc_addr beg, sc_addr end)
{
    return sc_storage_arc_new_ext(ctx, type, beg, end, ctx->access_levels);
}

sc_addr sc_storage_arc_new_ext(const sc_memory_context *ctx, sc_type type, sc_addr beg, sc_addr end, sc_access_levels access_levels)
{
    sc_addr addr;
    sc_element el;

    memset(&el, 0, sizeof(el));
    g_assert( !(sc_type_node & type) );
    el.flags.type = sc_flags_remove((type & sc_type_arc_mask) ? type : (sc_type_arc_common | type));

    el.arc.begin = beg;
    el.arc.end = end;
    el.flags.access_levels = access_levels;

    sc_result r;
    SC_ADDR_MAKE_EMPTY(addr);
    while (SC_ADDR_IS_EMPTY(addr))
    {
        sc_element *beg_el = 0, *end_el = 0;
        sc_element *f_out_arc = 0, *f_in_arc = 0;
        sc_element *tmp_el = 0;

        // try to lock begin and end elements
        r = sc_storage_element_lock_try(ctx, beg, s_max_storage_lock_attempts, &beg_el);
        if (beg_el == null_ptr)
            goto unlock;

        if (sc_element_is_valid(beg_el) == SC_FALSE)
        {
            r = SC_RESULT_ERROR_INVALID_STATE;
            goto unlock;
        }

        sc_access_levels beg_access = beg_el->flags.access_levels;

        r = sc_storage_element_lock_try(ctx, end, s_max_storage_lock_attempts, &end_el);
        if (end_el == null_ptr)
            goto unlock;

        if (sc_element_is_valid(end_el) == SC_FALSE)
        {
            r = SC_RESULT_ERROR_INVALID_STATE;
            goto unlock;
        }

        sc_access_levels end_access = end_el->flags.access_levels;

        // lock arcs to change output/input list
        sc_addr first_out_arc = beg_el->first_out_arc;
        if (SC_ADDR_IS_NOT_EMPTY(first_out_arc))
        {
            r = sc_storage_element_lock_try(ctx, first_out_arc, s_max_storage_lock_attempts, &f_out_arc);
            if (f_out_arc == null_ptr)
                goto unlock;
        }

        sc_addr first_in_arc = end_el->first_in_arc;
        if (SC_ADDR_IS_NOT_EMPTY(first_in_arc))
        {
            r = sc_storage_element_lock_try(ctx, first_in_arc, s_max_storage_lock_attempts, &f_in_arc);
            if (f_in_arc == null_ptr)
                goto unlock;
        }

        // get new element
        tmp_el = sc_storage_append_el_into_segments(ctx, &el, &addr);

        g_assert(tmp_el != 0);
        g_assert(SC_ADDR_IS_NOT_EQUAL(addr, first_in_arc));

        // emit events
        sc_event_emit(beg, beg_access, SC_EVENT_ADD_OUTPUT_ARC, addr);
        sc_event_emit(end, end_access, SC_EVENT_ADD_INPUT_ARC, addr);

        // check values
        g_assert(beg_el != null_ptr && end_el != null_ptr);
        g_assert(beg_el->flags.type != 0 && end_el->flags.type != 0);

        // set next output arc for our created arc
        tmp_el->arc.next_out_arc = first_out_arc;
        tmp_el->arc.next_in_arc = first_in_arc;

        g_assert(SC_ADDR_IS_NOT_EQUAL(addr, first_out_arc) && SC_ADDR_IS_NOT_EQUAL(addr, first_in_arc));
        if (f_out_arc)
            f_out_arc->arc.prev_out_arc = addr;

        if (f_in_arc)
            f_in_arc->arc.prev_in_arc = addr;

        // set our arc as first output/input at begin/end elements
        beg_el->first_out_arc = addr;
        end_el->first_in_arc = addr;

        unlock:
        {
            if (beg_el)
            {
                if (f_out_arc)
                    sc_storage_element_unlock(ctx, first_out_arc);
                sc_storage_element_unlock(ctx, beg);
            }
            if (end_el)
            {
                if (f_in_arc)
                    sc_storage_element_unlock(ctx, first_in_arc);
                sc_storage_element_unlock(ctx, end);
            }

            if (tmp_el)
                sc_storage_element_unlock(ctx, addr);
            if (r != SC_RESULT_OK)
                return addr;
        }

    }

    return addr;
}

sc_result sc_storage_get_element_type(const sc_memory_context *ctx, sc_addr addr, sc_type *result)
{
    sc_element *el = null_ptr;
    sc_result r = SC_RESULT_OK;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;
        
    if (sc_element_is_valid(el) == SC_FALSE)
    {
        r = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
        *result = sc_flags_remove(el->flags.type);
    else
        r = SC_RESULT_ERROR_NO_READ_RIGHTS;

    unlock:
    {
        sc_storage_element_unlock(ctx, addr);
    }
    return r;
}

sc_result sc_storage_change_element_subtype(const sc_memory_context *ctx, sc_addr addr, sc_type type)
{
    sc_element *el = null_ptr;
    sc_result r = SC_RESULT_OK;

    if (type & sc_type_element_mask)
        return SC_RESULT_ERROR_INVALID_PARAMS;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_element_is_valid(el) == SC_FALSE)
    {
        r = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    if (sc_access_lvl_check_write(ctx->access_levels, el->flags.access_levels))
        el->flags.type = (el->flags.type & sc_type_element_mask) | (type & ~sc_type_element_mask);
    else
        r = SC_RESULT_ERROR_NO_WRITE_RIGHTS;

    unlock:
    {
        sc_storage_element_unlock(ctx, addr);
    }
    return r;
}

sc_result sc_storage_get_arc_begin(const sc_memory_context *ctx, sc_addr addr, sc_addr *result)
{
    sc_element *el = null_ptr;
    sc_result res = SC_RESULT_ERROR_INVALID_TYPE;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;
        
    if (sc_element_is_valid(el) == SC_FALSE)
    {
        res = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
    {
        if (el->flags.type & sc_type_arc_mask)
        {
            *result = el->arc.begin;
            res = SC_RESULT_OK;
        }
    } else
        res = SC_RESULT_ERROR_NO_READ_RIGHTS;

    unlock:
    {
        sc_storage_element_unlock(ctx, addr);
    }
    return res;
}

sc_result sc_storage_get_arc_end(const sc_memory_context *ctx, sc_addr addr, sc_addr *result)
{
    sc_element *el = null_ptr;
    sc_result res = SC_RESULT_ERROR_INVALID_TYPE;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_element_is_valid(el) == SC_FALSE)
    {
        res = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
    {
        if (el->flags.type & sc_type_arc_mask)
        {
            *result = el->arc.end;
            res = SC_RESULT_OK;
        }
    } else
        res = SC_RESULT_ERROR_NO_READ_RIGHTS;

    unlock:
    {
        sc_storage_element_unlock(ctx, addr);
    }
    return res;
}

sc_result sc_storage_get_arc_info(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_begin_addr, sc_addr * result_end_addr)
{
    sc_element *el = null_ptr;
    sc_result res = SC_RESULT_ERROR_INVALID_TYPE;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_element_is_valid(el) == SC_FALSE)
    {
        res = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
    {
        if (el->flags.type & sc_type_arc_mask)
        {
            *result_begin_addr = el->arc.begin;
            *result_end_addr = el->arc.end;
            res = SC_RESULT_OK;
        }
    }
    else
        res = SC_RESULT_ERROR_NO_READ_RIGHTS;

unlock:
    {
        sc_storage_element_unlock(ctx, addr);
    }
    return res;
}

sc_result sc_storage_set_link_content(const sc_memory_context *ctx, sc_addr addr, const sc_stream *stream)
{
    sc_element *el;
    sc_check_sum check_sum;
    sc_result result = SC_RESULT_ERROR;
    sc_access_levels access_lvl;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_element_is_valid(el) == SC_FALSE)
    {
        result = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    g_assert(stream != null_ptr);

    access_lvl = el->flags.access_levels;
    if (!sc_access_lvl_check_write(ctx->access_levels, access_lvl))
    {
        result = SC_RESULT_ERROR_NO_WRITE_RIGHTS;
        goto unlock;
    }

    if (!(el->flags.type & sc_type_link))
    {
        result = SC_RESULT_ERROR_INVALID_TYPE;
        goto unlock;
    }

    if (sc_element_is_checksum_empty(el) == SC_FALSE)
    {
        sc_check_sum sum;
        if (el->flags.type & sc_flag_link_self_container)
            sc_link_self_container_calculate_checksum(el, &sum);
        else
        {
            sum.len = SC_CHECKSUM_LEN;
            memcpy(&sum.data[0], el->content.data, SC_CHECKSUM_LEN);
        }

        STORAGE_CHECK_CALL(sc_fs_storage_remove_content_addr(addr, &sum));
    }

    if (sc_link_calculate_checksum(stream, &check_sum) == SC_TRUE)
    {
        sc_uint32 len = 0;
        STORAGE_CHECK_CALL(sc_stream_get_length(stream, &len));
        if (len >= SC_CHECKSUM_LEN)
        {
            el->flags.type &= ~sc_flag_link_self_container;

            result = sc_fs_storage_write_content(addr, &check_sum, stream);
            memcpy(el->content.data, check_sum.data, check_sum.len);
        } else
        {
            G_STATIC_ASSERT(SC_CHECKSUM_LEN < 256);
            el->flags.type |= sc_flag_link_self_container;

            char buff[SC_CHECKSUM_LEN];
            sc_uint32 read = 0;
            STORAGE_CHECK_CALL(sc_stream_read_data(stream, &buff[0], len, &read));
            g_assert(read == len);

            el->content.data[0] = (sc_uint8)len;
            memcpy(&el->content.data[1], &buff[0], len);
            result = SC_RESULT_OK;

            sc_check_sum sum;
            STORAGE_CHECK_CALL(sc_link_calculate_checksum(stream, &sum));
            sc_fs_storage_add_content_addr(addr, &sum);
        }
    }
    g_assert(result == SC_RESULT_OK);

    sc_event_emit(addr, access_lvl, SC_EVENT_CONTENT_CHANGED, addr);

    unlock:
    {
        STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, addr));
    }

    return result;
}

sc_result sc_storage_get_link_content(const sc_memory_context *ctx, sc_addr addr, sc_stream **stream)
{
    sc_element *el = null_ptr;
    sc_result res = SC_RESULT_ERROR;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_element_is_valid(el) == SC_FALSE)
    {
        res = SC_RESULT_ERROR_INVALID_STATE;
        goto unlock;
    }

    if (!sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
    {
        res = SC_RESULT_ERROR_NO_READ_RIGHTS;
        goto unlock;
    }

    if (!(el->flags.type & sc_type_link))
    {
        res = SC_RESULT_ERROR_INVALID_TYPE;
        goto unlock;
    }

    if (el->flags.type & sc_flag_link_self_container)
    {
        sc_uint8 len = el->content.data[0];

        if (len != 0)
        {
            g_assert(len < SC_CHECKSUM_LEN);
            gchar *buff = g_new0(gchar, len);
            memcpy(buff, &el->content.data[1], len);
            *stream = sc_stream_memory_new(buff, len, SC_STREAM_FLAG_READ, SC_TRUE);

            res = SC_RESULT_OK;
        } else
        {
            *stream = 0;
            res = SC_RESULT_ERROR;
        }
    } else
    {
        // prepare checksum
        sc_check_sum checksum;
        checksum.len = SC_CHECKSUM_LEN;
        memcpy(checksum.data, el->content.data, checksum.len);

        res = sc_fs_storage_get_checksum_content(&checksum, stream);
    }

    unlock:
    {
        STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, addr));
    }

    return res;
}

sc_result sc_storage_find_links_with_content(const sc_memory_context *ctx, const sc_stream *stream, sc_addr **result, sc_uint32 *result_count)
{
    g_assert(stream != 0);
    sc_check_sum check_sum;

    sc_result r = SC_RESULT_ERROR;

    *result = 0;
    *result_count = 0;
    if (sc_link_calculate_checksum(stream, &check_sum) == SC_TRUE)
    {
        sc_addr * tmp_res = 0;
        sc_uint32 tmp_res_count = 0;

        r = sc_fs_storage_find_links_with_content(&check_sum, &tmp_res, &tmp_res_count);
        if (r == SC_RESULT_OK && tmp_res_count > 0)
        {
            // check read rights
            sc_uint32 i, passed = 0;
            *result = g_new0(sc_addr, tmp_res_count);
            for (i = 0; i < tmp_res_count; ++i)
            {
                sc_element *el = 0;

                if (sc_storage_element_lock(ctx, tmp_res[i], &el) != SC_RESULT_OK)
                {
                    g_free(*result);
                    *result = 0;
                    r = SC_RESULT_ERROR;
                    break;
                }

                if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
                {
                    (*result)[passed] = tmp_res[i];
                    ++passed;
                }

                STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, tmp_res[i]));
            }

            *result_count = passed;
            if (*result_count == 0 && *result)
            {
                g_free(*result);
                r = SC_RESULT_ERROR_NOT_FOUND;
            }
        }
    }

    return r;
}

sc_result sc_storage_set_access_levels(const sc_memory_context *ctx, sc_addr addr, sc_access_levels access_levels, sc_access_levels * new_value)
{
    sc_element *el = 0;
    sc_result r = SC_RESULT_OK;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;
        
    if (sc_access_lvl_check_write(ctx->access_levels, el->flags.access_levels))
    {
        el->flags.access_levels = sc_access_lvl_min(ctx->access_levels, access_levels);
        if (new_value)
            *new_value = el->flags.access_levels;
    }
    else
        r = SC_RESULT_ERROR_NO_WRITE_RIGHTS;

    STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, addr));

    return r;
}

sc_result sc_storage_get_access_levels(const sc_memory_context *ctx, sc_addr addr, sc_access_levels * result)
{
    sc_element *el = null_ptr;
    sc_result r = SC_RESULT_OK;

    if (sc_storage_element_lock(ctx, addr, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
    {
        *result = el->flags.access_levels;
    }
    else
        r = SC_RESULT_ERROR_NO_READ_RIGHTS;

    STORAGE_CHECK_CALL(sc_storage_element_unlock(ctx, addr));
    return r;
}


sc_result sc_storage_get_elements_stat(const sc_memory_context *ctx, sc_stat *stat)
{
    /// @todo implement function

    g_assert( stat != (sc_stat*)0 );

    memset(stat, 0, sizeof(sc_stat));
    stat->segments_count = sc_storage_get_segments_count();

    sc_int32 i;
    for (i = 0; i < g_atomic_int_get(&segments_num); ++i)
    {
        sc_segment *seg = segments[i];
        sc_segment_collect_elements_stat(ctx, seg, stat);
    }

    return SC_TRUE;
}

unsigned int sc_storage_get_segments_count()
{
    return g_atomic_int_get(&segments_num);
}

sc_result sc_storage_erase_element_from_segment(sc_addr addr)
{
    sc_segment_erase_element(g_atomic_pointer_get(&segments[addr.seg]), addr.offset);
    return SC_RESULT_OK;
}

// ------------------------------
sc_element_meta* sc_storage_get_element_meta(const sc_memory_context *ctx, sc_addr addr)
{
    g_assert(addr.seg < SC_ADDR_SEG_MAX);
    sc_segment *segment = g_atomic_pointer_get(&segments[addr.seg]);
    g_assert(segment != null_ptr);
    return sc_segment_get_meta(ctx, segment, addr.offset);
}

sc_result sc_storage_element_lock(const sc_memory_context *ctx, sc_addr addr, sc_element **el)
{
    if (addr.seg >= SC_ADDR_SEG_MAX)
    {
        *el = 0;
        return SC_RESULT_ERROR;
    }

    sc_segment *segment = g_atomic_pointer_get(&segments[addr.seg]);
    if (segment == 0)
    {
        *el = 0;
        return SC_RESULT_ERROR;
    }

    *el = sc_segment_lock_element(ctx, segment, addr.offset);
    return SC_RESULT_OK;
}

sc_result sc_storage_element_lock_try(const sc_memory_context *ctx, sc_addr addr, sc_uint16 max_attempts, sc_element **el)
{
    if (addr.seg >= SC_ADDR_SEG_MAX)
    {
        *el = 0;
        return SC_RESULT_ERROR;
    }

    sc_segment *segment = g_atomic_pointer_get(&segments[addr.seg]);
    if (segment == 0)
    {
        *el = 0;
        return SC_RESULT_ERROR;
    }

    *el = sc_segment_lock_element_try(ctx, segment, addr.offset, max_attempts);
    return SC_RESULT_OK;
}

sc_result sc_storage_element_unlock(const sc_memory_context *ctx, sc_addr addr)
{
    sc_segment * segment = null_ptr;

    if (addr.seg >= SC_ADDR_SEG_MAX)
        return SC_RESULT_ERROR;

    segment = g_atomic_pointer_get(&segments[addr.seg]);
    if (segment == 0)
        return SC_RESULT_ERROR;

    sc_segment_unlock_element(ctx, segment, addr.offset);
    return SC_RESULT_OK;
}

sc_result sc_storage_save(sc_memory_context const * ctx)
{
    sc_segment * seg;
    sc_uint32 i;

    // synchronize with free
    g_mutex_lock(&s_mutex_free);
    g_mutex_lock(&s_mutex_save);

    for (i = 0; i < SC_SEGMENT_MAX; ++i)
    {
        seg = segments[i];
        if (seg == null_ptr)
            continue;

        sc_segment_lock(seg, ctx);
    }

    sc_fs_storage_write_to_path(segments);

    g_mutex_unlock(&s_mutex_free);

    for (i = 0; i < SC_SEGMENT_MAX; ++i)
    {
        seg = segments[i];
        if (seg == null_ptr)
            continue;

        sc_segment_unlock(seg, ctx);
    }

    g_mutex_unlock(&s_mutex_save);

    return SC_RESULT_OK;
}
