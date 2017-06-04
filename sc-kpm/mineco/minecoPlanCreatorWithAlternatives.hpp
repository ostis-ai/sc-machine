/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "minecoPlanCreatorWithAlternatives.generated.hpp"

namespace mineco
{

class AMinEcoPlanCreatorWithAlternatives : public ScAgent
{
    SC_CLASS(Agent, Event(question_initiated, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("sc_agent_of_mineco_plan_creation_with_alternatives"), ForceCreate)
    static ScAddr msAgentKeynode;

private:
    SC_PROPERTY(Keynode("rrel_1"), ForceCreate)
    static ScAddr rrel_1;

    SC_PROPERTY(Keynode("rrel_2"), ForceCreate)
    static ScAddr rrel_2;

    SC_PROPERTY(Keynode("rrel_key_sc_element"), ForceCreate)
    static ScAddr rrel_key_sc_element;

    SC_PROPERTY(Keynode("question_plan_creation"), ForceCreate)
    static ScAddr question_plan_creation;

    SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
    static ScAddr question_initiated;

    SC_PROPERTY(Keynode("nrel_answer"), ForceCreate)
    static ScAddr nrel_answer;

    SC_PROPERTY(Keynode("nrel_inclusion"), ForceCreate)
    static ScAddr nrel_inclusion;

    SC_PROPERTY(Keynode("nrel_goto"), ForceCreate)
    static ScAddr nrel_goto;

    SC_PROPERTY(Keynode("nrel_then"), ForceCreate)
    static ScAddr nrel_then;

    SC_PROPERTY(Keynode("nrel_else"), ForceCreate)
    static ScAddr nrel_else;

    SC_PROPERTY(Keynode("nrel_authors"), ForceCreate)
    static ScAddr nrel_authors;

    SC_PROPERTY(Keynode("nrel_result"), ForceCreate)
    static ScAddr nrel_result;

    SC_PROPERTY(Keynode("nrel_object"), ForceCreate)
    static ScAddr nrel_object;

    SC_PROPERTY(Keynode("action_select_one_from_alternatives"), ForceCreate)
    static ScAddr action_select_one_from_alternatives;

    SC_PROPERTY(Keynode("nrel_decomposition_of_action"), ForceCreate)
    static ScAddr nrel_decomposition_of_action;

    SC_PROPERTY(Keynode("action.select_from_alternatives"), ForceCreate)
    static ScAddr action_select_from_alternatives;

    SC_PROPERTY(Keynode("nrel_transition_condition"), ForceCreate)
    static ScAddr nrel_transition_condition;

    SC_PROPERTY(Keynode("conditional_transfer"), ForceCreate)
    static ScAddr conditional_transfer;

    void RemoveSet(ScAddr set);
    void RemoveFirstLevelELements(ScAddr set);
    ScAddr GetFirstSubaction(ScAddr action);
    ScAddr GetTransferCondition(ScAddr action_arc);
    ScAddr SelectFromAlternative(ScAddr main_class, ScAddr user);
    sc_bool IsSelection(ScAddr action);
    void RemoveBranch(ScAddr action, ScAddr set_action);

};

}

