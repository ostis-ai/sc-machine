/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"

#include "scpOperatorStr.hpp"

namespace scp
{

SCPOperatorStr1::SCPOperatorStr1(ScMemoryContext &ctx, ScAddr addr): SCPOperator(ctx, addr)
{

}

sc_result SCPOperatorStr1::Parse()
{
    SCPOperator::Parse();
    ScIterator3Ptr iter_operator = ms_context.Iterator3(addr, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const));
    while (iter_operator->Next())
    {
        SCPOperand *operand = new SCPOperand(ms_context, iter_operator->Get(1));
        if (!(operand->GetOrder() == 1 && operands[0] == nullptr))
        {
            Utils::logSCPError(ms_context, "Invalid operand order " + operand->GetOrder(), addr);
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            operands[operand->GetOrder() - 1] = operand;
        }
    }
    return SC_RESULT_OK;
}

SCPOperatorStr3::SCPOperatorStr3(ScMemoryContext &ctx, ScAddr addr): SCPOperator(ctx, addr)
{

}

sc_result SCPOperatorStr3::Parse()
{
    SCPOperator::Parse();
    ScIterator3Ptr iter_operator = ms_context.Iterator3(addr, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const));
    while (iter_operator->Next())
    {
        SCPOperand *operand = new SCPOperand(ms_context, iter_operator->Get(1));
        if (!(operand->GetOrder() > 0 && operand->GetOrder() < 4 && operands[operand->GetOrder() - 1] == nullptr))
        {
            Utils::logSCPError(ms_context, "Invalid operand order " + operand->GetOrder(), addr);
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            operands[operand->GetOrder() - 1] = operand;
        }
    }
    return SC_RESULT_OK;
}

SCPOperatorStr5::SCPOperatorStr5(ScMemoryContext &ctx, ScAddr addr): SCPOperator(ctx, addr)
{

}

sc_result SCPOperatorStr5::Parse()
{
    SCPOperator::Parse();
    ScIterator3Ptr iter_operator = ms_context.Iterator3(addr, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const));
    while (iter_operator->Next())
    {
        SCPOperand *operand = new SCPOperand(ms_context, iter_operator->Get(1));
        if (!(operand->GetOrder() > 0 && operand->GetOrder() < 6 && operands[operand->GetOrder() - 1] == nullptr))
        {
            Utils::logSCPError(ms_context, "Invalid operand order " + operand->GetOrder(), addr);
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            operands[operand->GetOrder() - 1] = operand;
        }
    }
    return SC_RESULT_OK;
}

}

