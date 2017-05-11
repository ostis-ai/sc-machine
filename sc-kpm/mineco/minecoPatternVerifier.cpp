/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

#include "minecoPatternVerifier.hpp"

namespace mineco
{
ScAddr AMinEcoPatternVerifier::msAgentKeynode;
ScAddr AMinEcoPatternVerifier::question_initiated;
ScAddr AMinEcoPatternVerifier::nrel_answer;
ScAddr AMinEcoPatternVerifier::question_pattern_verification;
ScAddr AMinEcoPatternVerifier::incorrect_structure_pattern;

SC_AGENT_IMPLEMENTATION(AMinEcoPatternVerifier)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);

    if (!ms_context->HelperCheckArc(question_pattern_verification, action, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr answer = ms_context->CreateNode(sc_type_const);

    ScIterator3Ptr iter_struct = ms_context->Iterator3(incorrect_structure_pattern, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_const | sc_type_node));
    while (iter_struct->Next())
    {
        ScAddr curr_struct = iter_struct->Get(2);
        ScTemplate templ;
        if (!(ms_context->HelperBuildTemplate(templ, curr_struct)))
        {
            continue;
        }
        ScTemplateSearchResult res;
        ms_context->HelperSearchTemplate(templ, res);
        if (res.Size() > 0)
        {
            ScAddr ans_struct = ms_context->CreateNode(sc_type_const | sc_type_node_struct);
            for (int i = 0; i < res.Size(); i++)
            {
                for (int j = 0; j < res[i].Size(); j++)
                {
                    ms_context->CreateArc(sc_type_arc_pos_const_perm, ans_struct, res[i][j]);
                }
            }
            ms_context->CreateArc(sc_type_arc_pos_const_perm, answer, ans_struct);
        }
    }

//finish action
    ScAddr myarc = ms_context->CreateArc(SC_TYPE(sc_type_arc_common | sc_type_const), action, answer);
    ms_context->CreateArc(sc_type_arc_pos_const_perm, nrel_answer, myarc);

    return SC_RESULT_OK;
}

}

