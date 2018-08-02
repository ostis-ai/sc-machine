/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"

#include "scpOperatorSet.hpp"
#include <iostream>
#include <sstream>


namespace scp
{

SCPOperatorSetStr3::SCPOperatorSetStr3(ScMemoryContext &ctx_, ScAddr addr_): SCPOperator(ctx_, addr_)
{
    operands = std::vector<SCPOperand*>(3);
    set_operands = std::vector<SCPOperand*>(3);
}

sc_result SCPOperatorSetStr3::Parse()
{
    SCPOperator::Parse();
    ScIterator3Ptr iter_operator = ms_context.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType(0));
    while (iter_operator->Next())
    {
        SCPOperand *operand = new SCPOperand(ms_context, iter_operator->Get(1));
        if (operand->GetSetOrder() > 0)
        {
            if (!(operand->GetSetOrder() < 4 && set_operands[operand->GetSetOrder() - 1] == nullptr))
            {
#ifdef SCP_DEBUG
                std::stringstream ss;
                ss << "Invalid set operand order ";
                ss << (int)operand->GetSetOrder();
                Utils::logSCPError(ms_context, ss.str(), addr);
#endif
                FinishExecutionWithError();
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
            else
            {
                set_operands[operand->GetSetOrder() - 1] = operand;
            }
            continue;
        }

        if (!(operand->GetOrder() > 0 && operand->GetOrder() < 4 && operands[operand->GetOrder() - 1] == nullptr))
        {
#ifdef SCP_DEBUG
            std::stringstream ss;
            ss << "Invalid operand order ";
            ss << (int)operand->GetOrder();
            Utils::logSCPError(ms_context, ss.str(), addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            operands[operand->GetOrder() - 1] = operand;
        }
    }

    sc_result check = CheckNullValues();
    if (SC_RESULT_OK != check)
        return check;

    sc_bool has_set = SC_FALSE;
    for (int i = 0; i < 3; i++)
    {
        if (set_operands[i] != nullptr)
        {
            has_set = true;
            if (operands[i]->IsFixed())
            {
#ifdef SCP_DEBUG
                std::stringstream ss;
                ss << "SET operands must correspond to ASSIGN operands. Invalid operand ";
                ss << i + 1;
                Utils::logSCPError(ms_context, ss.str(), addr);
#endif
                FinishExecutionWithError();
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
        }
    }

    if (SC_TRUE == has_set)
        return SC_RESULT_OK;
    else
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "SetStr operator must have at least one rrel_set_X operand", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
}

SCPOperatorSetStr5::SCPOperatorSetStr5(ScMemoryContext &ctx_, ScAddr addr_): SCPOperator(ctx_, addr_)
{
    operands = std::vector<SCPOperand*>(5);
    set_operands = std::vector<SCPOperand*>(5);
}

sc_result SCPOperatorSetStr5::Parse()
{
    SCPOperator::Parse();
    ScIterator3Ptr iter_operator = ms_context.Iterator3(addr, ScType::EdgeAccessConstPosPerm, ScType(0));
    while (iter_operator->Next())
    {
        SCPOperand *operand = new SCPOperand(ms_context, iter_operator->Get(1));
        if (operand->GetSetOrder() > 0)
        {
            if (!(operand->GetSetOrder() < 6 && set_operands[operand->GetSetOrder() - 1] == nullptr))
            {
#ifdef SCP_DEBUG
                std::stringstream ss;
                ss << "Invalid set operand order ";
                ss << (int)operand->GetSetOrder();
                Utils::logSCPError(ms_context, ss.str(), addr);
#endif
                FinishExecutionWithError();
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
            else
            {
                set_operands[operand->GetSetOrder() - 1] = operand;
            }
            continue;
        }

        if (!(operand->GetOrder() > 0 && operand->GetOrder() < 6 && operands[operand->GetOrder() - 1] == nullptr))
        {
#ifdef SCP_DEBUG
            std::stringstream ss;
            ss << "Invalid operand order ";
            ss << (int)operand->GetOrder();
            Utils::logSCPError(ms_context, ss.str(), addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            operands[operand->GetOrder() - 1] = operand;
        }
    }

    sc_bool has_set = SC_FALSE;
    for (int i = 0; i < 5; i++)
    {
        if (set_operands[i] != nullptr)
        {
            has_set = true;
            if (operands[i]->IsFixed())
            {
#ifdef SCP_DEBUG
                std::stringstream ss;
                ss << "SET operands must correspond to ASSIGN operands. Invalid operand ";
                ss << i + 1;
                Utils::logSCPError(ms_context, ss.str(), addr);
#endif
                FinishExecutionWithError();
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
        }
    }

    if (SC_TRUE == has_set)
        return SC_RESULT_OK;
    else
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "SetStr operator must have at least one rrel_set_X operand", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
}

}

