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

    SC_PROPERTY(Keynode("scp_process"), ForceCreate)
    static ScAddr scp_process;

    SC_PROPERTY(Keynode("abstract_scp_machine"), ForceCreate)
    static ScAddr abstract_scp_machine;

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

    SC_PROPERTY(Keynode("rrel_not_relation"), ForceCreate)
    static ScAddr rrel_not_relation;

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
};

}
