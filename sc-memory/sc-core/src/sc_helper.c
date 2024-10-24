/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-core/sc_helper.h"

#include <string.h>
#include <regex.h>

#include "sc-core/sc_memory_headers.h"
#include "sc-core/sc-base/sc_allocator.h"
#include "sc-core/sc-container/sc_string.h"

#include "sc-store/sc-base/sc_message.h"

#include "sc_memory_private.h"

sc_char ** keynodes_str = null_ptr;
sc_addr * sc_keynodes = null_ptr;

sc_result resolve_nrel_system_identifier(sc_memory_context const * ctx)
{
  sc_stream * stream = sc_stream_memory_new(
      keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER],
      (sc_uint)(sizeof(sc_uchar) * strlen(keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER])),
      SC_STREAM_FLAG_READ,
      SC_FALSE);

  // try to find nrel_system_identifier strings
  sc_list * found_links = null_ptr;
  sc_result result = sc_memory_find_links_with_content_string(ctx, stream, &found_links);
  if (result != SC_RESULT_OK)
    goto error;

  sc_bool system_idtf_addr_is_found = SC_FALSE;
  sc_iterator * system_idtf_addr_it = sc_list_iterator(found_links);
  while (sc_iterator_next(system_idtf_addr_it))
  {
    sc_addr_hash addr_hash = (sc_pointer_to_sc_addr_hash)sc_iterator_get(system_idtf_addr_it);
    sc_addr addr;
    SC_ADDR_LOCAL_FROM_INT(addr_hash, addr);

    sc_iterator5 * it = sc_iterator5_a_a_f_a_a_new(
        ctx,
        sc_type_node | sc_type_const | sc_type_node_no_role,
        sc_type_const_common_arc,
        addr,
        sc_type_const_perm_pos_arc,
        sc_type_const | sc_type_node | sc_type_node_no_role);

    while (sc_iterator5_next(it))
    {
      sc_addr addr1 = sc_iterator5_value(it, 0);
      sc_addr addr2 = sc_iterator5_value(it, 4);
      // compare begin sc-element and attribute, they must be equivalent
      if (SC_ADDR_IS_EQUAL(addr1, addr2))
      {
        if (system_idtf_addr_is_found == SC_FALSE)
        {
          sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER] = addr1;
          system_idtf_addr_is_found = SC_TRUE;
        }
        else
        {
          sc_memory_error("There are more then one sc-element with system identifier `nrel_system_identifier`");
          result = SC_RESULT_ERROR_DUPLICATED_SYSTEM_IDENTIFIER;
          sc_iterator5_free(it);
          sc_iterator_destroy(system_idtf_addr_it);
          goto error;
        }
      }
    }

    sc_iterator5_free(it);
  }
  sc_iterator_destroy(system_idtf_addr_it);

  sc_list_destroy(found_links);
  sc_stream_free(stream);

  return system_idtf_addr_is_found ? SC_RESULT_OK : SC_RESULT_NO;

error:
  sc_list_destroy(found_links);
  sc_stream_free(stream);

  return result;
}

void _init_keynodes_str()
{
  keynodes_str = sc_mem_new(sc_char *, SC_KEYNODE_COUNT);
  keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER] = "nrel_system_identifier";

  // check for errors
  for (sc_uint32 i = 0; i < (sc_uint32)SC_KEYNODE_COUNT; ++i)
  {
    if (keynodes_str[(sc_keynode)i] == null_ptr)
      sc_memory_error("Error to create string representation of keynode: %d", i);
  }
}

void _destroy_keynodes_str()
{
  if (keynodes_str == null_ptr)
    return;

  sc_mem_free(keynodes_str);
}

sc_result sc_helper_init(sc_memory_context * ctx)
{
  sc_memory_info("Initialize sc-helper");

  _init_keynodes_str();

  sc_keynodes = sc_mem_new(sc_addr, SC_KEYNODE_COUNT);

  sc_result result = resolve_nrel_system_identifier(ctx);
  if (result != SC_RESULT_OK && result != SC_RESULT_NO)
    goto finish;

  if (result == SC_RESULT_OK)
    goto finish;

  sc_memory_info("Can't resolve nrel_system_identifier node. Create the last one");

  sc_addr addr = sc_memory_node_new(ctx, sc_type_node | sc_type_const | sc_type_node_no_role);
  sc_addr link = sc_memory_link_new(ctx);

  sc_stream * stream = sc_stream_memory_new(
      keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER],
      (sc_uint)(sizeof(sc_uchar) * strlen(keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER])),
      SC_STREAM_FLAG_READ,
      SC_FALSE);
  result = sc_memory_set_link_content(ctx, link, stream);
  if (result != SC_RESULT_OK)
  {
    sc_stream_free(stream);
    goto finish;
  }

  sc_stream_free(stream);

  sc_addr arc = sc_memory_arc_new(ctx, sc_type_const_common_arc, addr, link);
  sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, addr, arc);
  sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER] = addr;

