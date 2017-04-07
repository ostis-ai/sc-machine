/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorGenElStr3.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorGenElStr3::SCPOperatorGenElStr3(ScMemoryContext &ctx, ScAddr addr): SCPOperatorStr3(ctx, addr)
{
}

std::string SCPOperatorGenElStr3::GetTypeName()
{
    return "genElStr3";
}

sc_result SCPOperatorGenElStr3::Parse()
{
    return SCPOperatorStr3::Parse();
}

sc_result SCPOperatorGenElStr3::Execute()
{
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    sc_uint32 type = 0;
    ScAddr elem1, elem3;
    elem1.Reset();
    elem3.Reset();

    if (operands[1]->IsFixed())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 2 must have ASSIGN modifier", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
    if (operands[1]->GetType().IsNode())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 2 must have ARC type", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
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
        elem1 = operands[0]->GetValue();
        elem3 = operands[2]->GetValue();
        operands[1]->SetValue(ms_context.CreateArc(operands[1]->GetType(), elem1, elem3));
        break;
    }
    case 0x001:
    {
        elem1 = operands[0]->CreateNodeOrLink();
        elem3 = operands[2]->GetValue();
        operands[1]->SetValue(ms_context.CreateArc(operands[1]->GetType(), elem1, elem3));
        break;
    }
    case 0x100:
    {
        elem1 = operands[0]->GetValue();
        elem3 = operands[2]->CreateNodeOrLink();
        operands[1]->SetValue(ms_context.CreateArc(operands[1]->GetType(), elem1, elem3));
        break;
    }
    case 0x000:
    {
        elem1 = operands[0]->CreateNodeOrLink();
        elem3 = operands[2]->CreateNodeOrLink();
        operands[1]->SetValue(ms_context.CreateArc(operands[1]->GetType(), elem1, elem3));
        break;
    }
    default:
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Unsupported operand type combination", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    FinishExecutionSuccessfully();
    return SC_RESULT_OK;
}

}
