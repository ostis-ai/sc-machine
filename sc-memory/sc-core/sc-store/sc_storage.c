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
  sc_mem_free(storage);
  storage = null_ptr;

  return SC_TRUE;
}

sc_bool sc_storage_is_initialized()
{
  return storage != null_ptr;
}

sc_bool sc_storage_is_element(sc_memory_context const * ctx, sc_addr addr)
{
  sc_element * el = null_ptr;
  sc_result result = sc_storage_get_element_by_addr(addr, &el);

  return result == SC_RESULT_OK;
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

sc_element * sc_storage_append_el_into_segments(sc_memory_context const * ctx, sc_addr * addr)
{
  *addr = SC_ADDR_EMPTY;
  sc_element * element = null_ptr;

  sc_uint32 last_segment_idx = storage->segments_count - 1;
  sc_bool is_empty_storage = storage->segments_count == 0;
  sc_segment * segment = is_empty_storage ? null_ptr : storage->segments[last_segment_idx];
  if (is_empty_storage || segment->elements_count == SC_SEGMENT_ELEMENTS_COUNT)
  {
    if (storage->segments_count == storage->max_segments_count)
    {
      sc_memory_error(
          "Max segments count is %d. SC-memory is full. Please extends or swap sc-memory",
          storage->max_segments_count);
      goto error;
    }

    segment = storage->segments[storage->segments_count] = sc_segment_new(storage->segments_count);
    ++storage->segments_count;
  }

  element = &segment->elements[segment->elements_count];
  element->flags.access_levels |= SC_ACCESS_LVL_ELEMENT_EXIST;
  sc_atomic_int_add(&segment->elements_count, 1);
  *addr = (sc_addr){segment->num, sc_atomic_int_get(&segment->elements_count)};

error:
  return element;
}

sc_result sc_storage_element_free(sc_memory_context const * ctx, sc_addr addr)
{
  sc_result result;

  // the first we need to collect and lock all elements
  sc_element * el = null_ptr;
  result = sc_storage_get_element_by_addr(addr, &el);
  if (result != SC_RESULT_OK)
    goto error;

  // Iterate all connectors for deleted element and append them into remove_list
  sc_addr connector_addr = el->first_out_arc;
  while (SC_ADDR_IS_NOT_EMPTY(connector_addr))
  {
    sc_element * connector;
    result = sc_storage_get_element_by_addr(connector_addr, &connector);
    if (result != SC_RESULT_OK)
      goto error;

    sc_event_emit(
        ctx,
        connector->arc.begin,
        connector->flags.access_levels,
        SC_EVENT_REMOVE_OUTPUT_ARC,
        SC_ADDR_EMPTY,
        connector->arc.begin);

    sc_element * target;
    result = sc_storage_get_element_by_addr(connector->arc.end, &target);
    if (result != SC_RESULT_OK)
      goto error;

    sc_atomic_int_add(&target->input_arcs_count, -1);

    sc_event_emit(
        ctx, connector_addr, connector->flags.access_levels, SC_EVENT_REMOVE_ELEMENT, SC_ADDR_EMPTY, SC_ADDR_EMPTY);

    // remove registered events before deletion
    sc_event_notify_element_deleted(connector_addr);

    sc_storage_remove_element_by_addr(connector_addr);

    connector_addr = connector->arc.next_out_arc;
  }

  connector_addr = el->first_in_arc;
  while (SC_ADDR_IS_NOT_EMPTY(connector_addr))
  {
    sc_element * connector;
    result = sc_storage_get_element_by_addr(connector_addr, &connector);
    if (result != SC_RESULT_OK)
      goto error;

    sc_event_emit(
        ctx,
        connector->arc.end,
        connector->flags.access_levels,
        SC_EVENT_REMOVE_INPUT_ARC,
        SC_ADDR_EMPTY,
        connector->arc.begin);

    sc_element * source;
    result = sc_storage_get_element_by_addr(connector->arc.begin, &source);
    if (result != SC_RESULT_OK)
      goto error;

    sc_atomic_int_add(&source->output_arcs_count, -1);

    sc_event_emit(
        ctx, connector_addr, connector->flags.access_levels, SC_EVENT_REMOVE_ELEMENT, SC_ADDR_EMPTY, SC_ADDR_EMPTY);

    // remove registered events before deletion
    sc_event_notify_element_deleted(connector_addr);

    sc_storage_remove_element_by_addr(connector_addr);

    connector_addr = connector->arc.next_in_arc;
  }

  if (el->flags.type & sc_type_link)
    sc_fs_memory_unlink_string(SC_ADDR_LOCAL_TO_INT(addr));
  else if (el->flags.type & sc_type_arc_access) {
    sc_event_emit(
        ctx,
        el->arc.begin,
        el->flags.access_levels,
        SC_EVENT_REMOVE_OUTPUT_ARC,
        SC_ADDR_EMPTY,
        el->arc.begin);

    sc_element * target;
    result = sc_storage_get_element_by_addr(el->arc.end, &target);
    if (result != SC_RESULT_OK)
      goto error;

    sc_atomic_int_add(&target->input_arcs_count, -1);

    sc_event_emit(
        ctx,
        el->arc.end,
        el->flags.access_levels,
        SC_EVENT_REMOVE_INPUT_ARC,
        SC_ADDR_EMPTY,
        el->arc.begin);

    sc_element * source;
    result = sc_storage_get_element_by_addr(el->arc.begin, &source);
    if (result != SC_RESULT_OK)
      goto error;

    sc_atomic_int_add(&source->output_arcs_count, -1);
  }

  sc_event_emit(
      ctx, addr, el->flags.access_levels, SC_EVENT_REMOVE_ELEMENT, SC_ADDR_EMPTY, SC_ADDR_EMPTY);

  // remove registered events before deletion
  sc_event_notify_element_deleted(connector_addr);

  sc_storage_remove_element_by_addr(addr);

error:
  return result;
}

sc_addr sc_storage_node_new(const sc_memory_context * ctx, sc_type type)
{
  return sc_storage_node_new_ext(ctx, type, ctx->access_levels);
}

sc_addr sc_storage_node_new_ext(const sc_memory_context * ctx, sc_type type, sc_access_levels access_levels)
{
  sc_addr addr = SC_ADDR_EMPTY;

  sc_element * locked_el = sc_storage_append_el_into_segments(ctx, &addr);
  if (locked_el != null_ptr)
  {
    locked_el->flags.type = sc_type_node | type;
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

  sc_element * locked_el = sc_storage_append_el_into_segments(ctx, &addr);
  if (locked_el != null_ptr)
  {
    locked_el->flags.type = sc_type_link | (is_const ? sc_type_const : sc_type_var);
  }

  return addr;
}

sc_addr sc_storage_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end)
{
  return sc_storage_arc_new_ext(ctx, type, beg, end, ctx->access_levels);
}


static pthread_mutex_t arc_mutex = PTHREAD_MUTEX_INITIALIZER;

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

  sc_element * beg_el = null_ptr, * end_el = null_ptr;
  sc_element * f_out_arc = null_ptr, * f_in_arc = null_ptr;
  sc_element * tmp_el = null_ptr;

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
  sc_atomic_int_inc(&beg_el->output_arcs_count);
  sc_atomic_int_inc(&end_el->input_arcs_count);

  tmp_el->flags.type = (type & sc_type_arc_mask) ? type : (sc_type_arc_common | type);
  tmp_el->arc.begin = beg;
  tmp_el->arc.end = end;

  // emit events
  sc_event_emit(ctx, beg, 0, SC_EVENT_ADD_OUTPUT_ARC, addr, end);
  sc_event_emit(ctx, end, 0, SC_EVENT_ADD_INPUT_ARC, addr, beg);

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

error:
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
  return result;
}

