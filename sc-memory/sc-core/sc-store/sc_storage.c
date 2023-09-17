/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_storage.h"

#include "sc_defines.h"
#include "sc_segment.h"
#include "sc_element.h"
#include "sc_event.h"
#include "sc_stream_memory.h"

#include "sc_event/sc_event_private.h"
#include "../sc_memory_private.h"
#include "sc-fs-memory/sc_fs_memory.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_atomic.h"
#include "sc-base/sc_assert_utils.h"
#include "sc-base/sc_message.h"
#include "sc-container/sc-string/sc_string.h"

#include <stdio.h>

// segments array
sc_segment ** segments = null_ptr;
// number of segments
sc_uint32 segments_num = 0;

const sc_uint16 s_max_storage_lock_attempts = 100;
const sc_uint16 s_max_storage_cache_attempts = 10;

sc_bool is_initialized = SC_FALSE;

sc_memory_context * segments_cache_lock_ctx = null_ptr;
sc_int32 segments_cache_count = 0;
sc_segment * segments_cache[SC_SEGMENT_CACHE_SIZE];  // cache of segments that have empty elements

GMutex s_mutex_free;
GMutex s_mutex_save;

#define CONCURRENCY_TO_CACHE_IDX(x) ((x) % SC_SEGMENT_CACHE_SIZE)

void _sc_segment_cache_lock(const sc_memory_context * ctx)
{
  while (sc_atomic_pointer_compare_and_exchange((void **)&segments_cache_lock_ctx, null_ptr, (void *)ctx) == SC_FALSE)
    ;
}

void _sc_segment_cache_unlock(const sc_memory_context * ctx)
{
  sc_assert(sc_atomic_pointer_get((void **)&segments_cache_lock_ctx) == ctx);
  sc_atomic_pointer_set((void **)&segments_cache_lock_ctx, null_ptr);
}

void _sc_segment_cache_append(sc_segment * seg)
{
  sc_uint cidx = GPOINTER_TO_UINT(sc_thread());
  sc_int32 i;
  sc_uint32 idx = CONCURRENCY_TO_CACHE_IDX(cidx);
  for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
  {
    if (sc_atomic_pointer_compare_and_exchange(
            (void **)&segments_cache[(idx + i) % SC_SEGMENT_CACHE_SIZE], null_ptr, seg) == SC_TRUE)
    {
      sc_atomic_int_inc(&segments_cache_count);
      break;
    }
  }
}

void _sc_segment_cache_remove(const sc_memory_context * ctx, sc_segment * seg)
{
  sc_int32 i, idx = CONCURRENCY_TO_CACHE_IDX(ctx->id);
  for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
  {
    if (sc_atomic_pointer_compare_and_exchange(
            (void **)&segments_cache[(idx + i) % SC_SEGMENT_CACHE_SIZE], (void *)seg, null_ptr) == SC_TRUE)
    {
      sc_atomic_int_add(&segments_cache_count, -1);
      break;
    }
  }
}

void _sc_segment_cache_clear()
{
  sc_int32 i;
  for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
    sc_atomic_pointer_set((void **)&segments_cache[i], null_ptr);
}

void _sc_segment_cache_update()
{
  // trying to push segments to cache
  sc_int32 i;
  for (i = 0; i < sc_atomic_int_get(&segments_num); ++i)
  {
    sc_segment * s = sc_atomic_pointer_get((void **)&(segments[i]));
    // need to check pointer, because segments_num increments earlier, then segments appends into array
    if (s != null_ptr)
    {
      if (sc_segment_has_empty_slot(s))
        _sc_segment_cache_append(s);
    }

    if (sc_atomic_int_get(&segments_cache_count) == SC_SEGMENT_CACHE_SIZE)
      break;
  }
}

sc_segment * _sc_segment_cache_get(const sc_memory_context * ctx)
{
  _sc_segment_cache_lock(ctx);

  g_mutex_lock(&s_mutex_save);

  sc_segment * seg = null_ptr;
  if (sc_atomic_int_get(&segments_cache_count) > 0)
  {
    sc_int32 i, idx = CONCURRENCY_TO_CACHE_IDX(ctx->id);
    for (i = 0; i < SC_SEGMENT_CACHE_SIZE; ++i)
    {
      seg = sc_atomic_pointer_get((void **)&segments_cache[(idx + i) % SC_SEGMENT_CACHE_SIZE]);
      if (seg != null_ptr)
        goto result;
    }
  }

  // try to update cache
  _sc_segment_cache_update();

  // if element still not added, then create new segment and append element into it
  sc_int32 seg_num = sc_atomic_int_add(&segments_num, 1);
  seg = sc_segment_new(seg_num);
  segments[seg_num] = seg;
  _sc_segment_cache_append(seg);

result:
{
  _sc_segment_cache_unlock(ctx);
  g_mutex_unlock(&s_mutex_save);
}

  return seg;
}