finish:
  return result;
}

void sc_helper_shutdown()
{
  sc_memory_info("Shutdown sc-helper");

  sc_mem_free(sc_keynodes);
  _destroy_keynodes_str();
}

sc_result sc_helper_check_system_identifier(sc_char const * data)
{
  regex_t regex;
  regcomp(&regex, REGEX_SYSTEM_IDTF, REG_EXTENDED);

  if (regexec(&regex, data, 0, NULL, 0) == 0)
  {
    regfree(&regex);
    return SC_RESULT_OK;
  }

  regfree(&regex);
  return SC_RESULT_ERROR_INVALID_SYSTEM_IDENTIFIER;
}

void sc_system_identifier_fiver_make_empty(sc_system_identifier_fiver * fiver)
{
  if (fiver == null_ptr)
    return;

  SC_ADDR_MAKE_EMPTY(fiver->addr1);
  SC_ADDR_MAKE_EMPTY(fiver->addr2);
  SC_ADDR_MAKE_EMPTY(fiver->addr3);
  SC_ADDR_MAKE_EMPTY(fiver->addr4);
  SC_ADDR_MAKE_EMPTY(fiver->addr5);
}

sc_result sc_helper_find_element_by_system_identifier(
    sc_memory_context const * ctx,
    sc_char const * data,
    sc_uint32 len,
    sc_addr * result_addr)
{
  sc_system_identifier_fiver fiver;
  sc_result const result = sc_helper_find_element_by_system_identifier_ext(ctx, data, len, &fiver);
  *result_addr = fiver.addr1;
  return result;
}

sc_result sc_helper_find_element_by_system_identifier_ext(
    sc_memory_context const * ctx,
    sc_char const * data,
    sc_uint32 len,
    sc_system_identifier_fiver * out_fiver)
{
  sc_stream * stream = null_ptr;
  sc_system_identifier_fiver_make_empty(out_fiver);

  sc_result result = sc_helper_check_system_identifier(data);
  if (result != SC_RESULT_OK)
    goto error;

  sc_list * found_links;
  stream = sc_stream_memory_new(data, sizeof(sc_char) * len, SC_STREAM_FLAG_READ, SC_FALSE);

  // try to find sc-link with that contains system identifier value
  result = sc_memory_find_links_with_content_string(ctx, stream, &found_links);
  if (result != SC_RESULT_OK)
  {
    sc_list_destroy(found_links);
    sc_stream_free(stream);
    goto error;
  }

  sc_iterator * links_it = sc_list_iterator(found_links);
  while (sc_iterator_next(links_it))
  {
    sc_addr_hash addr_hash = (sc_pointer_to_sc_addr_hash)sc_iterator_get(links_it);
    sc_addr addr;
    SC_ADDR_LOCAL_FROM_INT(addr_hash, addr);

    sc_iterator5 * it = sc_iterator5_a_a_f_a_f_new(
        ctx,
        0,
        sc_type_const_common_arc,
        addr,
        sc_type_const_perm_pos_arc,
        sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER]);
    if (sc_iterator5_next(it))
    {
      result = SC_TRUE;
      *out_fiver = (sc_system_identifier_fiver){
          sc_iterator5_value(it, 0),
          sc_iterator5_value(it, 1),
          sc_iterator5_value(it, 2),
          sc_iterator5_value(it, 3),
          sc_iterator5_value(it, 4)};

      sc_iterator5_free(it);
      sc_iterator_destroy(links_it);

      goto success;
    }

    sc_iterator5_free(it);
  }
  sc_iterator_destroy(links_it);

  result = SC_RESULT_NO;
success:
  sc_stream_free(stream);
  sc_list_destroy(found_links);

error:
  return result;
}

sc_result sc_helper_set_system_identifier(sc_memory_context * ctx, sc_addr addr, sc_char const * data, sc_uint32 len)
{
  return sc_helper_set_system_identifier_ext(ctx, addr, data, len, null_ptr);
}

