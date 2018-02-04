/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "AVerificationAction.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>

namespace speech
{
ScAddr AVerificationAction::keynode_question_initiated;
ScAddr AVerificationAction::keynode_action_verifying;
ScAddr AVerificationAction::keynode_nrel_performer_class;
ScAddr AVerificationAction::keynode_nrel_object_class;
ScAddr AVerificationAction::keynode_incorrect_structure;
ScAddr AVerificationAction::keynode_action_finished;
ScAddr AVerificationAction::keynode_nrel_inclusion;
ScAddr AVerificationAction::keynode_action;
ScAddr AVerificationAction::keynode_nrel_performer;
ScAddr AVerificationAction::keynode_nrel_object;

bool AVerificationAction::checkClass(ScAddr elem, ScAddr set, ScAddr *superclass)
{
    //!TODO rewrite to up-class search

    if (ms_context->HelperCheckArc(set, elem, ScType::EdgeAccessConstPosPerm))
    {
        (*superclass) = set;
        return true;
    }

    ScIterator5Ptr iter_class = ms_context->Iterator5(set, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, keynode_nrel_inclusion);
    while (iter_class->Next())
    {
        ScAddr subclass = iter_class->Get(2);
        if (checkClass(elem, subclass, superclass))
            return true;
    }
    return false;
}

SC_AGENT_IMPLEMENTATION(AVerificationAction)
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


    ScIterator3Ptr iter_elem = ms_context->Iterator3(src_struct, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    bool flag = false;
    while (iter_elem->Next())
    {
        ScAddr act_node = iter_elem->Get(2);
        ScAddr act_class;
        if (!checkClass(act_node, keynode_action, &act_class))
            continue;

        ScAddr first_domain, second_domain;
        ScIterator5Ptr iter_domain = ms_context->Iterator5(act_class, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, keynode_nrel_performer_class);
        if (iter_domain->Next())
            first_domain = iter_domain->Get(2);

        iter_domain = ms_context->Iterator5(act_class, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, keynode_nrel_object_class);
        if (iter_domain->Next())
            second_domain = iter_domain->Get(2);

        if (!first_domain.IsValid() && !second_domain.IsValid())
            continue;

        ScIterator5Ptr iter_args = ms_context->Iterator5(act_node, ScType::EdgeDCommonConst, ScType::Const, ScType::EdgeAccessConstPosPerm, keynode_nrel_performer);
        if (iter_args->Next())
        {
            ScAddr performer = iter_args->Get(2);
            ScAddr domain_class;
            if (first_domain.IsValid() && !checkClass(performer, first_domain, &domain_class))
            {
                ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_incorrect_structure, src_struct);
                ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, keynode_action_finished, action);
                return SC_RESULT_OK;
            }
            flag = true;
        }

        iter_args = ms_context->Iterator5(act_node, ScType::EdgeDCommonConst, ScType::Const, ScType::EdgeAccessConstPosPerm, keynode_nrel_object);
        if (iter_args->Next())
        {
            ScAddr object = iter_args->Get(2);
            ScAddr domain_class;
            if (second_domain.IsValid() && !checkClass(object, second_domain, &domain_class))
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
