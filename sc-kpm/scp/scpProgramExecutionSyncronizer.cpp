/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpUtils.hpp"
#include "scpProgramExecutionSyncronizer.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{
ScAddr ASCPProgramExecutionSyncronizer::msAgentKeynode;

SC_AGENT_IMPLEMENTATION(ASCPProgramExecutionSyncronizer)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr scp_operator = ms_context->GetArcEnd(edgeAddr);

    if (ms_context->HelperCheckArc(Keynodes::question_finished_with_error, scp_operator, ScType::EdgeAccessConstPosPerm))
    {
        InitOperatorsByRelation(scp_operator, Keynodes::nrel_error);
        return SC_RESULT_OK;
    }
    if (ms_context->HelperCheckArc(Keynodes::question_finished_successfully, scp_operator, ScType::EdgeAccessConstPosPerm))
    {
        InitOperatorsByRelation(scp_operator, Keynodes::nrel_then);
        InitOperatorsByRelation(scp_operator, Keynodes::nrel_goto);
        return SC_RESULT_OK;
    }
    if (ms_context->HelperCheckArc(Keynodes::question_finished_unsuccessfully, scp_operator, ScType::EdgeAccessConstPosPerm))
    {
        InitOperatorsByRelation(scp_operator, Keynodes::nrel_else);
        InitOperatorsByRelation(scp_operator, Keynodes::nrel_goto);
        return SC_RESULT_OK;
    }

    return SC_RESULT_OK;
}

void ASCPProgramExecutionSyncronizer::InitOperatorsByRelation(ScAddr &scp_operator, ScAddr &relation)
{
    ScIterator5Ptr iter_error = ms_context->Iterator5(scp_operator, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, relation);
    while (iter_error->Next())
    {
        ScAddr next_op = iter_error->Get(2);
        ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::active_action, next_op);
    }
}

}
