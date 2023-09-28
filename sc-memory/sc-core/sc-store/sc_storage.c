/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_storage.h"

#include "sc_segment.h"
#include "sc_element.h"
#include "sc_stream_memory.h"

#include "sc_event/sc_event_private.h"
#include "../sc_memory_private.h"
#include "sc-fs-memory/sc_fs_memory.h"

#include "sc-base/sc_mutex.h"
#include "sc-base/sc_allocator.h"
#include "sc-base/sc_atomic.h"
#include "sc-container/sc-string/sc_string.h"

sc_storage * storage;

sc_bool sc_storage_initialize(sc_memory_params const * params)
{
  sc_bool result = sc_fs_memory_initialize_ext(params);
  if (result == SC_FALSE)
    return SC_FALSE;

  storage = sc_mem_new(sc_storage, 1);
  storage->max_segments_count = params->max_loaded_segments;
  storage->segments_count = 0;
  storage->segments = sc_mem_new(sc_segment *, params->max_loaded_segments);
  sc_list_init(&storage->empty_segments);
  sc_monitor_init(&storage->segments_monitor);
  _sc_monitor_global_init(&storage->addr_monitors_table);

  if (params->clear == SC_FALSE)
  {
    if (sc_fs_memory_load(storage->segments, &storage->segments_count) != SC_TRUE)
      return SC_FALSE;
  }

  return SC_TRUE;
}

sc_bool sc_storage_shutdown(sc_bool save_state)
{
  if (save_state == SC_TRUE)
  {
    if (sc_fs_memory_save(storage->segments, storage->segments_count) == SC_FALSE)
      return SC_FALSE;
  }

  if (sc_fs_memory_shutdown() == SC_FALSE)
    return SC_FALSE;

  for (sc_uint32 idx = 0; idx < storage->segments_count; idx++)
  {
    if (storage->segments[idx] == null_ptr)
      continue;  // skip segments, that are not loaded
    sc_segment_free(storage->segments[idx]);
  }

  sc_mem_free(storage->segments);
  sc_list_destroy(storage->empty_segments);
  sc_monitor_destroy(&storage->segments_monitor);
  _sc_monitor_global_destroy(&storage->addr_monitors_table);
  sc_mem_free(storage);
  storage = null_ptr;

  return SC_TRUE;
}

sc_bool sc_storage_is_initialized()
{
  return storage != null_ptr;
}

sc_storage * sc_storage_get()
{
  return storage;
}

sc_bool sc_storage_is_element(sc_memory_context const * ctx, sc_addr addr)
{
  sc_element * el = null_ptr;
  sc_result result = sc_storage_get_element_by_addr(addr, &el);

  return result == SC_RESULT_OK;
}

sc_result sc_storage_get_element_by_addr(sc_addr addr, sc_element ** el)
{
  *el = null_ptr;
  sc_result result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  if (SC_ADDR_IS_EMPTY(addr) || addr.seg >= storage->max_segments_count || addr.offset > SC_SEGMENT_ELEMENTS_COUNT)
    goto error;

  sc_segment * segment = storage->segments[addr.seg];
  if (segment == null_ptr)
    goto error;

  *el = sc_segment_get_element_by_offset(segment, addr.offset);
  if (((*el)->flags.access_levels & SC_ACCESS_LVL_ELEMENT_EXIST) == 0)
    goto error;

  result = SC_RESULT_OK;
error:
  return result;
}

sc_result sc_storage_remove_element_by_addr(sc_addr addr)
{
  sc_result result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_write(monitor);

  if (SC_ADDR_IS_EMPTY(addr) || addr.seg >= storage->max_segments_count || addr.offset > SC_SEGMENT_ELEMENTS_COUNT)
    goto error;

  sc_segment * segment = sc_atomic_pointer_get((void **)&storage->segments[addr.seg]);
  if (segment == null_ptr)
    goto error;

  segment->elements[addr.offset - 1] = (sc_element){};
  sc_monitor_release_write(monitor);

  sc_monitor_acquire_write(&storage->segments_monitor);
  if (segment->empty_element_offsets->size == 0)
    sc_list_push_back(storage->empty_segments, (void *)(sc_uint64)addr.seg);
  sc_list_push_back(segment->empty_element_offsets, (void *)(sc_uint64)addr.offset);
  sc_monitor_release_write(&storage->segments_monitor);

  result = SC_RESULT_OK;
  return result;
error:
  sc_monitor_release_write(monitor);
  return result;
}

