/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"

#include "scpKeynodes.generated.hpp"

namespace scp
{
SC_CLASS()
class Keynodes : public ScObject
{
    SC_GENERATED_BODY()

public:

    SC_PROPERTY(Keynode("scp_program"), ForceCreate)
    static ScAddr scp_program;

    SC_PROPERTY(Keynode("agent_scp_program"), ForceCreate)
    static ScAddr agent_scp_program;

    SC_PROPERTY(Keynode("platform_independent_abstract_sc_agent"), ForceCreate)
    static ScAddr platform_independent_abstract_sc_agent;

    SC_PROPERTY(Keynode("abstract_sc_agent"), ForceCreate)
    static ScAddr abstract_sc_agent;

    SC_PROPERTY(Keynode("nrel_sc_agent_program"), ForceCreate)
    static ScAddr nrel_sc_agent_program;

    SC_PROPERTY(Keynode("nrel_inclusion"), ForceCreate)
    static ScAddr nrel_inclusion;

    SC_PROPERTY(Keynode("nrel_primary_initiation_condition"), ForceCreate)
    static ScAddr nrel_primary_initiation_condition;

    SC_PROPERTY(Keynode("question_scp_interpretation_request"), ForceCreate)
    static ScAddr question_scp_interpretation_request;

    SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
    static ScAddr question_initiated;

    SC_PROPERTY(Keynode("question_finished"), ForceCreate)
    static ScAddr question_finished;

    SC_PROPERTY(Keynode("question_finished_successfully"), ForceCreate)
    static ScAddr question_finished_successfully;

    SC_PROPERTY(Keynode("question_finished_unsuccessfully"), ForceCreate)
    static ScAddr question_finished_unsuccessfully;

    SC_PROPERTY(Keynode("question_finished_with_error"), ForceCreate)
    static ScAddr question_finished_with_error;

    SC_PROPERTY(Keynode("scp_process"), ForceCreate)
    static ScAddr scp_process;

    SC_PROPERTY(Keynode("abstract_scp_machine"), ForceCreate)
    static ScAddr abstract_scp_machine;

    SC_PROPERTY(Keynode("nrel_decomposition_of_action"), ForceCreate)
    static ScAddr nrel_decomposition_of_action;

    SC_PROPERTY(Keynode("nrel_scp_process"), ForceCreate)
    static ScAddr nrel_scp_process;

    SC_PROPERTY(Keynode("nrel_authors"), ForceCreate)
    static ScAddr nrel_authors;

    SC_PROPERTY(Keynode("nrel_system_identifier"), ForceCreate)
    static ScAddr nrel_system_identifier;

    SC_PROPERTY(Keynode("active_action"), ForceCreate)
    static ScAddr active_action;

    SC_PROPERTY(Keynode("active_sc_agent"), ForceCreate)
    static ScAddr active_sc_agent;

// rrels
    SC_PROPERTY(Keynode("nrel_then"), ForceCreate)
    static ScAddr nrel_then;

    SC_PROPERTY(Keynode("nrel_else"), ForceCreate)
    static ScAddr nrel_else;

    SC_PROPERTY(Keynode("nrel_goto"), ForceCreate)
    static ScAddr nrel_goto;

    SC_PROPERTY(Keynode("nrel_error"), ForceCreate)
    static ScAddr nrel_error;

    SC_PROPERTY(Keynode("rrel_in"), ForceCreate)
    static ScAddr rrel_in;

    SC_PROPERTY(Keynode("rrel_out"), ForceCreate)
    static ScAddr rrel_out;

    SC_PROPERTY(Keynode("rrel_fixed"), ForceCreate)
    static ScAddr rrel_fixed;

    SC_PROPERTY(Keynode("rrel_assign"), ForceCreate)
    static ScAddr rrel_assign;

    SC_PROPERTY(Keynode("rrel_set"), ForceCreate)
    static ScAddr rrel_set;

    SC_PROPERTY(Keynode("rrel_set_1"), ForceCreate)
    static ScAddr rrel_set_1;

