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
    if (!edgeAddr.isValid())
        return SC_RESULT_ERROR;

    ScAddr process = msContext->getArcEnd(edgeAddr);

    if (!msContext->helperCheckArc(Keynodes::scp_process, process, sc_type_arc_pos_const_perm))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr decomp_node;
    ScIterator5Ptr it_temp = msContext->iterator5(SC_TYPE(sc_type_node | sc_type_const), SC_TYPE(sc_type_arc_common | sc_type_const), process, sc_type_arc_pos_const_perm, Keynodes::nrel_decomposition_of_action);
    if (it_temp->next())
        decomp_node = it_temp->value(0);
    else return SC_RESULT_ERROR_INVALID_STATE;

    ScIterator3Ptr it_oper = msContext->iterator3(decomp_node, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const));
    while (it_oper->next())
    {
        ScAddr curr_operator = it_oper->value(2);

        if (msContext->helperCheckArc(Keynodes::op_sys_search, curr_operator, sc_type_arc_pos_const_perm)
                || msContext->helperCheckArc(Keynodes::op_sys_gen, curr_operator, sc_type_arc_pos_const_perm))
        {
            ScIterator5Ptr it_operand = msContext->iterator5(curr_operator, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const), sc_type_arc_pos_const_perm, Keynodes::rrel_scp_const);
            while (it_operand->next())
            {
                if (!(msContext->helperCheckArc(Keynodes::rrel_2, it_operand->value(1), sc_type_arc_pos_const_perm)
                        || msContext->helperCheckArc(Keynodes::rrel_3, it_operand->value(1), sc_type_arc_pos_const_perm)))
                    continue;

                ScAddr curr_operand = it_operand->value(2);
                ScIterator3Ptr it_pairs = msContext->iterator3(curr_operand, sc_type_arc_pos_const_perm, SC_TYPE(0));
                while (it_pairs->next())
                {
                    ScAddr curr_pair = it_pairs->value(2);
                    deleteSCPVarsSet(curr_pair);
                    msContext->eraseElement(curr_pair);
                }
                msContext->eraseElement(curr_operand);
            }
        }
        if (msContext->helperCheckArc(Keynodes::op_call, curr_operator, sc_type_arc_pos_const_perm))
        {
            ScIterator5Ptr it_operand = msContext->iterator5(curr_operator, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const), sc_type_arc_pos_const_perm, Keynodes::rrel_2);
            if (it_operand->next())
            {
                if (msContext->helperCheckArc(Keynodes::rrel_scp_const, it_operand->value(1), sc_type_arc_pos_const_perm))
                {
                    ScAddr curr_operand = it_operand->value(2);
                    deleteSCPVarsSet(curr_operand);
                    msContext->eraseElement(curr_operand);
                }
            }
        }

        deleteSCPVarsSet(curr_operator);
        msContext->eraseElement(curr_operator);
    }

    msContext->eraseElement(decomp_node);
    msContext->eraseElement(process);

    ScAddr contur;
    msContext->helperFindBySystemIdtf("test_contour1", contur);
    Utils::printInfo((ScMemoryContext&)msContext, contur);

    return SC_RESULT_OK;
}

void ASCPProcessDestroyer::deleteSCPVarsSet(ScAddr & setAddr)
{
    ScIterator5Ptr it_operand = msContext->iterator5(setAddr, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const), sc_type_arc_pos_const_perm, Keynodes::rrel_scp_var);
    while (it_operand->next())
    {
        ScAddr elem = it_operand->value(2);
        msContext->eraseElement(elem);
    }
}

}
