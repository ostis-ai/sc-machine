/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_keynodes_h_
#define _sc_keynodes_h_

#include "sc-store/sc_types.h"

extern sc_addr sc_event_unknown_addr;
extern sc_addr sc_event_add_input_arc_addr;
extern sc_addr sc_event_add_output_arc_addr;
extern sc_addr sc_event_add_edge_addr;
extern sc_addr sc_event_remove_input_arc_addr;
extern sc_addr sc_event_remove_output_arc_addr;
extern sc_addr sc_event_remove_edge_addr;
extern sc_addr sc_event_remove_element_addr;
extern sc_addr sc_event_change_content_addr;

extern sc_addr myself_addr;
extern sc_addr concept_guest_user_addr;
extern sc_addr nrel_identified_user_addr;
extern sc_addr concept_authentication_request_user_addr;
extern sc_addr concept_authenticated_user_addr;
extern sc_addr nrel_user_action_class_addr;
extern sc_addr nrel_users_set_action_class_addr;
extern sc_addr action_read_from_sc_memory_addr;
extern sc_addr action_generate_in_sc_memory_addr;
extern sc_addr action_erase_from_sc_memory_addr;
extern sc_addr action_read_permissions_from_sc_memory_addr;
extern sc_addr action_generate_permissions_in_sc_memory_addr;
extern sc_addr action_erase_permissions_from_sc_memory_addr;
extern sc_addr nrel_user_action_class_within_sc_structure_addr;
extern sc_addr nrel_users_set_action_class_within_sc_structure_addr;

sc_result sc_keynodes_initialize(sc_memory_context * ctx, sc_addr init_memory_generated_structure);

#endif