    SC_PROPERTY(Keynode("rrel_set_2"), ForceCreate)
    static ScAddr rrel_set_2;

    SC_PROPERTY(Keynode("rrel_set_3"), ForceCreate)
    static ScAddr rrel_set_3;

    SC_PROPERTY(Keynode("rrel_set_4"), ForceCreate)
    static ScAddr rrel_set_4;

    SC_PROPERTY(Keynode("rrel_set_5"), ForceCreate)
    static ScAddr rrel_set_5;

    SC_PROPERTY(Keynode("rrel_erase"), ForceCreate)
    static ScAddr rrel_erase;

    SC_PROPERTY(Keynode("rrel_const"), ForceCreate)
    static ScAddr rrel_const;

    SC_PROPERTY(Keynode("rrel_var"), ForceCreate)
    static ScAddr rrel_var;

    SC_PROPERTY(Keynode("rrel_scp_const"), ForceCreate)
    static ScAddr rrel_scp_const;

    SC_PROPERTY(Keynode("rrel_scp_var"), ForceCreate)
    static ScAddr rrel_scp_var;

    SC_PROPERTY(Keynode("rrel_node"), ForceCreate)
    static ScAddr rrel_node;

    SC_PROPERTY(Keynode("rrel_link"), ForceCreate)
    static ScAddr rrel_link;

    SC_PROPERTY(Keynode("rrel_arc"), ForceCreate)
    static ScAddr rrel_arc;

    SC_PROPERTY(Keynode("rrel_edge"), ForceCreate)
    static ScAddr rrel_edge;

    SC_PROPERTY(Keynode("rrel_pos"), ForceCreate)
    static ScAddr rrel_pos;

    SC_PROPERTY(Keynode("rrel_neg"), ForceCreate)
    static ScAddr rrel_neg;

    SC_PROPERTY(Keynode("rrel_fuz"), ForceCreate)
    static ScAddr rrel_fuz;

    SC_PROPERTY(Keynode("rrel_temp"), ForceCreate)
    static ScAddr rrel_temp;

    SC_PROPERTY(Keynode("rrel_perm"), ForceCreate)
    static ScAddr rrel_perm;

    SC_PROPERTY(Keynode("rrel_access"), ForceCreate)
    static ScAddr rrel_access;

    SC_PROPERTY(Keynode("rrel_common"), ForceCreate)
    static ScAddr rrel_common;

    SC_PROPERTY(Keynode("rrel_pos_const_perm"), ForceCreate)
    static ScAddr rrel_pos_const_perm;

//Node types
    SC_PROPERTY(Keynode("rrel_struct"), ForceCreate)
    static ScAddr rrel_struct;

    SC_PROPERTY(Keynode("rrel_role_relation"), ForceCreate)
    static ScAddr rrel_role_relation;

    SC_PROPERTY(Keynode("rrel_norole_relation"), ForceCreate)
    static ScAddr rrel_norole_relation;

    SC_PROPERTY(Keynode("rrel_class"), ForceCreate)
    static ScAddr rrel_class;

// Events
    SC_PROPERTY(Keynode("sc_event_remove_element"), ForceCreate)
    static ScAddr sc_event_remove_element;

    SC_PROPERTY(Keynode("sc_event_add_output_arc"), ForceCreate)
    static ScAddr sc_event_add_output_arc;

    SC_PROPERTY(Keynode("sc_event_add_input_arc"), ForceCreate)
    static ScAddr sc_event_add_input_arc;

    SC_PROPERTY(Keynode("sc_event_remove_output_arc"), ForceCreate)
    static ScAddr sc_event_remove_output_arc;

    SC_PROPERTY(Keynode("sc_event_remove_input_arc"), ForceCreate)
    static ScAddr sc_event_remove_input_arc;

    SC_PROPERTY(Keynode("sc_event_content_changed"), ForceCreate)
    static ScAddr sc_event_content_changed;

// Order relations
    SC_PROPERTY(Keynode("rrel_1"), ForceCreate)
    static ScAddr rrel_1;

    SC_PROPERTY(Keynode("rrel_2"), ForceCreate)
    static ScAddr rrel_2;

