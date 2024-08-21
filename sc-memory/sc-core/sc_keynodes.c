/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include "sc_helper.h"

#include "sc-store/sc-container/sc-string/sc_string.h"

sc_addr sc_event_unknown_addr;
sc_addr sc_event_generate_incoming_arc_addr;
sc_addr sc_event_generate_outgoing_arc_addr;
sc_addr sc_event_generate_edge_addr;
sc_addr sc_event_erase_incoming_arc_addr;
sc_addr sc_event_erase_outgoing_arc_addr;
sc_addr sc_event_erase_edge_addr;
sc_addr sc_event_erase_element_addr;
sc_addr sc_event_change_link_content_addr;

sc_addr myself_addr;
sc_addr concept_guest_user_addr;
sc_addr nrel_identified_user_addr;
sc_addr concept_authentication_request_user_addr;
sc_addr concept_authenticated_user_addr;
sc_addr nrel_user_action_class_addr;
sc_addr nrel_users_set_action_class_addr;
sc_addr action_read_from_sc_memory_addr;
sc_addr action_generate_in_sc_memory_addr;
sc_addr action_erase_from_sc_memory_addr;
sc_addr action_read_permissions_from_sc_memory_addr;
sc_addr action_generate_permissions_in_sc_memory_addr;
sc_addr action_erase_permissions_from_sc_memory_addr;
sc_addr nrel_user_action_class_within_sc_structure_addr;
sc_addr nrel_users_set_action_class_within_sc_structure_addr;

sc_result sc_keynodes_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_type type,
    sc_addr const init_memory_generated_structure,
    sc_addr * keynode)
{
  sc_system_identifier_fiver fiver;
  sc_result const result = sc_helper_resolve_system_identifier_ext(ctx, sys_idtf, type, keynode, &fiver);

  if (SC_ADDR_IS_NOT_EMPTY(init_memory_generated_structure))
  {
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr1);
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr2);
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr3);
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, fiver.addr4);
  }

  return result;
}

#define SC_RESOLVE_KEYNODE(ctx, keynode, type, init_memory_generated_structure) \
  ({ \
    sc_char const keynode_identifier[] = #keynode; \
    sc_uint32 const pos = sc_str_find_position(keynode_identifier, "_addr"); \
    sc_char copied_keynode_identifier[pos]; \
    sc_mem_cpy(copied_keynode_identifier, keynode_identifier, pos - 1); \
    copied_keynode_identifier[pos - 1] = '\0'; \
    if (sc_keynodes_resolve_keynode(ctx, copied_keynode_identifier, type, init_memory_generated_structure, &keynode) \
        != SC_RESULT_OK) \
      return SC_RESULT_ERROR; \
  })

sc_result sc_keynodes_initialize(sc_memory_context * ctx, sc_addr const init_memory_generated_structure)
{
  SC_RESOLVE_KEYNODE(ctx, sc_event_unknown_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, sc_event_generate_incoming_arc_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, sc_event_generate_outgoing_arc_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, sc_event_generate_edge_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, sc_event_erase_incoming_arc_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, sc_event_erase_outgoing_arc_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, sc_event_erase_edge_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, sc_event_erase_element_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, sc_event_change_link_content_addr, sc_type_node_const_class, init_memory_generated_structure);

  SC_RESOLVE_KEYNODE(ctx, myself_addr, sc_type_node_const, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, concept_guest_user_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, nrel_identified_user_addr, sc_type_node_const_norole, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, concept_authentication_request_user_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, concept_authenticated_user_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, nrel_user_action_class_addr, sc_type_node_const_norole, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, nrel_users_set_action_class_addr, sc_type_node_const_norole, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, action_read_from_sc_memory_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, action_generate_in_sc_memory_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(ctx, action_erase_from_sc_memory_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, action_read_permissions_from_sc_memory_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, action_generate_permissions_in_sc_memory_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, action_erase_permissions_from_sc_memory_addr, sc_type_node_const_class, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx, nrel_user_action_class_within_sc_structure_addr, sc_type_node_const_norole, init_memory_generated_structure);
  SC_RESOLVE_KEYNODE(
      ctx,
      nrel_users_set_action_class_within_sc_structure_addr,
      sc_type_node_const_norole,
      init_memory_generated_structure);

  return SC_RESULT_OK;
}
