/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorSearchSetStr3.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorSearchSetStr3::SCPOperatorSearchSetStr3(ScMemoryContext &ctx, ScAddr addr): SCPOperatorSetStr3(ctx, addr)
{
}

std::string SCPOperatorSearchSetStr3::GetTypeName()
{
    return "searchSetStr3";
}

sc_result SCPOperatorSearchSetStr3::Parse()
{
    return SCPOperatorSetStr3::Parse();
}

sc_result SCPOperatorSearchSetStr3::Execute()
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
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 2 must have ASSIGN modifier", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
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

    if (set_operands[0] != nullptr && set_operands[0]->IsFixed())
    {
        if (!set_operands[0]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand SET 1 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }
    if (set_operands[1] != nullptr && set_operands[1]->IsFixed())
    {
        if (!set_operands[1]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand SET 2 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }
    if (set_operands[2] != nullptr && set_operands[2]->IsFixed())
    {
        if (!set_operands[2]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand SET 3 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }

    switch (type)
    {
    case 0x101:
    {
        ScAddr set1;

        sc_bool found = SC_FALSE;
        ScIterator3Ptr iter = ms_context.Iterator3(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetValue());
        while (iter->Next())
        {
            if (SC_FALSE == found)
            {
                if (set_operands[1]->IsFixed())
                {
                    set1 = set_operands[1]->GetValue();
                }
                else
                {
                    set1 = ms_context.CreateNode(ScType::NodeConst);
                    set_operands[1]->SetValue(set1);
                }
                operands[1]->SetValue(iter->Get(1));
                found = SC_TRUE;
            }
            ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set1, iter->Get(1));
        }

        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x001:
    {
        ScAddr set0, set1;

        sc_bool found = SC_FALSE;
        ScIterator3Ptr iter = ms_context.Iterator3(operands[0]->GetType(), operands[1]->GetType(), operands[2]->GetValue());
        while (iter->Next())
        {
            if (set_operands[0] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[0]->IsFixed())
                    {
                        set0 = set_operands[0]->GetValue();
                    }
                    else
                    {
                        set0 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[0]->SetValue(set0);
                    }
                    operands[0]->SetValue(iter->Get(0));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set0, iter->Get(0));
            }
            if (set_operands[1] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[1]->IsFixed())
                    {
                        set1 = set_operands[1]->GetValue();
                    }
                    else
                    {
                        set1 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[1]->SetValue(set1);
                    }
                    operands[1]->SetValue(iter->Get(1));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set1, iter->Get(1));
            }
            found = SC_TRUE;
        }

        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x100:
    {
        ScAddr set1, set2;

        sc_bool found = SC_FALSE;
        ScIterator3Ptr iter = ms_context.Iterator3(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetType());
        while (iter->Next())
        {
            if (set_operands[1] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[1]->IsFixed())
                    {
                        set1 = set_operands[1]->GetValue();
                    }
                    else
                    {
                        set1 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[1]->SetValue(set1);
                    }
                    operands[1]->SetValue(iter->Get(1));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set1, iter->Get(1));
            }
            if (set_operands[2] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[2]->IsFixed())
                    {
                        set2 = set_operands[2]->GetValue();
                    }
                    else
                    {
                        set2 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[2]->SetValue(set2);
                    }
                    operands[2]->SetValue(iter->Get(2));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set2, iter->Get(2));
            }
            found = SC_TRUE;
        }

        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
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
