/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorSearchElStr3.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorSearchElStr3::SCPOperatorSearchElStr3(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr3(ctx, addr)
{
}

std::string SCPOperatorSearchElStr3::GetTypeName()
{
    return "searchElStr3";
}

sc_result SCPOperatorSearchElStr3::Parse()
{
    return SCPOperatorElStr3::Parse();
}

sc_result SCPOperatorSearchElStr3::Execute()
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
        if (iter->Next())
        {
            operands[1]->SetValue(iter->Get(1));
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
        if (iter->Next())
        {
            operands[0]->SetValue(iter->Get(0));
            operands[1]->SetValue(iter->Get(1));
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
        if (iter->Next())
        {
            operands[1]->SetValue(iter->Get(1));
            operands[2]->SetValue(iter->Get(2));
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x010:
    {
        ScAddr elem1 = ms_context.GetEdgeSource(operands[1]->GetValue());
        ScAddr elem3 = ms_context.GetEdgeTarget(operands[1]->GetValue());
        ScType type1 = ms_context.GetElementType(elem1);
        ScType type3 = ms_context.GetElementType(elem3);
        if (((type1 & operands[0]->GetType()) == operands[0]->GetType()) && (((type3 & operands[2]->GetType()) == operands[2]->GetType())))
        {
            operands[0]->SetValue(elem1);
            operands[2]->SetValue(elem3);
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x110:
    {
        ScAddr elem1 = ms_context.GetEdgeSource(operands[1]->GetValue());
        ScAddr elem3 = ms_context.GetEdgeTarget(operands[1]->GetValue());
        ScType type3 = ms_context.GetElementType(elem3);
        if (elem1 == operands[0]->GetValue() && (type3 & operands[2]->GetType()) == operands[2]->GetType())
        {
            operands[2]->SetValue(elem3);
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x011:
    {
        ScAddr elem1 = ms_context.GetEdgeSource(operands[1]->GetValue());
        ScAddr elem3 = ms_context.GetEdgeTarget(operands[1]->GetValue());
        ScType type1 = ms_context.GetElementType(elem1);
        if ((type1 & operands[0]->GetType()) == operands[0]->GetType() && elem3 == operands[2]->GetValue())
        {
            operands[0]->SetValue(elem1);
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x111:
    {
        ScAddr elem1 = ms_context.GetEdgeSource(operands[1]->GetValue());
        ScAddr elem3 = ms_context.GetEdgeTarget(operands[1]->GetValue());
        if (elem1 == operands[0]->GetValue() && elem3 == operands[2]->GetValue())
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
