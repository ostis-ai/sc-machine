/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "AVerificationRelation.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>

namespace speech
{
ScAddr AVerificationRelation::keynode_question_initiated;
ScAddr AVerificationRelation::keynode_action_verifying;
ScAddr AVerificationRelation::keynode_nrel_first_domain;
ScAddr AVerificationRelation::keynode_nrel_second_domain;
ScAddr AVerificationRelation::keynode_incorrect_structure;
ScAddr AVerificationRelation::keynode_action_finished;
ScAddr AVerificationRelation::keynode_nrel_inclusion;

bool AVerificationRelation::checkClass(ScAddr elem, ScAddr set)
{
    //!TODO rewrite to up-class search

    if (ms_context->HelperCheckArc(set, elem, ScType::EdgeAccessConstPosPerm))
        return true;

    ScIterator5Ptr iter_class = ms_context->Iterator5(set, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, keynode_nrel_inclusion);
    while (iter_class->Next())
    {
        ScAddr subclass = iter_class->Get(2);
        if (checkClass(elem, subclass))
            return true;
    }
    return false;
}

SC_AGENT_IMPLEMENTATION(AVerificationRelation)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr action = ms_context->GetArcEnd(edgeAddr);
    if (!ms_context->HelperCheckArc(keynode_action_verifying, action, ScType::EdgeAccessConstPosPerm))
        return SC_RESULT_ERROR_INVALID_TYPE;

    ScAddr src_struct;
    ScIterator3Ptr iter_arg = ms_context->Iterator3(action, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    if (iter_arg->Next())
        src_struct = iter_arg->Get(2);
    else
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScIterator3Ptr iter_elem = ms_context->Iterator3(src_struct, ScType::EdgeAccessConstPosPerm, ScType::NodeConstNoRole);
    bool flag = false;
    while (iter_elem->Next())
    {
        ScAddr rel = iter_elem->Get(2);

        ScAddr first_domain, second_domain;
        ScIterator5Ptr iter_domain = ms_context->Iterator5(rel, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, keynode_nrel_first_domain);
        if (iter_domain->Next())
            first_domain = iter_domain->Get(2);

        iter_domain = ms_context->Iterator5(rel, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, keynode_nrel_second_domain);
        if (iter_domain->Next())
            second_domain = iter_domain->Get(2);

        if (!first_domain.IsValid() && !second_domain.IsValid())
            continue;

        ScIterator5Ptr iter_tuple = ms_context->Iterator5(rel, ScType::EdgeAccessConstPosPerm, ScType::Const, ScType::EdgeAccessConstPosPerm, src_struct);
        while (iter_tuple->Next())
        {
            ScAddr arc = iter_tuple->Get(2);
            ScAddr first_node = ms_context->GetArcBegin(arc);
            ScAddr second_node = ms_context->GetArcEnd(arc);
            if (first_domain.IsValid() && !checkClass(first_node, first_domain))
            {
                ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_incorrect_structure, src_struct);
                ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_action_finished, action);
                return SC_RESULT_OK;
            }
            if (second_domain.IsValid() && !checkClass(second_node, second_domain))
            {
                ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_incorrect_structure, src_struct);
                ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_action_finished, action);
                return SC_RESULT_OK;
            }
            flag = true;
        }
    }

    if (flag)
        ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_action_finished, action);

    return SC_RESULT_OK;
}

} // namespace speech