// -----------------------------------------------------------------------------

sc_bool sc_storage_initialize(sc_memory_params const * params)
{
  sc_bool result = sc_fs_memory_initialize_ext(params);
  if (result == SC_FALSE)
    return SC_FALSE;

  segments = sc_mem_new(sc_segment *, params->max_loaded_segments);
  memset(&*segments_cache, 0, sizeof(sc_segment *) * SC_SEGMENT_CACHE_SIZE);

  if (params->clear == SC_FALSE)
  {
    if (sc_fs_memory_load(segments, &segments_num) != SC_TRUE)
      return SC_FALSE;
  }
  else
  {
    segments_num = 0;
  }

  is_initialized = SC_TRUE;
  return SC_TRUE;
}

sc_bool sc_storage_shutdown(sc_bool save_state)
{
  if (save_state == SC_TRUE)
  {
    if (sc_fs_memory_save(segments, segments_num) == SC_FALSE)
      return SC_FALSE;
  }

  if (sc_fs_memory_shutdown() == SC_FALSE)
    return SC_FALSE;

  sc_uint idx;
  for (idx = 0; idx < segments_num; idx++)
  {
    if (segments[idx] == null_ptr)
      continue;  // skip segments, that are not loaded
    sc_segment_free(segments[idx]);
  }

  sc_mem_free(segments);
  segments = null_ptr;
  segments_num = 0;

  _sc_segment_cache_clear();
  is_initialized = SC_FALSE;
  return SC_TRUE;
}

sc_bool sc_storage_is_initialized()
{
  return is_initialized;
}

sc_bool sc_storage_is_element(const sc_memory_context * ctx, sc_addr addr)
{
  sc_element * el = null_ptr;
  sc_bool res = SC_TRUE;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return SC_FALSE;

  if (el->flags.type == 0 || (el->flags.type & sc_flag_request_deletion))
    res = SC_FALSE;

  sc_storage_element_unlock(addr);

  return res;
}

sc_uint32 sc_storage_get_element_output_arcs_count(const sc_memory_context * ctx, sc_addr addr)
{
  sc_element * el = null_ptr;
  sc_uint32 count = 0;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return count;

  count = sc_atomic_int_get(&el->output_arcs_count);

  sc_storage_element_unlock(addr);

  return count;
}

sc_uint32 sc_storage_get_element_input_arcs_count(const sc_memory_context * ctx, sc_addr addr)
{
  sc_element * el = null_ptr;
  sc_uint32 count = 0;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return count;

  count = sc_atomic_int_get(&el->input_arcs_count);

  sc_storage_element_unlock(addr);

  return count;
}

sc_element * sc_storage_append_el_into_segments(const sc_memory_context * ctx, sc_addr * addr)
{
  sc_segment * seg = (sc_segment *)0x1;

  sc_assert(addr != null_ptr);
  SC_ADDR_MAKE_EMPTY(*addr);

  /// @todo store segment with empty slots
  // try to find segment with empty slots
  while (seg != null_ptr)
  {
    sc_segment * seg = _sc_segment_cache_get(ctx);

    if (seg == null_ptr)
      break;

    sc_element * el = sc_segment_lock_empty_element(ctx, seg, &addr->offset);
    if (el != null_ptr)
    {
      addr->seg = seg->num;
      el->flags.access_levels = sc_access_lvl_min(ctx->access_levels, el->flags.access_levels);
      el->input_arcs_count = 0;
      el->output_arcs_count = 0;

      sc_element_meta * meta = sc_segment_get_meta(seg, addr->offset);
      sc_assert(meta != null_ptr);
      meta->ref_count = 1;
      return el;
    }
    else
    {
      _sc_segment_cache_remove(ctx, seg);
    }
  }

  return null_ptr;
}

sc_addr sc_storage_element_new_access(const sc_memory_context * ctx, sc_type type, sc_access_levels access_levels)
{
  sc_addr addr;
  sc_element * res = null_ptr;

  res = sc_storage_append_el_into_segments(ctx, &addr);
  if (res != null_ptr)
  {
    res->flags.type = type;
    res->flags.access_levels = access_levels;
    STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));
  }
  else
  {
    SC_ADDR_MAKE_EMPTY(addr);
  }

  return addr;
}