sc_uint32 sc_storage_get_element_output_arcs_count(sc_memory_context const * ctx, sc_addr addr)
{
  sc_uint32 count = 0;
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  count = sc_atomic_int_get(&el->output_arcs_count);

error:
  return count;
}

sc_uint32 sc_storage_get_element_input_arcs_count(sc_memory_context const * ctx, sc_addr addr)
{
  sc_uint32 count = 0;
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  count = sc_atomic_int_get(&el->input_arcs_count);

error:
  return count;
}

sc_element * _sc_storage_get_old_empty_element(sc_addr * addr)
{
  sc_uint16 segment_num = (sc_uint16)(sc_uint64)sc_list_back(storage->empty_segments)->data;
  sc_segment * segment = storage->segments[segment_num];

  if (segment->empty_element_offsets->size == 1)
    sc_mem_free(sc_list_pop_back(storage->empty_segments));

  sc_struct_node * node = sc_list_pop_back(segment->empty_element_offsets);
  sc_uint64 element_offset = (sc_uint16)(sc_uint64)node->data;
  sc_mem_free(node);

  *addr = (sc_addr){segment_num, element_offset};
  sc_element * element = sc_segment_get_element_by_offset(segment, addr->offset);

  return element;
}

sc_element * _sc_storage_get_new_empty_element(sc_addr * addr)
{
  sc_element * element = null_ptr;

  sc_segment * segment = null_ptr;
  if (storage->segments_count == 0)
  {
    segment = storage->segments[storage->segments_count] = sc_segment_new(storage->segments_count);
    ++storage->segments_count;
    goto new_element;
  }

  if (storage->segments_count + 1 == storage->max_segments_count)
  {
    sc_memory_error(
        "Max segments count is %d. SC-memory is full. Please extends or swap sc-memory", storage->max_segments_count);
    goto error;
  }

  sc_uint32 last_segment_idx = storage->segments_count - 1;
  segment = storage->segments[last_segment_idx];
  if (segment->last_element_offset == SC_SEGMENT_ELEMENTS_COUNT)
  {
    segment = storage->segments[storage->segments_count] = sc_segment_new(storage->segments_count);
    ++storage->segments_count;
  }

new_element:
  element = &segment->elements[segment->last_element_offset];
  ++segment->last_element_offset;
  *addr = (sc_addr){segment->num, segment->last_element_offset};

error:
  return element;
}

sc_element * sc_storage_append_el_into_segments(sc_memory_context const * ctx, sc_addr * addr)
{
  *addr = SC_ADDR_EMPTY;
  sc_element * element = null_ptr;

  sc_monitor_acquire_write(&storage->segments_monitor);

  if (storage->empty_segments->size > 0)
    element = _sc_storage_get_old_empty_element(addr);
  else
    element = _sc_storage_get_new_empty_element(addr);

  if (element != null_ptr)
    element->flags.access_levels |= SC_ACCESS_LVL_ELEMENT_EXIST;

  sc_monitor_release_write(&storage->segments_monitor);

  return element;
}

