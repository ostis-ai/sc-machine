/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorEraseElStr5.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorEraseElStr5::SCPOperatorEraseElStr5(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr5(ctx, addr)
{
}

std::string SCPOperatorEraseElStr5::GetTypeName()
{
    return "eraseElStr5";
}

sc_result SCPOperatorEraseElStr5::Parse()
{
    return SCPOperatorElStr5::Parse();
}

sc_result SCPOperatorEraseElStr5::Execute()
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
        type = type | 0x10000;
    }
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
    if (operands[3]->IsFixed())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand 4 must have ASSIGN modifier", addr);
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
    case 0x10000:
    {
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetType(), operands[3]->GetType(), operands[4]->GetType());
        if (iter->Next())
        {
            if (operands[2]->IsErase())
            {
                ms_context.EraseElement(iter->Get(2));
                if (operands[4]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(4));
                }
            }
            else
            {
                if (operands[1]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(1));
                    if (operands[4]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(4));
                    }
                }
                else
                {
                    if (operands[4]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(4));
                    }
                    else
                    {
                        if (operands[3]->IsErase())
                        {
                            ms_context.EraseElement(iter->Get(3));
                        }
                    }
                }

            }
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x00001:
    {
        ScIterator3Ptr iter = ms_context.Iterator3(operands[4]->GetValue(), operands[3]->GetType(), operands[1]->GetType());
        bool flag = false;
        while (iter->Next())
        {
            ScAddr elem1 = ms_context.GetEdgeSource(iter->Get(2));
            ScAddr elem3 = ms_context.GetEdgeTarget(iter->Get(2));
            ScType type1 = ms_context.GetElementType(elem1);
            ScType type3 = ms_context.GetElementType(elem3);
            if (((type1 & operands[0]->GetType()) == operands[0]->GetType()) && (((type3 & operands[2]->GetType()) == operands[2]->GetType())))
            {
                if (operands[0]->IsErase())
                {
                    ms_context.EraseElement(elem1);
                    if (operands[2]->IsErase())
                    {
                        ms_context.EraseElement(elem3);
                    }
                }
                else
                {
                    if (operands[2]->IsErase())
                    {
                        ms_context.EraseElement(elem3);
                    }
                    else
                    {
                        if (operands[1]->IsErase())
                        {
                            ms_context.EraseElement(iter->Get(2));
                        }
                        else
                        {
                            if (operands[3]->IsErase())
                            {
                                ms_context.EraseElement(iter->Get(1));
                            }
                        }
                    }
                }
                FinishExecutionSuccessfully();
                flag = true;
                break;
            }
            else
                continue;
        }
        if (!flag)
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x00100:
    {
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetType(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetType());
        if (iter->Next())
        {
            if (operands[0]->IsErase())
            {
                ms_context.EraseElement(iter->Get(0));
                if (operands[4]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(4));
                }
            }
            else
            {
                if (operands[1]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(1));
                    if (operands[4]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(4));
                    }
                }
                else
                {
                    if (operands[4]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(4));
                    }
                    else
                    {
                        if (operands[3]->IsErase())
                        {
                            ms_context.EraseElement(iter->Get(3));
                        }
                    }
                }
            }
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x10100:
    {
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetType());
        if (iter->Next())
        {
            if (operands[1]->IsErase())
            {
                ms_context.EraseElement(iter->Get(1));
                if (operands[4]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(4));
                }
            }
            else
            {
                if (operands[4]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(4));
                }
                else
                {
                    if (operands[3]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(3));
                    }
                }
            }
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x00101:
    {
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetType(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetValue());
        if (iter->Next())
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
                else
                {
                    if (operands[3]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(3));
                    }
                }
            }
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x10001:
    {
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetType(), operands[3]->GetType(), operands[4]->GetValue());
        if (iter->Next())
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
                else
                {
                    if (operands[3]->IsErase())
                    {
                        ms_context.EraseElement(iter->Get(3));
                    }
                }
            }
            FinishExecutionSuccessfully();
        }
        else
        {
            FinishExecutionUnsuccessfully();
        }
        break;
    }
    case 0x10101:
    {
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetValue());
        if (iter->Next())
        {
            if (operands[1]->IsErase())
            {
                ms_context.EraseElement(iter->Get(1));
            }
            else
            {
                if (operands[3]->IsErase())
                {
                    ms_context.EraseElement(iter->Get(3));
                }
            }
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