sc_result sc_storage_element_free(sc_memory_context * ctx, sc_addr addr)
{
  GHashTable * remove_table = null_ptr;
  GHashTable * lock_table = null_ptr;
  GSList * remove_list = null_ptr;
  sc_result result = SC_RESULT_OK;

  g_mutex_lock(&s_mutex_free);

  // the first we need to collect and lock all elements
  sc_element * el = null_ptr;
  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
  {
    g_mutex_unlock(&s_mutex_free);
    return SC_RESULT_ERROR;
  }

  sc_assert(el != null_ptr);
  if (el->flags.type == 0 || el->flags.type & sc_flag_request_deletion)
  {
    g_mutex_unlock(&s_mutex_free);
    sc_storage_element_unlock(addr);
    return SC_RESULT_ERROR;
  }

  remove_table = g_hash_table_new(g_direct_hash, g_direct_equal);
  lock_table = g_hash_table_new(g_direct_hash, g_direct_equal);
  g_hash_table_insert(remove_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)), el);
  g_hash_table_insert(lock_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)), el);

  remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)));
  while (remove_list != null_ptr)
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
      STORAGE_CHECK_CALL(sc_storage_element_lock(_addr, &el));

      sc_assert(el->flags.type != 0);
      g_hash_table_insert(remove_table, p_addr, el);
      g_hash_table_insert(lock_table, p_addr, el);
    }

    if (el->flags.type & sc_type_arc_mask)
    {
      // lock begin and end elements of arc
      sc_element * el2 = null_ptr;
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.begin));
      if ((el2 = g_hash_table_lookup(lock_table, p_addr)) == null_ptr)
      {
        STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.begin, &el2));
        g_hash_table_insert(lock_table, p_addr, el2);
      }

      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.end));
      if ((el2 = g_hash_table_lookup(lock_table, p_addr)) == null_ptr)
      {
        el2 = null_ptr;
        STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.end, &el2));
        g_hash_table_insert(lock_table, p_addr, el2);
      }

      // lock next/prev arcs in out/in lists
      if (SC_ADDR_IS_NOT_EMPTY(el->arc.prev_out_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.prev_out_arc));
        if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
        {
          el2 = null_ptr;
          STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.prev_out_arc, &el2));
          sc_assert(el2 != null_ptr);
          g_hash_table_insert(lock_table, p_addr, el2);
        }
      }

      if (SC_ADDR_IS_NOT_EMPTY(el->arc.prev_in_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.prev_in_arc));
        if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
        {
          el2 = null_ptr;
          STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.prev_in_arc, &el2));
          sc_assert(el2 != null_ptr);
          g_hash_table_insert(lock_table, p_addr, el2);
        }
      }

      if (SC_ADDR_IS_NOT_EMPTY(el->arc.next_out_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.next_out_arc));
        if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
        {
          el2 = null_ptr;
          STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.next_out_arc, &el2));
          sc_assert(el2 != null_ptr);
          g_hash_table_insert(lock_table, p_addr, el2);
        }
      }

      if (SC_ADDR_IS_NOT_EMPTY(el->arc.next_in_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.next_in_arc));
        if (g_hash_table_lookup(lock_table, p_addr) == null_ptr)
        {
          el2 = null_ptr;
          STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.next_in_arc, &el2));
          sc_assert(el2 != null_ptr);
          g_hash_table_insert(lock_table, p_addr, el2);
        }
      }
    }

    // Iterate all connectors for deleted element and append them into remove_list
    _addr = el->first_out_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      gpointer p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));
      sc_element * el2 = g_hash_table_lookup(remove_table, p_addr);

      if (el2 == null_ptr)
      {
        el2 = g_hash_table_lookup(lock_table, p_addr);
        if (el2 == null_ptr)
        {
          sc_storage_element_lock(_addr, &el2);
          g_hash_table_insert(lock_table, p_addr, el2);
        }

        sc_assert(el2 != null_ptr);
        g_hash_table_insert(remove_table, p_addr, el2);

        remove_list = g_slist_append(remove_list, p_addr);
      }

      _addr = el2->arc.next_out_arc;
    }

    _addr = el->first_in_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      gpointer p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));
      sc_element * el2 = g_hash_table_lookup(remove_table, p_addr);

      if (el2 == null_ptr)
      {
        el2 = g_hash_table_lookup(lock_table, p_addr);
        if (el2 == null_ptr)
        {
          sc_storage_element_lock(_addr, &el2);
          g_hash_table_insert(lock_table, p_addr, el2);
        }

        sc_assert(el2 != null_ptr);
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
      sc_fs_memory_unlink_string(SC_ADDR_LOCAL_TO_INT(addr));
    }
    else if (el->flags.type & sc_type_arc_mask)
    {
      // output arcs
      sc_addr prev_arc = el->arc.prev_out_arc;
      sc_addr next_arc = el->arc.next_out_arc;

      if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(prev_arc));
        sc_element * prev_el_arc = g_hash_table_lookup(lock_table, p_addr);
        sc_assert(prev_el_arc != null_ptr);
        prev_el_arc->arc.next_out_arc = next_arc;
      }

      if (SC_ADDR_IS_NOT_EMPTY(next_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(next_arc));
        sc_element * next_el_arc = g_hash_table_lookup(lock_table, p_addr);
        sc_assert(next_el_arc != null_ptr);
        next_el_arc->arc.prev_out_arc = prev_arc;
      }

      sc_element * b_el = g_hash_table_lookup(lock_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.begin)));
      sc_bool need_unlock = SC_FALSE;
      if (b_el == null_ptr)
      {
        STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.begin, &b_el));
        need_unlock = SC_TRUE;
      }
      if (SC_ADDR_IS_EQUAL(addr, b_el->first_out_arc))
        b_el->first_out_arc = next_arc;

      sc_atomic_int_add(&b_el->output_arcs_count, -1);
      sc_event_emit(ctx, el->arc.begin, b_el->flags.access_levels, SC_EVENT_REMOVE_OUTPUT_ARC, addr, el->arc.end);

      if (need_unlock)
        sc_storage_element_unlock(el->arc.begin);

      // input arcs
      prev_arc = el->arc.prev_in_arc;
      next_arc = el->arc.next_in_arc;

      if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(prev_arc));
        sc_element * prev_el_arc = g_hash_table_lookup(lock_table, p_addr);
        sc_assert(prev_el_arc != null_ptr);
        prev_el_arc->arc.next_in_arc = next_arc;
      }

      if (SC_ADDR_IS_NOT_EMPTY(next_arc))
      {
        p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(next_arc));
        sc_element * next_el_arc = g_hash_table_lookup(lock_table, p_addr);
        sc_assert(next_el_arc != null_ptr);
        next_el_arc->arc.prev_in_arc = prev_arc;
      }

      need_unlock = SC_FALSE;
      sc_element * e_el = g_hash_table_lookup(lock_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(el->arc.end)));
      if (e_el == null_ptr)
      {
        STORAGE_CHECK_CALL(sc_storage_element_lock(el->arc.end, &e_el));
        need_unlock = SC_TRUE;
      }
      if (SC_ADDR_IS_EQUAL(addr, e_el->first_in_arc))
        e_el->first_in_arc = next_arc;

      sc_atomic_int_add(&e_el->input_arcs_count, -1);
      sc_event_emit(ctx, el->arc.end, e_el->flags.access_levels, SC_EVENT_REMOVE_INPUT_ARC, addr, el->arc.begin);

      if (need_unlock)
        sc_storage_element_unlock(el->arc.end);
    }

    el->flags.type |= sc_flag_request_deletion;
    sc_storage_element_unref(addr);

    sc_addr empty;
    SC_ADDR_MAKE_EMPTY(empty);
    sc_event_emit(ctx, addr, el_access, SC_EVENT_REMOVE_ELEMENT, empty, empty);

    // remove registered events before deletion
    sc_event_notify_element_deleted(addr);
  }