    SC_PROPERTY(Keynode("rrel_3"), ForceCreate)
    static ScAddr rrel_3;

    SC_PROPERTY(Keynode("rrel_4"), ForceCreate)
    static ScAddr rrel_4;

    SC_PROPERTY(Keynode("rrel_5"), ForceCreate)
    static ScAddr rrel_5;

    SC_PROPERTY(Keynode("rrel_6"), ForceCreate)
    static ScAddr rrel_6;

    SC_PROPERTY(Keynode("rrel_7"), ForceCreate)
    static ScAddr rrel_7;

    SC_PROPERTY(Keynode("rrel_8"), ForceCreate)
    static ScAddr rrel_8;

    SC_PROPERTY(Keynode("rrel_9"), ForceCreate)
    static ScAddr rrel_9;

    SC_PROPERTY(Keynode("rrel_10"), ForceCreate)
    static ScAddr rrel_10;

    SC_PROPERTY(Keynode("order_role_relation"), ForceCreate)
    static ScAddr order_role_relation;

//Operators
    SC_PROPERTY(Keynode("scp_operator_atomic_type"), ForceCreate)
    static ScAddr scp_operator_atomic_type;

    SC_PROPERTY(Keynode("searchElStr3"), ForceCreate)
    static ScAddr op_searchElStr3;
    SC_PROPERTY(Keynode("searchElStr5"), ForceCreate)
    static ScAddr op_searchElStr5;

    SC_PROPERTY(Keynode("searchSetStr3"), ForceCreate)
    static ScAddr op_searchSetStr3;
    SC_PROPERTY(Keynode("searchSetStr5"), ForceCreate)
    static ScAddr op_searchSetStr5;

    SC_PROPERTY(Keynode("genEl"), ForceCreate)
    static ScAddr op_genEl;
    SC_PROPERTY(Keynode("genElStr3"), ForceCreate)
    static ScAddr op_genElStr3;
    SC_PROPERTY(Keynode("genElStr5"), ForceCreate)
    static ScAddr op_genElStr5;

    SC_PROPERTY(Keynode("eraseEl"), ForceCreate)
    static ScAddr op_eraseEl;
    SC_PROPERTY(Keynode("eraseElStr3"), ForceCreate)
    static ScAddr op_eraseElStr3;
    SC_PROPERTY(Keynode("eraseElStr5"), ForceCreate)
    static ScAddr op_eraseElStr5;
    SC_PROPERTY(Keynode("eraseSetStr3"), ForceCreate)
    static ScAddr op_eraseSetStr3;
    SC_PROPERTY(Keynode("eraseSetStr5"), ForceCreate)
    static ScAddr op_eraseSetStr5;

    SC_PROPERTY(Keynode("ifVarAssign"), ForceCreate)
    static ScAddr op_ifVarAssign;
    SC_PROPERTY(Keynode("ifFormCont"), ForceCreate)
    static ScAddr op_ifFormCont;
    SC_PROPERTY(Keynode("ifCoin"), ForceCreate)
    static ScAddr op_ifCoin;
    SC_PROPERTY(Keynode("ifType"), ForceCreate)
    static ScAddr op_ifType;

    SC_PROPERTY(Keynode("varAssign"), ForceCreate)
    static ScAddr op_varAssign;

#ifdef SCP_MATH
    SC_PROPERTY(Keynode("ifEq"), ForceCreate)
    static ScAddr op_ifEq;
    SC_PROPERTY(Keynode("ifGr"), ForceCreate)
    static ScAddr op_ifGr;

    SC_PROPERTY(Keynode("contAdd"), ForceCreate)
    static ScAddr op_contAdd;
    SC_PROPERTY(Keynode("contDiv"), ForceCreate)
    static ScAddr op_contDiv;
    SC_PROPERTY(Keynode("contMult"), ForceCreate)
    static ScAddr op_contMult;
    SC_PROPERTY(Keynode("contSub"), ForceCreate)
    static ScAddr op_contSub;
    SC_PROPERTY(Keynode("contPow"), ForceCreate)
    static ScAddr op_contPow;