sc_result sc_storage_element_free(sc_memory_context const * ctx, sc_addr addr)
{
  GHashTable * remove_table = null_ptr;
  GSList * remove_list = null_ptr;

  sc_result result;

  // the first we need to collect and lock all elements
  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  remove_table = g_hash_table_new(g_direct_hash, g_direct_equal);
  g_hash_table_insert(remove_table, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)), el);

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

    result = sc_storage_get_element_by_addr(_addr, &el);
    if (result != SC_RESULT_OK)
      goto error;

    g_hash_table_insert(remove_table, p_addr, el);

    // Iterate all connectors for deleted element and append them into remove_list
    _addr = el->first_out_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));
      sc_element * el2 = g_hash_table_lookup(remove_table, p_addr);

      if (el2 == null_ptr)
      {
        result = sc_storage_get_element_by_addr(_addr, &el2);
        if (result != SC_RESULT_OK)
          goto error;

        g_hash_table_insert(remove_table, p_addr, el2);

        remove_list = g_slist_append(remove_list, p_addr);
      }

      _addr = el2->arc.next_out_arc;
    }

    _addr = el->first_in_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      p_addr = GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr));
      sc_element * el2 = g_hash_table_lookup(remove_table, p_addr);

      if (el2 == null_ptr)
      {
        result = sc_storage_get_element_by_addr(_addr, &el2);
        if (result != SC_RESULT_OK)
          goto error;

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
  while (g_hash_table_iter_next(&iter, &key, &value) == SC_TRUE)
  {
    el = value;
    sc_uint32 uint_addr = GPOINTER_TO_UINT(key);
    gpointer p_addr;
    addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(uint_addr);
    addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(uint_addr);

    if ((el->flags.access_levels & SC_ACCESS_LVL_ELEMENT_EXIST) != SC_ACCESS_LVL_ELEMENT_EXIST)
      continue;

    if (el->flags.type & sc_type_link)
      sc_fs_memory_unlink_string(SC_ADDR_LOCAL_TO_INT(addr));
    else if (el->flags.type & sc_type_arc_mask)
    {
      sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
      sc_monitor * beg_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, el->arc.begin);
      sc_monitor * end_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, el->arc.end);
      sc_monitor_acquire_write_n(3, monitor, beg_monitor, end_monitor);

      // output arcs
      sc_addr prev_arc = el->arc.prev_out_arc;
      sc_addr next_arc = el->arc.next_out_arc;

      if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
      {
        sc_element * prev_el_arc;
        result = sc_storage_get_element_by_addr(prev_arc, &prev_el_arc);
        if (result == SC_RESULT_OK)
          prev_el_arc->arc.next_out_arc = next_arc;
      }

      if (SC_ADDR_IS_NOT_EMPTY(next_arc))
      {
        sc_element * next_el_arc;
        result = sc_storage_get_element_by_addr(next_arc, &next_el_arc);
        if (result == SC_RESULT_OK)
          next_el_arc->arc.prev_out_arc = prev_arc;
      }

      sc_element * b_el;
      result = sc_storage_get_element_by_addr(el->arc.begin, &b_el);
      if (result == SC_RESULT_OK && SC_ADDR_IS_EQUAL(addr, b_el->first_out_arc))
        b_el->first_out_arc = next_arc;

      --b_el->output_arcs_count;

      sc_event_emit(ctx, el->arc.begin, b_el->flags.access_levels, SC_EVENT_REMOVE_OUTPUT_ARC, addr, el->arc.end);

      // input arcs
      prev_arc = el->arc.prev_in_arc;
      next_arc = el->arc.next_in_arc;

      if (SC_ADDR_IS_NOT_EMPTY(prev_arc))
      {
        sc_element * prev_el_arc;
        result = sc_storage_get_element_by_addr(prev_arc, &prev_el_arc);
        if (result == SC_RESULT_OK)
          prev_el_arc->arc.next_in_arc = next_arc;
      }

      if (SC_ADDR_IS_NOT_EMPTY(next_arc))
      {
        sc_element * next_el_arc;
        result = sc_storage_get_element_by_addr(next_arc, &next_el_arc);
        if (result == SC_RESULT_OK)
          next_el_arc->arc.prev_in_arc = prev_arc;
      }

      sc_element * e_el;
      result = sc_storage_get_element_by_addr(el->arc.end, &e_el);
      if (result == SC_RESULT_OK && SC_ADDR_IS_EQUAL(addr, e_el->first_in_arc))
        e_el->first_in_arc = next_arc;

      --e_el->input_arcs_count;

      sc_event_emit(ctx, el->arc.end, e_el->flags.access_levels, SC_EVENT_REMOVE_INPUT_ARC, addr, el->arc.begin);

      sc_monitor_release_write_n(3, monitor, beg_monitor, end_monitor);
    }

    sc_event_emit(ctx, addr, el->flags.access_levels, SC_EVENT_REMOVE_ELEMENT, SC_ADDR_EMPTY, SC_ADDR_EMPTY);

    // remove registered events before deletion
    sc_event_notify_element_deleted(addr);

    sc_storage_remove_element_by_addr(addr);
  }

  result = SC_RESULT_OK;
error:
  if (remove_list != null_ptr)
    g_slist_free(remove_list);
  if (remove_table != null_ptr)
    g_hash_table_destroy(remove_table);
  return result;
}

sc_addr sc_storage_node_new(sc_memory_context const * ctx, sc_type type)
{
  return sc_storage_node_new_ext(ctx, type, ctx->access_levels);
}

sc_addr sc_storage_node_new_ext(const sc_memory_context * ctx, sc_type type, sc_access_levels access_levels)
{
  sc_addr addr = SC_ADDR_EMPTY;

  sc_element * element = sc_storage_append_el_into_segments(ctx, &addr);
  if (element != null_ptr)
  {
    element->flags.type = sc_type_node | type;
    element->flags.access_levels |= access_levels;
  }

  return addr;
}