unlock:

  // now unlock elements
  g_hash_table_iter_init(&iter, lock_table);
  while (g_hash_table_iter_next(&iter, &key, &value) == SC_TRUE)
  {
    sc_uint32 uint_addr = GPOINTER_TO_UINT(key);
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(uint_addr);
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(uint_addr);

    sc_storage_element_unlock(addr);
  }

  g_mutex_unlock(&s_mutex_free);

  g_slist_free(remove_list);
  g_hash_table_destroy(remove_table);
  g_hash_table_destroy(lock_table);

  return result;
}

sc_addr sc_storage_node_new(const sc_memory_context * ctx, sc_type type)
{
  return sc_storage_node_new_ext(ctx, type, ctx->access_levels);
}

sc_addr sc_storage_node_new_ext(const sc_memory_context * ctx, sc_type type, sc_access_levels access_levels)
{
  sc_assert(!(sc_type_arc_mask & type));

  sc_addr addr;
  sc_element * locked_el = sc_storage_append_el_into_segments(ctx, &addr);
  if (locked_el != null_ptr)
  {
    locked_el->flags.type = sc_flags_remove(sc_type_node | type);
    locked_el->flags.access_levels = access_levels;
    STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));
  }
  else
  {
    SC_ADDR_MAKE_EMPTY(addr);
  }

  return addr;
}

sc_addr sc_storage_link_new(const sc_memory_context * ctx, sc_bool is_const)
{
  return sc_storage_link_new_ext(ctx, ctx->access_levels, is_const);
}

