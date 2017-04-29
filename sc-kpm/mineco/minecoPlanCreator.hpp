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
    SC_CLASS(Agent, Event(question_initiated, SC_EVENT_ADD_OUTPUT_ARC))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("sc_agent_of_mineco_plan_creation"), ForceCreate)
    static ScAddr msAgentKeynode;

    SC_PROPERTY(Keynode("question_plan_creation"), ForceCreate)
    static ScAddr question_plan_creation;

    SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
    static ScAddr question_initiated;

    SC_PROPERTY(Keynode("nrel_answer"), ForceCreate)
    static ScAddr nrel_answer;

};

}