sc_addr sc_storage_link_new(sc_memory_context const * ctx, sc_bool is_const)
{
  return sc_storage_link_new_ext(ctx, ctx->access_levels, is_const);
}

sc_addr sc_storage_link_new_ext(sc_memory_context const * ctx, sc_access_levels access_levels, sc_bool is_const)
{
  sc_addr addr = SC_ADDR_EMPTY;

  sc_element * element = sc_storage_append_el_into_segments(ctx, &addr);
  if (element != null_ptr)
  {
    element->flags.type = sc_type_link | (is_const ? sc_type_const : sc_type_var);
    element->flags.access_levels |= access_levels;
  }

  return addr;
}

sc_addr sc_storage_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end)
{
  return sc_storage_arc_new_ext(ctx, type, beg, end, ctx->access_levels);
}

sc_addr sc_storage_arc_new_ext(
    sc_memory_context const * ctx,
    sc_type type,
    sc_addr beg,
    sc_addr end,
    sc_access_levels access_levels)
{
  sc_addr addr = SC_ADDR_EMPTY;
  sc_result result;

  if (SC_ADDR_IS_EMPTY(beg) || SC_ADDR_IS_EMPTY(end))
    return addr;

  sc_element *beg_el = null_ptr, *end_el = null_ptr;
  sc_element *f_out_arc = null_ptr, *f_in_arc = null_ptr;
  sc_element * tmp_el = null_ptr;

  sc_monitor * beg_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, beg);
  sc_monitor * end_monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, end);
  sc_monitor_acquire_write_n(2, beg_monitor, end_monitor);

  // try to lock begin and end elements
  result = sc_storage_get_element_by_addr(beg, &beg_el);
  if (result != SC_RESULT_OK)
    goto error;

  result = sc_storage_get_element_by_addr(end, &end_el);
  if (result != SC_RESULT_OK)
    goto error;

  // lock arcs to change output/input list
  sc_addr first_out_arc = beg_el->first_out_arc;
  if (SC_ADDR_IS_NOT_EMPTY(first_out_arc))
    sc_storage_get_element_by_addr(first_out_arc, &f_out_arc);

  sc_addr first_in_arc = end_el->first_in_arc;
  if (SC_ADDR_IS_NOT_EMPTY(first_in_arc))
    sc_storage_get_element_by_addr(first_in_arc, &f_in_arc);

  // get new element
  tmp_el = sc_storage_append_el_into_segments(ctx, &addr);

  tmp_el->flags.type = (type & sc_type_arc_mask) ? type : (sc_type_arc_common | type);
  tmp_el->flags.access_levels |= access_levels;
  tmp_el->arc.begin = beg;
  tmp_el->arc.end = end;

  // emit events
  sc_event_emit(ctx, beg, beg_el->flags.access_levels, SC_EVENT_ADD_OUTPUT_ARC, addr, end);
  sc_event_emit(ctx, end, beg_el->flags.access_levels, SC_EVENT_ADD_INPUT_ARC, addr, beg);

  // set next output arc for our created arc
  tmp_el->arc.next_out_arc = first_out_arc;
  tmp_el->arc.next_in_arc = first_in_arc;

  if (f_out_arc)
    f_out_arc->arc.prev_out_arc = addr;

  if (f_in_arc)
    f_in_arc->arc.prev_in_arc = addr;

  // set our arc as first output/input at begin/end elements
  beg_el->first_out_arc = addr;
  end_el->first_in_arc = addr;

  ++beg_el->output_arcs_count;
  ++end_el->input_arcs_count;

error:
  sc_monitor_release_write_n(2, beg_monitor, end_monitor);
  return addr;
}

sc_result sc_storage_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * type)
{
  sc_result result;

  sc_element * el = null_ptr;

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  *type = el->flags.type;

error:
  return result;
}

sc_result sc_storage_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_write(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_element_mask) != (type & sc_type_element_mask))
  {
    result = SC_RESULT_ERROR_INVALID_PARAMS;
    goto error;
  }

  el->flags.type = type;

error:
  sc_monitor_release_write(monitor);
  return result;
}

sc_result sc_storage_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_begin_addr)
{
  *result_begin_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_arc_mask) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    goto error;
  }

  *result_begin_addr = el->arc.begin;

error:
  sc_monitor_release_read(monitor);
  return result;
}

sc_result sc_storage_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_end_addr)
{
  *result_end_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_arc_mask) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    goto error;
  }

  *result_end_addr = el->arc.end;

error:
  sc_monitor_release_read(monitor);
  return result;
}

