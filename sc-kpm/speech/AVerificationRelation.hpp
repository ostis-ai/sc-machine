/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "sc-memory/cpp/sc_struct.hpp"

#include "AVerificationRelation.generated.hpp"

#include<vector>

using namespace std;

namespace speech
{

class AVerificationRelation : public ScAgent
{
    SC_CLASS(Agent, Event(keynode_question_initiated, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

    SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
    static ScAddr keynode_question_initiated;

    SC_PROPERTY(Keynode("action_of_verifying_the_desired_structure"), ForceCreate)
    static ScAddr keynode_action_verifying;

    SC_PROPERTY(Keynode("nrel_first_domain"), ForceCreate)
    static ScAddr keynode_nrel_first_domain;

    SC_PROPERTY(Keynode("nrel_second_domain"), ForceCreate)
    static ScAddr keynode_nrel_second_domain;

    SC_PROPERTY(Keynode("incorrect_structure"), ForceCreate)
    static ScAddr keynode_incorrect_structure;

    SC_PROPERTY(Keynode("question_finished"), ForceCreate)
    static ScAddr keynode_action_finished;

    SC_PROPERTY(Keynode("nrel_inclusion"), ForceCreate)
    static ScAddr keynode_nrel_inclusion;

private:
    bool checkClass(ScAddr elem, ScAddr set);

};

} // namespace speech
