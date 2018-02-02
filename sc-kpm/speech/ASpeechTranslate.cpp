/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "ASpeechTranslate.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>

namespace speech
{

ScAddr ASpeechTranslate::keynode_question_initiated;
ScAddr ASpeechTranslate::keynode_translation_result;
ScAddr ASpeechTranslate::keynode_nrel_semantic_equivalence;
ScAddr ASpeechTranslate::keynode_confidence_level;
ScAddr ASpeechTranslate::keynode_nrel_measurement;
ScAddr ASpeechTranslate::keynode_action_translate;
ScAddr ASpeechTranslate::keynode_rrel_1;
ScAddr ASpeechTranslate::keynode_nrel_basic_sequence;
ScAddr ASpeechTranslate::keynode_nrel_idtf;

float ASpeechTranslate::readFloat(ScMemoryContext &ms_context, ScAddr addr)
{
    ScStream stream;
    if (ms_context.GetLinkContent(addr, stream))
    {
        std::string str;
        if (ScStreamConverter::StreamToString(stream, str))
        {
            std::stringstream ss(str);
            float result;
            ss >> result;
            return result;
        }
    }
    return -1;
}

ScAddrList ASpeechTranslate::findConcepts(ScMemoryContext &ms_context, ScAddr name)
{
    ScAddrList result;
    ScStream content;
    if (!ms_context.GetLinkContent(name, content))
        return result;
    ScAddrList links;
    ms_context.FindLinksByContent(content, links);

    for (ScAddrList::const_iterator iterator = links.begin(), end = links.end(); iterator != end; ++iterator)
    {
        if (*iterator == name)
            continue;

        ScIterator5Ptr iter_idtf = ms_context.Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, *iterator, ScType::EdgeAccessConstPosPerm, keynode_nrel_idtf);
        while (iter_idtf->Next())
        {
            result.push_back(iter_idtf->Get(0));
        }
    }
    return result;
}

float ASpeechTranslate::findConfidence(ScMemoryContext &ms_context, ScAddr edge)
{
    ScIterator3Ptr iter_inp = ms_context.Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, edge);
    while (iter_inp->Next())
    {
        ScAddr confid = iter_inp->Get(0);
        if (ms_context.HelperCheckArc(keynode_confidence_level, confid, ScType::EdgeAccessConstPosPerm))
        {
            ScIterator5Ptr iter_value = ms_context.Iterator5(confid, ScType::EdgeDCommonConst, ScType::Link, ScType::EdgeAccessConstPosPerm, keynode_nrel_measurement);
            if (iter_value->Next())
            {
                return readFloat(ms_context, iter_value->Get(2));
            }
        }
    }

    return -1;
}

bool ASpeechTranslate::pair_comp(pair<ScAddr, float>* i, pair<ScAddr, float>* j)
{
    return (i->second > j->second);
}

SC_AGENT_IMPLEMENTATION(ASpeechTranslate)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);
    if (!ms_context->HelperCheckArc(keynode_action_translate, action, ScType::EdgeAccessConstPosPerm))
        return SC_RESULT_ERROR_INVALID_TYPE;

    ScIterator3Ptr iter_arg = ms_context->Iterator3(action, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    ScAddr src_struct;
    if (iter_arg->Next())
        src_struct = iter_arg->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr s_word;
    ScIterator5Ptr iter_src = ms_context->Iterator5(src_struct, ScType::EdgeAccessConstPosPerm, ScType::Link, ScType::EdgeAccessConstPosPerm, keynode_rrel_1);
    if (iter_src->Next())
        s_word = iter_src->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    while (true)
    {
        vector<pair<ScAddr, float>*> *vect = new vector<pair<ScAddr, float>*>();

        ScIterator5Ptr iter_pair = ms_context->Iterator5(s_word, ScType::EdgeUCommonConst, ScType::Link, ScType::EdgeAccessConstPosPerm, keynode_nrel_semantic_equivalence);
        while (iter_pair->Next())
        {
            if (!ms_context->HelperCheckArc(src_struct, iter_pair->Get(1), ScType::EdgeAccessConstPosPerm))
                continue;

            ScAddr p_word = iter_pair->Get(2);

            ScAddrList concepts = findConcepts((ScMemoryContext&)ms_context, p_word);
            float value = findConfidence((ScMemoryContext&)ms_context, iter_pair->Get(1));

            for (ScAddrList::const_iterator iterator = concepts.begin(), end = concepts.end(); iterator != end; ++iterator)
            {
                vect->push_back(new pair<ScAddr, float>(*iterator, value));
            }
        }

        if (vect->size() > 0)
            confid_map.push_back(vect);

        iter_src = ms_context->Iterator5(s_word, ScType::EdgeDCommonConst, ScType::Link, ScType::EdgeAccessConstPosPerm, keynode_nrel_basic_sequence);
        if (iter_src->Next())
            s_word = iter_src->Get(2);
        else
            break;
    }

    for (uint i = 0; i < confid_map.size(); i++)
    {
        sort(confid_map[i]->begin(), confid_map[i]->end(), pair_comp);
    }

    /*cout << "SIZE: " << confid_map.size() << endl;
    for (uint i = 0; i < confid_map.size(); i++)
    {
        cout << "\nWORD " << i << endl;
        for (uint j = 0; j < confid_map[i]->size(); j++)
        {
            cout << "\tPAIR " << j << " " << (*confid_map[i])[j]->first.Hash() << ":" << (*confid_map[i])[j]->second << endl;
        }
    }*/

    return SC_RESULT_OK;
}

ASpeechTranslate::~ASpeechTranslate()
{
    for (uint i = 0; i < confid_map.size(); i++)
    {
        for (uint j = 0; j < confid_map[i]->size(); j++)
        {
            delete (*confid_map[i])[j];
        }
        delete confid_map[i];
    }
}

} // namespace speech