sc_result sc_storage_get_arc_info(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_addr * result_begin_addr,
    sc_addr * result_end_addr)
{
  *result_begin_addr = SC_ADDR_EMPTY;
  *result_end_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;

  sc_monitor * monitor = sc_monitor_get_monitor_for_addr(&storage->addr_monitors_table, addr);
  sc_monitor_acquire_read(monitor);

  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_arc_mask) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_CONNECTOR;
    goto error;
  }

  *result_begin_addr = el->arc.begin;
  *result_end_addr = el->arc.end;

error:
  sc_monitor_release_read(monitor);
  return result;
}

sc_result sc_storage_set_link_content(sc_memory_context const * ctx, sc_addr addr, const sc_stream * stream)
{
  return sc_storage_set_link_content_ext(ctx, addr, stream, SC_TRUE);
}

sc_result sc_storage_set_link_content_ext(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string)
{
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if ((el->flags.type & sc_type_link) == 0)
  {
    result = SC_RESULT_ERROR_ELEMENT_IS_NOT_LINK;
    goto error;
  }

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) == SC_FALSE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  sc_fs_memory_link_string_ext(SC_ADDR_LOCAL_TO_INT(addr), string, string_size, is_searchable_string);
  sc_mem_free(string);

  sc_addr empty;
  SC_ADDR_MAKE_EMPTY(empty);
  sc_event_emit(ctx, addr, el->flags.access_levels, SC_EVENT_CONTENT_CHANGED, empty, empty);

error:
  return result;
}

sc_result sc_storage_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream)
{
  *stream = null_ptr;
  sc_result result;

  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  if (!(el->flags.type & sc_type_link))
  {
    result = SC_RESULT_ERROR_INVALID_TYPE;
    goto error;
  }

  sc_uint32 string_size = 0;
  sc_char * string = null_ptr;
  sc_fs_memory_get_string_by_link_hash(SC_ADDR_LOCAL_TO_INT(addr), &string, &string_size);

  if (string == null_ptr)
    sc_string_empty(string);

  *stream = sc_stream_memory_new(string, string_size, SC_STREAM_FLAG_READ, SC_TRUE);

  return SC_RESULT_OK;

error:
  return result;
}

sc_result sc_storage_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes)
{
  *result_hashes = null_ptr;
  sc_result result;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
  {
    string_size = 0;
    sc_string_empty(string);
  }

  result = sc_fs_memory_get_link_hashes_by_string(string, string_size, result_hashes);
  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result_hashes,
    sc_uint32 max_length_to_search_as_prefix)
{
  *result_hashes = null_ptr;
  sc_result result;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  result =
      sc_fs_memory_get_link_hashes_by_substring(string, string_size, max_length_to_search_as_prefix, result_hashes);
  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_find_links_contents_by_content_substring(
    const sc_memory_context * ctx,
    const sc_stream * stream,
    sc_list ** result_strings,
    sc_uint32 max_length_to_search_as_prefix)
{
  *result_strings = null_ptr;
  sc_result result;

  sc_char * string = null_ptr;
  sc_uint32 string_size = 0;
  if (sc_stream_get_data(stream, &string, &string_size) != SC_TRUE)
  {
    result = SC_RESULT_ERROR_IO;
    goto error;
  }

  if (string == null_ptr)
    sc_string_empty(string);

  result = sc_fs_memory_get_strings_by_substring(string, string_size, max_length_to_search_as_prefix, result_strings);
  sc_mem_free(string);

error:
  return result;
}

sc_result sc_storage_get_elements_stat(sc_stat * stat)
{
  sc_mem_set(stat, 0, sizeof(sc_stat));

  sc_int32 i;
  for (i = 0; i < sc_atomic_int_get(&storage->segments_count); ++i)
  {
    sc_segment * seg = storage->segments[i];
    sc_segment_collect_elements_stat(seg, stat);
  }

  return SC_TRUE;
}

sc_result sc_storage_save(sc_memory_context const * ctx)
{
  sc_segment * seg = null_ptr;
  sc_uint32 i;

  for (i = 0; i < sc_atomic_int_get(&storage->segments_count); ++i)
  {
    seg = storage->segments[i];
    if (seg == null_ptr)
      continue;
  }

  sc_fs_memory_save(storage->segments, storage->segments_count);

  for (i = 0; i < sc_atomic_int_get(&storage->segments_count); ++i)
  {
    seg = storage->segments[i];
    if (seg == null_ptr)
      continue;
  }

  return SC_RESULT_OK;
}
