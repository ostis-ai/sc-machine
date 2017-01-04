/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "sc-memory/cpp/sc_memory.hpp"

namespace scp
{

ScAddr Keynodes::scp_program;
ScAddr Keynodes::agent_scp_program;
ScAddr Keynodes::platform_independent_abstract_sc_agent;
ScAddr Keynodes::abstract_sc_agent;

ScAddr Keynodes::nrel_sc_agent_program;
ScAddr Keynodes::nrel_inclusion;
ScAddr Keynodes::nrel_primary_initiation_condition;
ScAddr Keynodes::question_scp_interpretation_request;
ScAddr Keynodes::question_initiated;
ScAddr Keynodes::question_finished;
ScAddr Keynodes::question_finished_successfully;
ScAddr Keynodes::question_finished_unsuccessfully;
ScAddr Keynodes::scp_process;
ScAddr Keynodes::abstract_scp_machine;
ScAddr Keynodes::nrel_scp_process;
ScAddr Keynodes::nrel_authors;
ScAddr Keynodes::nrel_system_identifier;
ScAddr Keynodes::active_action;
ScAddr Keynodes::active_sc_agent;

// rrels
ScAddr Keynodes::nrel_then;
ScAddr Keynodes::nrel_else;
ScAddr Keynodes::nrel_goto;
ScAddr Keynodes::nrel_error;
ScAddr Keynodes::rrel_in;
ScAddr Keynodes::rrel_out;
ScAddr Keynodes::rrel_fixed;
ScAddr Keynodes::rrel_assign;
ScAddr Keynodes::rrel_set;
ScAddr Keynodes::rrel_set_1;
ScAddr Keynodes::rrel_set_2;
ScAddr Keynodes::rrel_set_3;
ScAddr Keynodes::rrel_set_4;
ScAddr Keynodes::rrel_set_5;
ScAddr Keynodes::rrel_erase;
ScAddr Keynodes::rrel_const;
ScAddr Keynodes::rrel_var;
ScAddr Keynodes::rrel_scp_const;
ScAddr Keynodes::rrel_scp_var;
ScAddr Keynodes::rrel_node;
ScAddr Keynodes::rrel_link;
ScAddr Keynodes::rrel_arc;
ScAddr Keynodes::rrel_edge;
ScAddr Keynodes::rrel_pos;
ScAddr Keynodes::rrel_neg;
ScAddr Keynodes::rrel_fuz;
ScAddr Keynodes::rrel_temp;
ScAddr Keynodes::rrel_perm;
ScAddr Keynodes::rrel_access;
ScAddr Keynodes::rrel_common;
ScAddr Keynodes::rrel_pos_const_perm;

//Node types
ScAddr Keynodes::rrel_struct;
ScAddr Keynodes::rrel_role_relation;
ScAddr Keynodes::rrel_norole_relation;
ScAddr Keynodes::rrel_not_relation;

// Events
ScAddr Keynodes::sc_event_remove_element;
ScAddr Keynodes::sc_event_add_output_arc;
ScAddr Keynodes::sc_event_add_input_arc;
ScAddr Keynodes::sc_event_remove_output_arc;
ScAddr Keynodes::sc_event_remove_input_arc;
ScAddr Keynodes::sc_event_content_changed;

// Order relations
ScAddr Keynodes::rrel_1;
ScAddr Keynodes::rrel_2;
ScAddr Keynodes::rrel_3;
ScAddr Keynodes::rrel_4;
ScAddr Keynodes::rrel_5;
ScAddr Keynodes::rrel_6;
ScAddr Keynodes::rrel_7;
ScAddr Keynodes::rrel_8;
ScAddr Keynodes::rrel_9;
ScAddr Keynodes::rrel_10;
ScAddr Keynodes::order_role_relation;
}