sc_addr sc_storage_link_new_ext(const sc_memory_context * ctx, sc_access_levels access_levels, sc_bool is_const)
{
  sc_addr addr;

  sc_element * locked_el = sc_storage_append_el_into_segments(ctx, &addr);
  if (locked_el != null_ptr)
  {
    locked_el->flags.type = sc_type_link | (is_const ? sc_type_const : sc_type_var);
    locked_el->flags.access_levels = access_levels;
    STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));
  }
  else
  {
    SC_ADDR_MAKE_EMPTY(addr);
  }

  return addr;
}

sc_addr sc_storage_arc_new(sc_memory_context * ctx, sc_type type, sc_addr beg, sc_addr end)
{
  return sc_storage_arc_new_ext(ctx, type, beg, end, ctx->access_levels);
}

sc_addr sc_storage_arc_new_ext(
    sc_memory_context * ctx,
    sc_type type,
    sc_addr beg,
    sc_addr end,
    sc_access_levels access_levels)
{
  sc_addr addr;

  sc_assert(!(sc_type_node & type));

  sc_result r;
  SC_ADDR_MAKE_EMPTY(addr);

  if (SC_ADDR_IS_EMPTY(beg) || SC_ADDR_IS_EMPTY(end))
    return addr;

  while (SC_ADDR_IS_EMPTY(addr))
  {
    sc_element *beg_el = null_ptr, *end_el = null_ptr;
    sc_element *f_out_arc = null_ptr, *f_in_arc = null_ptr;
    sc_element * tmp_el = null_ptr;
    sc_bool should_break = SC_FALSE;

    // try to lock begin and end elements
    r = sc_storage_element_lock_try(beg, s_max_storage_lock_attempts, &beg_el);
    if (beg_el == null_ptr)
      goto unlock;

    if (sc_element_is_valid(beg_el) == SC_FALSE)
    {
      should_break = SC_TRUE;
      r = SC_RESULT_ERROR_INVALID_STATE;
      goto unlock;
    }

    sc_access_levels beg_access = beg_el->flags.access_levels;

    r = sc_storage_element_lock_try(end, s_max_storage_lock_attempts, &end_el);
    if (end_el == null_ptr)
      goto unlock;

    if (sc_element_is_valid(end_el) == SC_FALSE)
    {
      should_break = SC_TRUE;
      r = SC_RESULT_ERROR_INVALID_STATE;
      goto unlock;
    }

    sc_access_levels end_access = end_el->flags.access_levels;

    // lock arcs to change output/input list
    sc_addr first_out_arc = beg_el->first_out_arc;
    if (SC_ADDR_IS_NOT_EMPTY(first_out_arc))
    {
      r = sc_storage_element_lock_try(first_out_arc, s_max_storage_lock_attempts, &f_out_arc);
      if (f_out_arc == null_ptr)
        goto unlock;
    }

    sc_addr first_in_arc = end_el->first_in_arc;
    if (SC_ADDR_IS_NOT_EMPTY(first_in_arc))
    {
      r = sc_storage_element_lock_try(first_in_arc, s_max_storage_lock_attempts, &f_in_arc);
      if (f_in_arc == null_ptr)
        goto unlock;
    }

    // get new element
    tmp_el = sc_storage_append_el_into_segments(ctx, &addr);
    sc_atomic_int_inc(&beg_el->output_arcs_count);
    sc_atomic_int_inc(&end_el->input_arcs_count);

    tmp_el->flags.type = sc_flags_remove((type & sc_type_arc_mask) ? type : (sc_type_arc_common | type));
    tmp_el->arc.begin = beg;
    tmp_el->arc.end = end;
    tmp_el->flags.access_levels = access_levels;

    sc_assert(tmp_el != null_ptr);
    sc_assert(SC_ADDR_IS_NOT_EQUAL(addr, first_in_arc));

    // emit events
    sc_event_emit(ctx, beg, beg_access, SC_EVENT_ADD_OUTPUT_ARC, addr, end);
    sc_event_emit(ctx, end, end_access, SC_EVENT_ADD_INPUT_ARC, addr, beg);

    // check values
    sc_assert(beg_el != null_ptr && end_el != null_ptr);
    sc_assert(beg_el->flags.type != 0 && end_el->flags.type != 0);

    // set next output arc for our created arc
    tmp_el->arc.next_out_arc = first_out_arc;
    tmp_el->arc.next_in_arc = first_in_arc;

    sc_assert(SC_ADDR_IS_NOT_EQUAL(addr, first_out_arc) && SC_ADDR_IS_NOT_EQUAL(addr, first_in_arc));
    if (f_out_arc)
      f_out_arc->arc.prev_out_arc = addr;

    if (f_in_arc)
      f_in_arc->arc.prev_in_arc = addr;

    // set our arc as first output/input at begin/end elements
    beg_el->first_out_arc = addr;
    end_el->first_in_arc = addr;

  unlock:
  {
    if (beg_el != null_ptr)
    {
      if (f_out_arc != null_ptr)
        sc_storage_element_unlock(first_out_arc);
      sc_storage_element_unlock(beg);
    }
    if (end_el != null_ptr)
    {
      if (f_in_arc != null_ptr)
        sc_storage_element_unlock(first_in_arc);
      sc_storage_element_unlock(end);
    }

    if (tmp_el != null_ptr)
      sc_storage_element_unlock(addr);
  }

    if (should_break == SC_TRUE)
      break;
  }

  return addr;
}

