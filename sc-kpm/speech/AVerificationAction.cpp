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

bool AVerificationAction::checkClass(ScAddr elem, ScAddr set)
{
    if (ms_context->HelperCheckArc(set, elem, ScType::EdgeAccessConstPosPerm))
        return true;

    ScIterator5Ptr iter_class = ms_context->Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, set, ScType::EdgeAccessConstPosPerm, keynode_nrel_inclusion);
    while (iter_class->Next())
    {
        ScAddr superclass = iter_class->Get(0);
        if (checkClass(superclass, elem))
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



    return SC_RESULT_OK;
}

} // namespace speech