sc_result sc_helper_set_system_identifier_ext(
    sc_memory_context * ctx,
    sc_addr addr,
    sc_char const * data,
    sc_uint32 len,
    sc_system_identifier_fiver * out_fiver)
{
  sc_system_identifier_fiver_make_empty(out_fiver);

  sc_result result = sc_helper_check_system_identifier(data);
  if (result != SC_RESULT_OK)
    goto error;

  sc_stream * stream = null_ptr;
  sc_addr idtf_addr = SC_ADDR_EMPTY;

  // check if specified system identifier already used
  stream = sc_stream_memory_new(data, sizeof(sc_char) * len, SC_STREAM_FLAG_READ, SC_FALSE);
  result = sc_helper_find_element_by_system_identifier(ctx, data, len, &idtf_addr);
  if (result == SC_RESULT_OK)
  {
    sc_stream_free(stream);
    result = SC_RESULT_ERROR_DUPLICATED_SYSTEM_IDENTIFIER;
    goto error;
  }
  if (result != SC_RESULT_NO)
  {
    sc_stream_free(stream);
    goto error;
  }

  // if there are no elements with specified system identifier, then we can use it
  idtf_addr = sc_memory_link_new(ctx);
  result = sc_memory_set_link_content(ctx, idtf_addr, stream);
  if (result != SC_RESULT_OK)
  {
    sc_stream_free(stream);
    goto error;
  }

  // we doesn't need link data anymore
  sc_stream_free(stream);

  // setup new system identifier
  sc_addr arc_addr = sc_memory_arc_new_ext(ctx, sc_type_const_common_arc, addr, idtf_addr, &result);
  if (result != SC_RESULT_OK)
    goto error;

  sc_addr const arc_to_arc_addr =
      sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER], arc_addr);
  if (result != SC_RESULT_OK)
    goto error;

  if (out_fiver != null_ptr)
    *out_fiver = (sc_system_identifier_fiver){
        addr, arc_addr, idtf_addr, arc_to_arc_addr, sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER]};

error:
  return result;
}

sc_result sc_helper_get_system_identifier_link(sc_memory_context const * ctx, sc_addr el, sc_addr * sys_idtf_addr)
{
  *sys_idtf_addr = SC_ADDR_EMPTY;

  sc_iterator5 * it = null_ptr;
  sc_result result = SC_RESULT_NO;

  it = sc_iterator5_f_a_a_a_f_new(
      ctx,
      el,
      sc_type_const | sc_type_common_arc,
      sc_type_const_node_link,
      sc_type_const_perm_pos_arc,
      sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER]);
  if (sc_iterator5_next(it))
  {
    *sys_idtf_addr = sc_iterator5_value(it, 2);
    result = SC_RESULT_OK;
  }

  sc_iterator5_free(it);

  return result;
}

sc_bool sc_helper_resolve_system_identifier(sc_memory_context * ctx, sc_char const * system_idtf, sc_addr * result_addr)
{
  sc_system_identifier_fiver fiver;
  return sc_helper_resolve_system_identifier_ext(ctx, system_idtf, sc_type_node | sc_type_const, result_addr, &fiver);
}

sc_bool sc_helper_resolve_system_identifier_ext(
    sc_memory_context * ctx,
    sc_char const * system_idtf,
    sc_type type,
    sc_addr * result_addr,
    sc_system_identifier_fiver * fiver)
{
  *result_addr = SC_ADDR_EMPTY;

  if (ctx == null_ptr || system_idtf == null_ptr)
    return SC_FALSE;

  sc_uint32 const string_size = sc_str_len(system_idtf);
  sc_result result = sc_helper_find_element_by_system_identifier_ext(ctx, system_idtf, string_size, fiver);
  if (result != SC_RESULT_OK && result != SC_RESULT_NO)
    goto error;

  if (result == SC_RESULT_OK)
  {
    *result_addr = fiver->addr1;
    return SC_TRUE;
  }

  *result_addr = sc_memory_node_new(ctx, type);
  result = sc_helper_set_system_identifier_ext(ctx, *result_addr, system_idtf, string_size, fiver);
  if (result != SC_RESULT_OK)
    goto error;

  return SC_TRUE;
error:
  return SC_FALSE;
}

sc_bool sc_helper_check_arc(sc_memory_context const * ctx, sc_addr beg_el, sc_addr end_el, sc_type arc_type)
{
  sc_result result;
  return sc_helper_check_arc_ext(ctx, beg_el, end_el, arc_type, &result);
}

sc_bool sc_helper_check_arc_ext(
    sc_memory_context const * ctx,
    sc_addr beg_el,
    sc_addr end_el,
    sc_type arc_type,
    sc_result * result)
{
  sc_iterator3 * it = null_ptr;
  sc_bool status = SC_FALSE;

  it = sc_iterator3_f_a_f_new(ctx, beg_el, arc_type, end_el);
  if (it == null_ptr)
    return SC_FALSE;

  if (sc_iterator3_next_ext(it, result) == SC_TRUE)
    status = SC_TRUE;

  sc_iterator3_free(it);
  return status;
}
