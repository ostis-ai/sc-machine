/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include "sc_helper.h"

#include "sc-store/sc-container/sc-string/sc_string.h"

sc_addr concept_authentication_request_user_addr;
sc_addr concept_authenticated_user_addr;
sc_addr nrel_user_action_class_addr;
sc_addr read_action_in_sc_memory_addr;
sc_addr write_action_in_sc_memory_addr;
sc_addr erase_action_in_sc_memory_addr;
sc_addr read_access_levels_action_in_sc_memory_addr;
sc_addr write_access_levels_action_in_sc_memory_addr;
sc_addr erase_access_levels_action_in_sc_memory_addr;

sc_result sc_keynodes_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_addr const init_memory_generated_structure,
    sc_addr * keynode)
{
  sc_system_identifier_fiver fiver;
  sc_result const result = sc_helper_resolve_system_identifier_ext(ctx, sys_idtf, keynode, &fiver);

  if (SC_ADDR_IS_NOT_EMPTY(init_memory_generated_structure))
  {
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr1);
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr2);
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr3);
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr4);
  }

  return result;
}

#define SC_RESOLVE_KEYNODE(ctx, keynode, init_memory_generated_structure) \
  ({ \
    sc_char const keynode_identifier[] = #keynode; \
    sc_uint32 const pos = sc_str_find_position(keynode_identifier, "_addr"); \
    sc_char copied_keynode_identifier[pos]; \
    sc_mem_cpy(copied_keynode_identifier, keynode_identifier, pos - 1); \
    copied_keynode_identifier[pos - 1] = '\0'; \
    if (sc_keynodes_resolve_keynode(ctx, copied_keynode_identifier, init_memory_generated_structure, &keynode) \
        != SC_RESULT_OK) \
      return SC_RESULT_ERROR; \
  })

sc_result sc_keynodes_initialize(sc_memory_context * ctx, sc_addr const init_memory_generated_structure)
{
  SC_RESOLVE_KEYNODE(ctx, concept_authentication_request_user_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, concept_authenticated_user_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, nrel_user_action_class_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, read_action_in_sc_memory_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, write_action_in_sc_memory_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, erase_action_in_sc_memory_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, read_access_levels_action_in_sc_memory_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, write_access_levels_action_in_sc_memory_addr, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, erase_access_levels_action_in_sc_memory_addr, init_memory_generated_structure);

  return SC_RESULT_OK;
}
