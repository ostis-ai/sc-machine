/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-common/sc_keynodes.h"

#include <sc-core/sc_iterator3.h>

sc_addr keynode_action;
sc_addr keynode_action_initiated;
sc_addr keynode_action_finished;
sc_addr keynode_action_finished_successfully;
sc_addr keynode_action_finished_unsuccessfully;
sc_addr keynode_nrel_result;
sc_addr keynode_rrel_1;
sc_addr keynode_rrel_2;

sc_addr keynode_system_element;

sc_char const keynode_action_str[] = "action";
sc_char const keynode_action_initiated_str[] = "action_initiated";
sc_char const keynode_action_finished_str[] = "action_finished";
sc_char const keynode_action_finished_successfully_str[] = "action_finished_successfully";
sc_char const keynode_action_finished_unsuccessfully_str[] = "action_finished_unsuccessfully";
sc_char const keynode_nrel_result_str[] = "nrel_result";
sc_char const keynode_rrel_1_str[] = "rrel_1";
sc_char const keynode_rrel_2_str[] = "rrel_2";

sc_char const keynode_system_element_str[] = "system_element";

sc_result sc_common_keynodes_initialize(sc_memory_context * context, sc_addr const init_memory_generated_structure)
{
  RESOLVE_KEYNODE(context, keynode_action, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_action_initiated, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_action_finished, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_action_finished_successfully, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_action_finished_unsuccessfully, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_nrel_result, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_rrel_1, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_rrel_2, init_memory_generated_structure);
  RESOLVE_KEYNODE(context, keynode_system_element, init_memory_generated_structure);

  return SC_RESULT_OK;
}

sc_result sc_common_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_addr init_memory_generated_structure,
    sc_addr * keynode)
{
  sc_system_identifier_fiver fiver;
  sc_result const result = sc_helper_resolve_system_identifier_ext(ctx, sys_idtf, sc_type_const_node, keynode, &fiver);

  if (SC_ADDR_IS_NOT_EMPTY(init_memory_generated_structure))
  {
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr1);
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr2);
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr3);
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr4);
  }

  return result;
}