sc_result sc_storage_get_element_type(const sc_memory_context * ctx, sc_addr addr, sc_type * result)
{
  sc_element * el = null_ptr;
  sc_result r = SC_RESULT_OK;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
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
  sc_storage_element_unlock(addr);
}
  return r;
}

sc_result sc_storage_change_element_subtype(const sc_memory_context * ctx, sc_addr addr, sc_type type)
{
  sc_element * el = null_ptr;
  sc_result r = SC_RESULT_OK;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (sc_element_is_valid(el) == SC_FALSE)
  {
    r = SC_RESULT_ERROR_INVALID_STATE;
    goto unlock;
  }

  if ((el->flags.type & sc_type_element_mask) != (type & sc_type_element_mask))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (sc_access_lvl_check_write(ctx->access_levels, el->flags.access_levels))
    el->flags.type = type;
  else
    r = SC_RESULT_ERROR_NO_WRITE_RIGHTS;

unlock:
{
  sc_storage_element_unlock(addr);
}
  return r;
}

sc_result sc_storage_get_arc_begin(const sc_memory_context * ctx, sc_addr addr, sc_addr * result)
{
  sc_element * el = null_ptr;
  sc_result res = SC_RESULT_ERROR_INVALID_TYPE;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
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
  }
  else
    res = SC_RESULT_ERROR_NO_READ_RIGHTS;

unlock:
{
  sc_storage_element_unlock(addr);
}
  return res;
}

sc_result sc_storage_get_arc_end(const sc_memory_context * ctx, sc_addr addr, sc_addr * result)
{
  sc_element * el = null_ptr;
  sc_result res = SC_RESULT_ERROR_INVALID_TYPE;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
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
  }
  else
    res = SC_RESULT_ERROR_NO_READ_RIGHTS;

unlock:
{
  sc_storage_element_unlock(addr);
}
  return res;
}

sc_result sc_storage_get_arc_info(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_addr * result_begin_addr,
    sc_addr * result_end_addr)
{
  sc_element * el = null_ptr;
  sc_result res = SC_RESULT_ERROR_INVALID_TYPE;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
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
  sc_storage_element_unlock(addr);
}
  return res;
}

sc_result sc_storage_set_link_content(sc_memory_context * ctx, sc_addr addr, const sc_stream * stream)
{
  sc_storage_set_link_content_ext(ctx, addr, stream, SC_TRUE);
}

sc_result sc_storage_set_link_content_ext(
    sc_memory_context * ctx,
    sc_addr addr,
    const sc_stream * stream,
    sc_bool is_searchable_string)
{
  sc_assert(ctx != null_ptr);
  sc_assert(stream != null_ptr);

  sc_element * el = null_ptr;
  sc_result result = SC_RESULT_ERROR;
  sc_access_levels access_lvl;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
  {
    result = SC_RESULT_ERROR;
    goto unlock;
  }

  if (sc_element_is_valid(el) == SC_FALSE)
  {
    result = SC_RESULT_ERROR_INVALID_STATE;
    goto unlock;
  }

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

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) == SC_FALSE)
  {
    result = SC_RESULT_ERROR_NO_READ_RIGHTS;
    goto unlock;
  }
  else
  {
    el->flags.type |= sc_flag_link_self_container;

    if (string == null_ptr)
      sc_string_empty(string);

    sc_fs_memory_link_string_ext(SC_ADDR_LOCAL_TO_INT(addr), string, string_size, is_searchable_string);
    result = SC_RESULT_OK;
  }
  sc_mem_free(string);

  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty);
  sc_event_emit(ctx, addr, access_lvl, SC_EVENT_CONTENT_CHANGED, empty, empty);

unlock:
{
  STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));
}

  return result;
}

