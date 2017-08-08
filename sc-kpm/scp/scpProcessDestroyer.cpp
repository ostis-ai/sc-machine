/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "scpProcessDestroyer.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{
ScAddr ASCPProcessDestroyer::msAgentKeynode;

SC_AGENT_IMPLEMENTATION(ASCPProcessDestroyer)
{
    if (!edgeAddr.IsValid())
        return SC_RESULT_ERROR;

    ScAddr process = ms_context->GetArcEnd(edgeAddr);

    if (!ms_context->HelperCheckArc(Keynodes::scp_process, process, ScType::EdgeAccessConstPosPerm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr decomp_node;
    ScIterator5Ptr it_temp = ms_context->Iterator5(ScType::NodeConst, ScType::EdgeDCommonConst, process, ScType::EdgeAccessConstPosPerm, Keynodes::nrel_decomposition_of_action);
    if (it_temp->Next())
        decomp_node = it_temp->Get(0);
    else return SC_RESULT_ERROR_INVALID_STATE;

    ScIterator3Ptr it_oper = ms_context->Iterator3(decomp_node, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    while (it_oper->Next())
    {
        ScAddr curr_operator = it_oper->Get(2);

        if (ms_context->HelperCheckArc(Keynodes::op_sys_search, curr_operator, ScType::EdgeAccessConstPosPerm)
                || ms_context->HelperCheckArc(Keynodes::op_sys_gen, curr_operator, ScType::EdgeAccessConstPosPerm))
        {
            ScIterator5Ptr it_operand = ms_context->Iterator5(curr_operator, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_scp_const);
            while (it_operand->Next())
            {
                if (!(ms_context->HelperCheckArc(Keynodes::rrel_2, it_operand->Get(1), ScType::EdgeAccessConstPosPerm)
                        || ms_context->HelperCheckArc(Keynodes::rrel_3, it_operand->Get(1), ScType::EdgeAccessConstPosPerm)))
                    continue;

                ScAddr curr_operand = it_operand->Get(2);
                ScIterator3Ptr it_pairs = ms_context->Iterator3(curr_operand, ScType::EdgeAccessConstPosPerm, ScType(0));
                while (it_pairs->Next())
                {
                    ScAddr curr_pair = it_pairs->Get(2);
                    deleteSCPVarsSet(curr_pair);
                    ms_context->EraseElement(curr_pair);
                }
                ms_context->EraseElement(curr_operand);
            }
        }
        if (ms_context->HelperCheckArc(Keynodes::op_call, curr_operator, ScType::EdgeAccessConstPosPerm))
        {
            ScIterator5Ptr it_operand = ms_context->Iterator5(curr_operator, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_2);
            if (it_operand->Next())
            {
                if (ms_context->HelperCheckArc(Keynodes::rrel_scp_const, it_operand->Get(1), ScType::EdgeAccessConstPosPerm))
                {
                    ScAddr curr_operand = it_operand->Get(2);
                    deleteSCPVarsSet(curr_operand);
                    ms_context->EraseElement(curr_operand);
                }
            }
        }

        deleteSCPVarsSet(curr_operator);
        ms_context->EraseElement(curr_operator);
    }

    ms_context->EraseElement(decomp_node);
    ms_context->EraseElement(process);

    ScAddr contur;
    ms_context->HelperFindBySystemIdtf("test_contour1", contur);
    Utils::printInfo((ScMemoryContext&)ms_context, contur);

    return SC_RESULT_OK;
}

void ASCPProcessDestroyer::deleteSCPVarsSet(ScAddr & setAddr)
{
    ScIterator5Ptr it_operand = ms_context->Iterator5(setAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_scp_var);
    while (it_operand->Next())
    {
        ScAddr elem = it_operand->Get(2);
        ms_context->EraseElement(elem);
    }
}

}
