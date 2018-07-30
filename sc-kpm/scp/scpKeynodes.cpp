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
ScAddr Keynodes::question_finished_with_error;
ScAddr Keynodes::scp_process;
ScAddr Keynodes::abstract_scp_machine;
ScAddr Keynodes::nrel_scp_process;
ScAddr Keynodes::rrel_key_sc_element;
ScAddr Keynodes::nrel_decomposition_of_action;
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
ScAddr Keynodes::rrel_class;

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

//Operators
ScAddr Keynodes::scp_operator_atomic_type;
ScAddr Keynodes::op_searchElStr3;
ScAddr Keynodes::op_searchElStr5;
ScAddr Keynodes::op_searchSetStr3;
ScAddr Keynodes::op_searchSetStr5;
ScAddr Keynodes::op_genEl;
ScAddr Keynodes::op_genElStr3;
ScAddr Keynodes::op_genElStr5;
ScAddr Keynodes::op_eraseEl;
ScAddr Keynodes::op_eraseElStr3;
ScAddr Keynodes::op_eraseElStr5;
ScAddr Keynodes::op_eraseSetStr3;
ScAddr Keynodes::op_eraseSetStr5;
ScAddr Keynodes::op_ifVarAssign;
ScAddr Keynodes::op_ifFormCont;
ScAddr Keynodes::op_ifCoin;
ScAddr Keynodes::op_ifType;
ScAddr Keynodes::op_varAssign;

#ifdef SCP_MATH
ScAddr Keynodes::op_ifEq;
ScAddr Keynodes::op_ifGr;
ScAddr Keynodes::op_contAdd;
ScAddr Keynodes::op_contDiv;
ScAddr Keynodes::op_contMult;
ScAddr Keynodes::op_contSub;
ScAddr Keynodes::op_contPow;
ScAddr Keynodes::op_contLn;
ScAddr Keynodes::op_contSin;
ScAddr Keynodes::op_contCos;
ScAddr Keynodes::op_contTg;
ScAddr Keynodes::op_contASin;
ScAddr Keynodes::op_contACos;
ScAddr Keynodes::op_contATg;
ScAddr Keynodes::op_contDivInt;
ScAddr Keynodes::op_contDivRem;
#endif

#ifdef SCP_STRING
ScAddr Keynodes::op_contStringConcat;
ScAddr Keynodes::op_stringIfEq;
ScAddr Keynodes::op_stringIfGr;
ScAddr Keynodes::op_stringSplit;
ScAddr Keynodes::op_stringLen;
ScAddr Keynodes::op_stringSub;
ScAddr Keynodes::op_stringSlice;
ScAddr Keynodes::op_stringStartsWith;
ScAddr Keynodes::op_stringEndsWith;
ScAddr Keynodes::op_stringReplace;
ScAddr Keynodes::op_stringToUpperCase;
ScAddr Keynodes::op_stringToLowerCase;
#endif

ScAddr Keynodes::op_contAssign;
ScAddr Keynodes::op_contErase;
ScAddr Keynodes::op_print;
ScAddr Keynodes::op_printNl;
ScAddr Keynodes::op_printEl;
ScAddr Keynodes::op_sys_search;
ScAddr Keynodes::op_sys_gen;
ScAddr Keynodes::op_call;
ScAddr Keynodes::op_waitReturn;
ScAddr Keynodes::op_waitReturnSet;
ScAddr Keynodes::op_return;
ScAddr Keynodes::op_sys_wait;
ScAddr Keynodes::op_syncronize;
}
