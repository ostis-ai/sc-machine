/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "ASpeechTranslate.generated.hpp"

#include<vector>

using namespace std;

namespace speech
{

class ASpeechTranslate : public ScAgent
{
    SC_CLASS(Agent, Event(keynode_question_initiated, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

    SC_PROPERTY(Keynode("question_initiated"), ForceCreate)
    static ScAddr keynode_question_initiated;

    SC_PROPERTY(Keynode("translation_result"), ForceCreate)
    static ScAddr keynode_translation_result;

    SC_PROPERTY(Keynode("nrel_semantic_equivalence"), ForceCreate)
    static ScAddr keynode_nrel_semantic_equivalence;

    SC_PROPERTY(Keynode("confidence_level"), ForceCreate)
    static ScAddr keynode_confidence_level;

    SC_PROPERTY(Keynode("nrel_measurement"), ForceCreate)
    static ScAddr keynode_nrel_measurement;

    SC_PROPERTY(Keynode("action_translate"), ForceCreate)
    static ScAddr keynode_action_translate;

    SC_PROPERTY(Keynode("rrel_1"), ForceCreate)
    static ScAddr keynode_rrel_1;

    SC_PROPERTY(Keynode("nrel_basic_sequence"), ForceCreate)
    static ScAddr keynode_nrel_basic_sequence;

    SC_PROPERTY(Keynode("nrel_idtf"), ForceCreate)
    static ScAddr keynode_nrel_idtf;

private:
    vector<vector<pair<ScAddr, float>*>*> confid_map;

    float readFloat(ScMemoryContext &ms_context, ScAddr addr);
    ScAddrList findConcepts(ScMemoryContext &ms_context, ScAddr name);
    float findConfidence(ScMemoryContext &ms_context, ScAddr edge);

    ~ASpeechTranslate();
};

} // namespace speech
