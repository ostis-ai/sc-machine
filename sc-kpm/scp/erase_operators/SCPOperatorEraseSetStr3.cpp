/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorEraseSetStr3.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorEraseSetStr3::SCPOperatorEraseSetStr3(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr3(ctx, addr)
{
}

std::string SCPOperatorEraseSetStr3::GetTypeName()
{
    return "eraseSetStr3";
}

sc_result SCPOperatorEraseSetStr3::Parse()
{
    return SCPOperatorElStr3::Parse();
}

sc_result SCPOperatorEraseSetStr3::Execute()
{
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    sc_uint32 type = 0;

    if (operands[0]->IsFixed())
    {
        if (!operands[0]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand 1 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        type = type | 0x100;
    }
    if (operands[1]->IsFixed())
    {
        if (!operands[1]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand 2 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        type = type | 0x010;
    }
    else
    {
        if (operands[1]->GetType().IsNode())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand 2 must have ARC type", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }
    if (operands[2]->IsFixed())
    {
        if (!operands[2]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand 3 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        type = type | 0x001;
    }

    switch (type)
    {
    case 0x101:
    {
        ScIterator3Ptr iter = ms_context.Iterator3(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetValue());
        sc_bool flag = SC_FALSE;
        while (iter->Next())
        {
            if (operands[1]->IsErase())
            {
                ms_context.EraseElement(iter->Get(1));
            }
            flag = SC_TRUE;
        }
        if (flag)
        {
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x001:
    {
        ScIterator3Ptr iter = ms_context.Iterator3(operands[0]->GetType(), operands[1]->GetType(), operands[2]->GetValue());
        sc_bool flag = SC_FALSE;
        while (iter->Next())
        {
            if (operands[0]->IsErase())
            {
                ms_context.EraseElement(iter->Get(0));
            }
            else
            {
                if (operands[1]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(1));
                }
            }
            flag = SC_TRUE;
        }
        if (flag)
        {
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x100:
    {
        ScIterator3Ptr iter = ms_context.Iterator3(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetType());
        sc_bool flag = SC_FALSE;
        while (iter->Next())
        {
            if (operands[2]->IsErase())
            {
                ms_context.EraseElement(iter->Get(2));
            }
            else
            {
                if (operands[1]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(1));
                }
            }
            flag = SC_TRUE;
        }
        if (flag)
        {
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    default:
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Unsupported operand type combination", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
    return SC_RESULT_OK;
}

}
