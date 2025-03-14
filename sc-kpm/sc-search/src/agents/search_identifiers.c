/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_identifiers.h"
#include "search_keynodes.h"
#include "search.h"

#include <sc-core/sc_memory_headers.h>

#include <sc-common/sc_utils.h>

sc_result agent_search_all_identifiers(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action, result;
  sc_iterator3 *it1, *it2;
  sc_iterator5 * it5;
  sc_bool found = SC_FALSE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &action))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // check action type
  if (sc_helper_check_arc(s_default_ctx, keynode_action_all_identifiers, action, sc_type_const_perm_pos_arc)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  result = create_result_node(s_default_ctx);

  // get operation argument
  it1 = sc_iterator3_f_a_a_new(s_default_ctx, action, sc_type_const_perm_pos_arc, 0);
  if (sc_iterator3_next(it1) == SC_TRUE)
  {
    found = SC_TRUE;
    appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 2));

    // iterate all const arcs, that are no accessory, and go out from sc-element
    it5 = sc_iterator5_f_a_a_a_a_new(
        s_default_ctx,
        sc_iterator3_value(it1, 2),
        sc_type_const_common_arc,
        sc_type_node_link,
        sc_type_const_perm_pos_arc,
        sc_type_node | sc_type_const | sc_type_node_non_role);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
      // check if this relation is an identification
      if (sc_helper_check_arc(
              s_default_ctx, keynode_identification_relation, sc_iterator5_value(it5, 4), sc_type_const_perm_pos_arc)
          == SC_TRUE)
      {
        // iterate incoming sc-arcs for sc-link
        it2 = sc_iterator3_a_a_f_new(
            s_default_ctx, sc_type_node | sc_type_const, sc_type_const_perm_pos_arc, sc_iterator5_value(it5, 2));
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
          if (sc_helper_check_arc(
                  s_default_ctx, keynode_languages, sc_iterator3_value(it2, 0), sc_type_const_perm_pos_arc)
              == SC_TRUE)
          {
            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 0));
            appendIntoResult(s_default_ctx, result, sc_iterator3_value(it2, 1));
          }
        }
        sc_iterator3_free(it2);

        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 1));
        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 2));
        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 3));
        appendIntoResult(s_default_ctx, result, sc_iterator5_value(it5, 4));
      }
    }
    sc_iterator5_free(it5);
  }
  sc_iterator3_free(it1);

  if (found == SC_TRUE)
    appendIntoResult(s_default_ctx, result, keynode_nrel_identification);

  connect_result_to_action(s_default_ctx, action, result);
  finish_action(s_default_ctx, action);

  return SC_RESULT_OK;
}

sc_result agent_search_all_identified_elements(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action, result, begin, end;
  sc_iterator3 * it1;
  sc_bool found = SC_FALSE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &action))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // check action type
  if (sc_helper_check_arc(s_default_ctx, keynode_action_all_identified_elements, action, sc_type_const_perm_pos_arc)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  result = create_result_node(s_default_ctx);

  it1 = sc_iterator3_f_a_a_new(
      s_default_ctx, keynode_nrel_main_idtf, sc_type_const_perm_pos_arc, sc_type_common_arc | sc_type_const);
  while (sc_iterator3_next(it1) == SC_TRUE)
  {
    found = SC_TRUE;
    sc_memory_get_arc_begin(s_default_ctx, sc_iterator3_value(it1, 2), &begin);
    sc_memory_get_arc_end(s_default_ctx, sc_iterator3_value(it1, 2), &end);

    appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 1));
    appendIntoResult(s_default_ctx, result, sc_iterator3_value(it1, 2));
    appendIntoResult(s_default_ctx, result, begin);
    appendIntoResult(s_default_ctx, result, end);
  }
  sc_iterator3_free(it1);

  if (found == SC_TRUE)
    appendIntoResult(s_default_ctx, result, keynode_nrel_main_idtf);

  connect_result_to_action(s_default_ctx, action, result);
  finish_action(s_default_ctx, action);

  return SC_RESULT_OK;
}