sc_result sc_storage_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_begin_addr)
{
  *result_begin_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;
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
  return result;
}

sc_result sc_storage_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result_end_addr)
{
  *result_end_addr = SC_ADDR_EMPTY;
  sc_result result;

  sc_element * el = null_ptr;
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
  sc_event_emit(ctx, addr, 0, SC_EVENT_CONTENT_CHANGED, empty, empty);

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
    const sc_memory_context * ctx,
    const sc_stream * stream,
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

  result =
      sc_fs_memory_get_strings_by_substring(string, string_size, max_length_to_search_as_prefix, result_strings);
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

sc_result sc_storage_get_element_by_addr(sc_addr addr, sc_element ** el)
{
  sc_result result = SC_RESULT_OK;

  if (SC_ADDR_IS_EMPTY(addr) || addr.seg >= storage->max_segments_count || addr.offset > SC_SEGMENT_ELEMENTS_COUNT)
  {
    *el = null_ptr;
    result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
  }

  sc_segment * segment = sc_atomic_pointer_get((void **)&storage->segments[addr.seg]);
  if (segment == null_ptr)
  {
    *el = null_ptr;
    result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
  }

  *el = sc_segment_get_element_by_offset(segment, addr.offset);
  if (((*el)->flags.access_levels & SC_ACCESS_LVL_ELEMENT_EXIST) == 0)
  {
    *el = null_ptr;
    result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;
  }

  return result;
}

sc_result sc_storage_remove_element_by_addr(sc_addr addr)
{
  sc_result result = SC_RESULT_OK;

  if (SC_ADDR_IS_EMPTY(addr) || addr.seg >= storage->max_segments_count || addr.offset > SC_SEGMENT_ELEMENTS_COUNT)
    result = SC_RESULT_ERROR_ADDR_IS_NOT_VALID;

  storage->segments[addr.seg]->elements[addr.offset - 1] = (sc_element){};

  return result;
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
