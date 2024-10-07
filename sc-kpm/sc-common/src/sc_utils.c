/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-common/sc_utils.h"

#include <sc-core/sc_helper.h>

#include "sc-common/sc_keynodes.h"
#include "sc-common/sc_defines.h"

sc_addr create_result_node(sc_memory_context * context)
{
  sc_addr res = sc_memory_node_new(context, sc_type_const_node);
  SYSTEM_ELEMENT(context, res);
  return res;
}

void connect_result_to_action(sc_memory_context * context, sc_addr action, sc_addr result)
{
  sc_addr arc = sc_memory_arc_new(context, sc_type_const_common_arc, action, result);
  SYSTEM_ELEMENT(context, arc);
  arc = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_nrel_result, arc);
  SYSTEM_ELEMENT(context, arc);
}

void appendIntoResult(sc_memory_context * context, sc_addr result, sc_addr el)
{
  sc_addr arc;
  if (sc_helper_check_arc(context, result, el, sc_type_const_perm_pos_arc) == SC_TRUE)
    return;

  arc = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, result, el);
  SYSTEM_ELEMENT(context, arc);
}

void finish_action(sc_memory_context * context, sc_addr action)
{
  sc_addr arc;

  arc = sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_finished, action);
  SYSTEM_ELEMENT(context, arc);
}

void finish_action_successfully(sc_memory_context * context, sc_addr action)
{
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_finished_successfully, action);
}

void finish_action_unsuccessfully(sc_memory_context * context, sc_addr action)
{
  sc_memory_arc_new(context, sc_type_const_perm_pos_arc, keynode_action_finished_unsuccessfully, action);
}