sc_result sc_storage_get_link_content(const sc_memory_context * ctx, sc_addr addr, sc_stream ** stream)
{
  sc_assert(ctx != null_ptr);

  sc_element * el = null_ptr;
  sc_result result = SC_RESULT_ERROR;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (sc_element_is_valid(el) == SC_FALSE)
  {
    result = SC_RESULT_ERROR_INVALID_STATE;
    goto unlock;
  }

  if (!sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
  {
    result = SC_RESULT_ERROR_NO_READ_RIGHTS;
    goto unlock;
  }

  if (!(el->flags.type & sc_type_link))
  {
    result = SC_RESULT_ERROR_INVALID_TYPE;
    goto unlock;
  }

  if (el->flags.type & sc_flag_link_self_container)
  {
    sc_uint32 sc_string_size = 0;
    sc_char * sc_string = null_ptr;
    sc_fs_memory_get_string_by_link_hash(SC_ADDR_LOCAL_TO_INT(addr), &sc_string, &sc_string_size);

    if (sc_string == null_ptr)
      sc_string_empty(sc_string);

    *stream = sc_stream_memory_new(sc_string, sc_string_size, SC_STREAM_FLAG_READ, SC_TRUE);
    result = SC_RESULT_OK;
  }

unlock:
{
  STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));
}

  return result;
}

sc_result sc_storage_find_links_with_content_string(
    const sc_memory_context * ctx,
    const sc_stream * stream,
    sc_list ** result_hashes)
{
  sc_assert(ctx != null_ptr);
  sc_assert(stream != null_ptr);

  *result_hashes = null_ptr;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
    return SC_RESULT_ERROR;

  if (string == null_ptr)
  {
    string_size = 0;
    sc_string_empty(string);
  }

  sc_result result = sc_fs_memory_get_link_hashes_by_string(string, string_size, result_hashes);
  sc_mem_free(string);

  if (result != SC_RESULT_OK || result_hashes == null_ptr || *result_hashes == 0)
    return SC_RESULT_ERROR;

  return result;
}

sc_result sc_storage_find_links_by_content_substring(
    const sc_memory_context * ctx,
    const sc_stream * stream,
    sc_list ** result_hashes,
    sc_uint32 max_length_to_search_as_prefix)
{
  sc_assert(ctx != null_ptr);
  sc_assert(stream != null_ptr);

  *result_hashes = null_ptr;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
    return SC_RESULT_ERROR;

  if (string == null_ptr)
    sc_string_empty(string);

  sc_result result =
      sc_fs_memory_get_link_hashes_by_substring(string, string_size, max_length_to_search_as_prefix, result_hashes);
  sc_mem_free(string);

  if (result != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  return result;
}

sc_result sc_storage_find_links_contents_by_content_substring(
    const sc_memory_context * ctx,
    const sc_stream * stream,
    sc_list ** result_strings,
    sc_uint32 max_length_to_search_as_prefix)
{
  sc_assert(ctx != null_ptr);
  sc_assert(stream != null_ptr);

  *result_strings = null_ptr;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
    return SC_RESULT_ERROR;

  if (string == null_ptr)
    sc_string_empty(string);

  sc_result result =
      sc_fs_memory_get_strings_by_substring(string, string_size, max_length_to_search_as_prefix, result_strings);
  sc_mem_free(string);
  if (result != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  return result;
}

sc_result sc_storage_set_access_levels(
    const sc_memory_context * ctx,
    sc_addr addr,
    sc_access_levels access_levels,
    sc_access_levels * new_value)
{
  sc_element * el = null_ptr;
  sc_result r = SC_RESULT_OK;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (sc_access_lvl_check_write(ctx->access_levels, el->flags.access_levels))
  {
    el->flags.access_levels = sc_access_lvl_min(ctx->access_levels, access_levels);
    if (new_value)
      *new_value = el->flags.access_levels;
  }
  else
    r = SC_RESULT_ERROR_NO_WRITE_RIGHTS;

  STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));

  return r;
}

