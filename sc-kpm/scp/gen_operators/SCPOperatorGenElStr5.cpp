/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorGenElStr5.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorGenElStr5::SCPOperatorGenElStr5(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr5(ctx, addr)
{
}

std::string SCPOperatorGenElStr5::GetTypeName()
{
    return "genElStr5";
}

sc_result SCPOperatorGenElStr5::Parse()
{
    return SCPOperatorElStr5::Parse();
}

sc_result SCPOperatorGenElStr5::Execute()
{
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    sc_uint32 type = 0;
    ScAddr elem1, elem3, elem5, arc1;
    elem1.Reset();
    elem3.Reset();
    elem5.Reset();
    arc1.Reset();

    if (operands[1]->IsFixed())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 2 must have ASSIGN modifier", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
    if (operands[3]->IsFixed())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 4 must have ASSIGN modifier", addr);
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
    if (operands[3]->GetType().IsNode())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 4 must have ARC type", addr);
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
        type = type | 0x10000;
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
        type = type | 0x00100;
    }
    if (operands[4]->IsFixed())
    {
        if (!operands[4]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand 5 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        type = type | 0x00001;
    }
    switch (type)
    {
    case 0x10101:
    {
        elem1 = operands[0]->GetValue();
        elem3 = operands[2]->GetValue();
        elem5 = operands[4]->GetValue();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x00101:
    {
        elem1 = operands[0]->CreateNodeOrLink();
        elem3 = operands[2]->GetValue();
        elem5 = operands[4]->GetValue();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x10001:
    {
        elem1 = operands[0]->GetValue();
        elem3 = operands[2]->CreateNodeOrLink();
        elem5 = operands[4]->GetValue();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x10100:
    {
        elem1 = operands[0]->GetValue();
        elem3 = operands[2]->GetValue();
        elem5 = operands[4]->CreateNodeOrLink();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x10000:
    {
        elem1 = operands[0]->GetValue();
        elem3 = operands[2]->CreateNodeOrLink();
        elem5 = operands[4]->CreateNodeOrLink();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x00100:
    {
        elem1 = operands[0]->CreateNodeOrLink();
        elem3 = operands[2]->GetValue();
        elem5 = operands[4]->CreateNodeOrLink();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x00001:
    {
        elem1 = operands[0]->CreateNodeOrLink();
        elem3 = operands[2]->CreateNodeOrLink();
        elem5 = operands[4]->GetValue();
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
        break;
    }
    case 0x00000:
    {
        if (operands[0]->GetType().IsLink())
            elem1 = ms_context.CreateLink();
        else
            elem1 = ms_context.CreateNode(operands[0]->GetType());
        if (operands[2]->GetType().IsLink())
            elem3 = ms_context.CreateLink();
        else
            elem3 = ms_context.CreateNode(operands[2]->GetType());
        if (operands[4]->GetType().IsLink())
            elem5 = ms_context.CreateLink();
        else
            elem5 = ms_context.CreateNode(operands[4]->GetType());
        operands[0]->SetValue(elem1);
        operands[2]->SetValue(elem3);
        operands[4]->SetValue(elem5);
        arc1 = ms_context.CreateArc(operands[1]->GetType(), elem1, elem3);
        operands[1]->SetValue(arc1);
        operands[3]->SetValue(ms_context.CreateArc(operands[3]->GetType(), elem5, arc1));
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
