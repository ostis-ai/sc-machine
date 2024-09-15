/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_utils.h"
#include "search_keynodes.h"
#include "search_defines.h"
#include "search.h"

#include "sc-core/sc_helper.h"

sc_addr create_result_node()
{
  sc_addr res = sc_memory_node_new(s_default_ctx, sc_type_node | sc_type_const | sc_type_node_structure);
  SYSTEM_ELEMENT(res);
  return res;
}

void connect_result_to_action(sc_addr action, sc_addr result)
{
  sc_addr arc = sc_memory_arc_new(s_default_ctx, sc_type_common_arc | sc_type_const, action, result);
  SYSTEM_ELEMENT(arc);
  arc = sc_memory_arc_new(s_default_ctx, sc_type_const_perm_pos_arc, keynode_nrel_result, arc);
  SYSTEM_ELEMENT(arc);
}

void appendIntoResult(sc_addr result, sc_addr el)
{
  sc_addr arc;
  if (sc_helper_check_arc(s_default_ctx, result, el, sc_type_const_perm_pos_arc) == SC_TRUE)
    return;

  arc = sc_memory_arc_new(s_default_ctx, sc_type_const_perm_pos_arc, result, el);
  SYSTEM_ELEMENT(arc);
}

void finish_action(sc_addr action)
{
  sc_addr arc;

  arc = sc_memory_arc_new(s_default_ctx, sc_type_const_perm_pos_arc, keynode_action_finished, action);
  SYSTEM_ELEMENT(arc);
}

void finish_action_successfully(sc_memory_context * ctx, sc_addr action)
{
  sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, keynode_action_finished_successfully, action);
}

void finish_action_unsuccessfully(sc_memory_context * ctx, sc_addr action)
{
  sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, keynode_action_finished_unsuccessfully, action);
}