sc_result sc_storage_get_access_levels(const sc_memory_context * ctx, sc_addr addr, sc_access_levels * result)
{
  sc_element * el = null_ptr;
  sc_result r = SC_RESULT_OK;

  if (sc_storage_element_lock(addr, &el) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  if (sc_access_lvl_check_read(ctx->access_levels, el->flags.access_levels))
  {
    *result = el->flags.access_levels;
  }
  else
    r = SC_RESULT_ERROR_NO_READ_RIGHTS;

  STORAGE_CHECK_CALL(sc_storage_element_unlock(addr));
  return r;
}

sc_result sc_storage_get_elements_stat(sc_stat * stat)
{
  sc_assert(stat != null_ptr);

  sc_mem_set(stat, 0, sizeof(sc_stat));
  stat->segments_count = sc_storage_get_segments_count();

  sc_int32 i;
  for (i = 0; i < sc_atomic_int_get(&segments_num); ++i)
  {
    sc_segment * seg = segments[i];
    sc_segment_collect_elements_stat(seg, stat);
  }

  return SC_TRUE;
}

sc_uint32 sc_storage_get_segments_count()
{
  return sc_atomic_int_get(&segments_num);
}

sc_result sc_storage_erase_element_from_segment(sc_addr addr)
{
  sc_segment_erase_element(sc_atomic_pointer_get((void **)&segments[addr.seg]), addr.offset);
  return SC_RESULT_OK;
}

// ------------------------------
sc_element_meta * sc_storage_get_element_meta(sc_addr addr)
{
  sc_assert(addr.seg < SC_ADDR_SEG_MAX);
  sc_segment * segment = sc_atomic_pointer_get((void **)&segments[addr.seg]);
  sc_assert(segment != null_ptr);
  return sc_segment_get_meta(segment, addr.offset);
}

sc_result sc_storage_element_lock(sc_addr addr, sc_element ** el)
{
  if (addr.seg >= SC_ADDR_SEG_MAX)
  {
    *el = null_ptr;
    return SC_RESULT_ERROR;
  }

  sc_segment * segment = sc_atomic_pointer_get((void **)&segments[addr.seg]);
  if (segment == null_ptr)
  {
    *el = null_ptr;
    return SC_RESULT_ERROR;
  }

  *el = sc_segment_lock_element(segment, addr.offset);
  return SC_RESULT_OK;
}

sc_result sc_storage_element_lock_try(sc_addr addr, sc_uint16 max_attempts, sc_element ** el)
{
  if (addr.seg >= SC_ADDR_SEG_MAX)
  {
    *el = null_ptr;
    return SC_RESULT_ERROR;
  }

  sc_segment * segment = sc_atomic_pointer_get((void **)&segments[addr.seg]);
  if (segment == null_ptr)
  {
    *el = null_ptr;
    return SC_RESULT_ERROR;
  }

  *el = sc_segment_lock_element_try(segment, addr.offset, max_attempts);
  return SC_RESULT_OK;
}

sc_result sc_storage_element_unlock(sc_addr addr)
{
  sc_segment * segment = null_ptr;

  if (addr.seg >= SC_ADDR_SEG_MAX)
    return SC_RESULT_ERROR;

  segment = sc_atomic_pointer_get((void **)&segments[addr.seg]);
  if (segment == null_ptr)
    return SC_RESULT_ERROR;

  sc_segment_unlock_element(segment, addr.offset);
  return SC_RESULT_OK;
}

sc_result sc_storage_save(sc_memory_context const * ctx)
{
  sc_segment * seg = null_ptr;
  sc_uint32 i;

  // synchronize with free
  g_mutex_lock(&s_mutex_free);
  g_mutex_lock(&s_mutex_save);

  for (i = 0; i < segments_num; ++i)
  {
    seg = segments[i];
    if (seg == null_ptr)
      continue;
  }

  sc_fs_memory_save(segments, segments_num);

  g_mutex_unlock(&s_mutex_free);

  for (i = 0; i < segments_num; ++i)
  {
    seg = segments[i];
    if (seg == null_ptr)
      continue;
  }

  g_mutex_unlock(&s_mutex_save);

  return SC_RESULT_OK;
}

sc_bool _sc_storage_ref_common(sc_addr addr, int dir)
{
  sc_assert(dir == 1 || dir == -1);
  sc_assert(SC_ADDR_IS_NOT_EMPTY(addr));
  sc_element * el = null_ptr;
  sc_uint32 new_ref = 0;

  if (sc_storage_element_lock(addr, &el) == SC_RESULT_OK)
  {
    sc_assert(el != null_ptr);
    sc_element_meta * meta = sc_storage_get_element_meta(addr);

    sc_assert(meta != null_ptr);
    sc_assert(meta->ref_count > 0);
    sc_assert(meta->ref_count + dir < G_MAXUINT32);
    sc_assert(sizeof(meta->ref_count) == 4);

    meta->ref_count += dir;
    new_ref = meta->ref_count;

    sc_storage_element_unlock(addr);
  }
  else
  {
    sc_critical("Critical error, while add reference to an element (can't lock)");
  }

  return (new_ref == 0) ? SC_TRUE : SC_FALSE;
}

void sc_storage_element_ref(sc_addr addr)
{
}

sc_bool sc_storage_element_unref(sc_addr addr)
{
}