    SC_PROPERTY(Keynode("contLn"), ForceCreate)
    static ScAddr op_contLn;
    SC_PROPERTY(Keynode("contSin"), ForceCreate)
    static ScAddr op_contSin;
    SC_PROPERTY(Keynode("contCos"), ForceCreate)
    static ScAddr op_contCos;
    SC_PROPERTY(Keynode("contTg"), ForceCreate)
    static ScAddr op_contTg;
    SC_PROPERTY(Keynode("contASin"), ForceCreate)
    static ScAddr op_contASin;
    SC_PROPERTY(Keynode("contACos"), ForceCreate)
    static ScAddr op_contACos;
    SC_PROPERTY(Keynode("contATg"), ForceCreate)
    static ScAddr op_contATg;

    SC_PROPERTY(Keynode("contDivInt"), ForceCreate)
    static ScAddr op_contDivInt;
    SC_PROPERTY(Keynode("contDivRem"), ForceCreate)
    static ScAddr op_contDivRem;
#endif
#ifdef SCP_STRING
    SC_PROPERTY(Keynode("contStringConcat"), ForceCreate)
    static ScAddr op_contStringConcat;
    SC_PROPERTY(Keynode("stringIfEq"), ForceCreate)
    static ScAddr op_stringIfEq;
    SC_PROPERTY(Keynode("stringIfGr"), ForceCreate)
    static ScAddr op_stringIfGr;
    SC_PROPERTY(Keynode("stringSplit"), ForceCreate)
    static ScAddr op_stringSplit;
    SC_PROPERTY(Keynode("stringLen"), ForceCreate)
    static ScAddr op_stringLen;
    SC_PROPERTY(Keynode("stringSub"), ForceCreate)
    static ScAddr op_stringSub;
    SC_PROPERTY(Keynode("stringSlice"), ForceCreate)
    static ScAddr op_stringSlice;
    SC_PROPERTY(Keynode("stringStartsWith"), ForceCreate)
    static ScAddr op_stringStartsWith;
    SC_PROPERTY(Keynode("stringEndsWith"), ForceCreate)
    static ScAddr op_stringEndsWith;
    SC_PROPERTY(Keynode("stringReplace"), ForceCreate)
    static ScAddr op_stringReplace;
    SC_PROPERTY(Keynode("stringToUpperCase"), ForceCreate)
    static ScAddr op_stringToUpperCase;
    SC_PROPERTY(Keynode("stringToLowerCase"), ForceCreate)
    static ScAddr op_stringToLowerCase;
#endif

    SC_PROPERTY(Keynode("contAssign"), ForceCreate)
    static ScAddr op_contAssign;
    SC_PROPERTY(Keynode("contErase"), ForceCreate)
    static ScAddr op_contErase;

    SC_PROPERTY(Keynode("print"), ForceCreate)
    static ScAddr op_print;
    SC_PROPERTY(Keynode("printNl"), ForceCreate)
    static ScAddr op_printNl;
    SC_PROPERTY(Keynode("printEl"), ForceCreate)
    static ScAddr op_printEl;

    SC_PROPERTY(Keynode("sys_search"), ForceCreate)
    static ScAddr op_sys_search;
    SC_PROPERTY(Keynode("sys_gen"), ForceCreate)
    static ScAddr op_sys_gen;

    SC_PROPERTY(Keynode("call"), ForceCreate)
    static ScAddr op_call;
    SC_PROPERTY(Keynode("waitReturn"), ForceCreate)
    static ScAddr op_waitReturn;
    SC_PROPERTY(Keynode("waitReturnSet"), ForceCreate)
    static ScAddr op_waitReturnSet;
    SC_PROPERTY(Keynode("return"), ForceCreate)
    static ScAddr op_return;

    SC_PROPERTY(Keynode("sys_wait"), ForceCreate)
    static ScAddr op_sys_wait;

    SC_PROPERTY(Keynode("syncronize"), ForceCreate)
    static ScAddr op_syncronize;

};

}
