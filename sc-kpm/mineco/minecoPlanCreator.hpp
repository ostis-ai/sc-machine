/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "minecoPlanCreator.generated.hpp"

namespace mineco
{

class AMinEcoPlanCreator : public ScAgent
{
    SC_CLASS(Agent, Event(question_initiated, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("sc_agent_of_mineco_plan_creation"), ForceCreate)
    static ScAddr msAgentKeynode;

private:
    SC_PROPERTY(Keynode("rrel_1"), ForceCreate)
    static ScAddr rrel_1;

    SC_PROPERTY(Keynode("rrel_2"), ForceCreate)
    static ScAddr rrel_2;

    SC_PROPERTY(Keynode("concept_organization"), ForceCreate)
    static ScAddr concept_organization;

    SC_PROPERTY(Keynode("administrative_procedure.receiving_and_registering_book_of_comments_and_suggestions"), ForceCreate)
    static ScAddr ap_receiving_and_registering_book;

    SC_PROPERTY(Keynode("concept_store_opening"), ForceCreate)
    static ScAddr concept_store_opening;

    SC_PROPERTY(Keynode("question_plan_creation"), ForceCreate)
    static ScAddr question_plan_creation;

    SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
    static ScAddr question_initiated;

    SC_PROPERTY(Keynode("nrel_answer"), ForceCreate)
    static ScAddr nrel_answer;

    SC_PROPERTY(Keynode("nrel_goto"), ForceCreate)
    static ScAddr nrel_goto;

    SC_PROPERTY(Keynode("nrel_decomposition_of_action"), ForceCreate)
    static ScAddr nrel_decomposition_of_action;

    SC_PROPERTY(Keynode("nrel_address"), ForceCreate)
    static ScAddr nrel_address;

    SC_PROPERTY(Keynode("nrel_legal_address"), ForceCreate)
    static ScAddr nrel_legal_address;

    SC_PROPERTY(Keynode("nrel_administrative_subordination"), ForceCreate)
    static ScAddr nrel_administrative_subordination;

    SC_PROPERTY(Keynode("nrel_registration"), ForceCreate)
    static ScAddr nrel_registration;

    SC_PROPERTY(Keynode("inspection_MNS"), ForceCreate)
    static ScAddr inspection_MNS;

    void AddRegistrationPlace(ScAddr action, ScAddr organization, ScAddr answer);
};

}

