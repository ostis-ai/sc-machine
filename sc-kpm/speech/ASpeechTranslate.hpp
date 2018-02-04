/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "sc-memory/cpp/sc_struct.hpp"

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

    SC_PROPERTY(Keynode("nrel_object"), ForceCreate)
    static ScAddr keynode_nrel_object;

    SC_PROPERTY(Keynode("nrel_performer"), ForceCreate)
    static ScAddr keynode_nrel_subject;

    SC_PROPERTY(Keynode("incorrect_structure"), ForceCreate)
    static ScAddr keynode_incorrect_structure;

    SC_PROPERTY(Keynode("question_finished"), ForceCreate)
    static ScAddr keynode_action_finished;

    SC_PROPERTY(Keynode("action_of_verifying_the_desired_structure"), ForceCreate)
    static ScAddr keynode_action_verifying;

private:
    vector<vector<pair<ScAddr, float>*>*> confid_map;
    ScAddrVector source_triple;
    vector<int> triple_mask{-1, 0, 0};
    ScStruct translation_result = ScStruct(&((ScMemoryContext&)ms_context), ms_context->CreateNode(ScType::NodeConstStruct));

    static bool pair_comp(pair<ScAddr, float>* i, pair<ScAddr, float>* j);
    static float readFloat(ScMemoryContext &ms_context, ScAddr addr);
    static ScAddrList findConcepts(ScMemoryContext &ms_context, ScAddr name);

    float findConfidence(ScMemoryContext &ms_context, ScAddr edge);
    ScAddr makeClassInstance(ScAddr elem);
    void clearTranslation();
    bool nextTriple();
    bool translateAsAction();
    bool translateAsRelation();
    void translate();
    bool verify();

    ~ASpeechTranslate();
};

} // namespace speech
