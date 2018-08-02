/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorSearchSetStr5.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorSearchSetStr5::SCPOperatorSearchSetStr5(ScMemoryContext &ctx, ScAddr addr): SCPOperatorSetStr5(ctx, addr)
{
}

std::string SCPOperatorSearchSetStr5::GetTypeName()
{
    return "searchSetStr5";
}

sc_result SCPOperatorSearchSetStr5::Parse()
{
    return SCPOperatorSetStr5::Parse();
}

sc_result SCPOperatorSearchSetStr5::Execute()
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
    if (set_operands[3] != nullptr && set_operands[3]->IsFixed())
    {
        if (!set_operands[3]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand SET 4 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }
    if (set_operands[4] != nullptr && set_operands[4]->IsFixed())
    {
        if (!set_operands[4]->GetValue().IsValid())
        {
#ifdef SCP_DEBUG
            Utils::logSCPError(ms_context, "Operand SET 5 has modifier FIXED, but has no value", addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }

    switch (type)
    {
    case 0x10000:
    {
        ScAddr set1, set2, set3, set4;

        sc_bool found = SC_FALSE;
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetType(), operands[3]->GetType(), operands[4]->GetType());
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
            if (set_operands[3] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[3]->IsFixed())
                    {
                        set3 = set_operands[3]->GetValue();
                    }
                    else
                    {
                        set3 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[3]->SetValue(set3);
                    }
                    operands[3]->SetValue(iter->Get(3));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
            }
            if (set_operands[4] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[4]->IsFixed())
                    {
                        set4 = set_operands[4]->GetValue();
                    }
                    else
                    {
                        set4 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[4]->SetValue(set4);
                    }
                    operands[4]->SetValue(iter->Get(4));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set4, iter->Get(4));
            }
            found = SC_TRUE;
        }
        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();

        break;
    }

    case 0x00001:
    {
        ScAddr set0, set1, set2, set3;

        sc_bool found = SC_FALSE;
        ScIterator3Ptr iter = ms_context.Iterator3(operands[4]->GetValue(), operands[3]->GetType(), operands[1]->GetType());
        while (iter->Next())
        {
            ScAddr elem0 = ms_context.GetEdgeSource(iter->Get(2));
            ScAddr elem2 = ms_context.GetEdgeTarget(iter->Get(2));
            ScType type0 = ms_context.GetElementType(elem0);
            ScType type2 = ms_context.GetElementType(elem2);
            if (((type0 & operands[0]->GetType()) == operands[0]->GetType()) && (((type2 & operands[2]->GetType()) == operands[2]->GetType())))
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
                        operands[0]->SetValue(elem0);
                    }
                    ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set0, elem0);
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
                        operands[2]->SetValue(elem2);
                    }
                    ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set2, elem2);
                }
                if (set_operands[3] != nullptr)
                {
                    if (SC_FALSE == found)
                    {
                        if (set_operands[3]->IsFixed())
                        {
                            set3 = set_operands[3]->GetValue();
                        }
                        else
                        {
                            set3 = ms_context.CreateNode(ScType::NodeConst);
                            set_operands[3]->SetValue(set3);
                        }
                        operands[3]->SetValue(iter->Get(3));
                    }
                    ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
                }
                found = SC_TRUE;
            }
        }

        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x00100:
    {
        ScAddr set0, set1, set3, set4;

        sc_bool found = SC_FALSE;
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetType(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetType());
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
            if (set_operands[3] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[3]->IsFixed())
                    {
                        set3 = set_operands[3]->GetValue();
                    }
                    else
                    {
                        set3 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[3]->SetValue(set3);
                    }
                    operands[3]->SetValue(iter->Get(3));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
            }
            if (set_operands[4] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[4]->IsFixed())
                    {
                        set4 = set_operands[4]->GetValue();
                    }
                    else
                    {
                        set4 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[4]->SetValue(set4);
                    }
                    operands[4]->SetValue(iter->Get(4));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set4, iter->Get(4));
            }
            found = SC_TRUE;
        }
        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x10100:
    {
        ScAddr set1, set3, set4;

        sc_bool found = SC_FALSE;
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetType());
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
            if (set_operands[3] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[3]->IsFixed())
                    {
                        set3 = set_operands[3]->GetValue();
                    }
                    else
                    {
                        set3 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[3]->SetValue(set3);
                    }
                    operands[3]->SetValue(iter->Get(3));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
            }
            if (set_operands[4] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[4]->IsFixed())
                    {
                        set4 = set_operands[4]->GetValue();
                    }
                    else
                    {
                        set4 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[4]->SetValue(set4);
                    }
                    operands[4]->SetValue(iter->Get(4));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set4, iter->Get(4));
            }
            found = SC_TRUE;
        }
        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x00101:
    {
        ScAddr set0, set1, set3;

        sc_bool found = SC_FALSE;
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetType(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetValue());
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
            if (set_operands[3] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[3]->IsFixed())
                    {
                        set3 = set_operands[3]->GetValue();
                    }
                    else
                    {
                        set3 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[3]->SetValue(set3);
                    }
                    operands[3]->SetValue(iter->Get(3));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
            }
            found = SC_TRUE;
        }
        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();
        break;
    }
    case 0x10001:
    {
        ScAddr set1, set2, set3;

        sc_bool found = SC_FALSE;
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetType(), operands[3]->GetType(), operands[4]->GetValue());
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
            if (set_operands[3] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[3]->IsFixed())
                    {
                        set3 = set_operands[3]->GetValue();
                    }
                    else
                    {
                        set3 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[3]->SetValue(set3);
                    }
                    operands[3]->SetValue(iter->Get(3));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
            }
            found = SC_TRUE;
        }
        if (SC_TRUE == found)
            FinishExecutionSuccessfully();
        else
            FinishExecutionUnsuccessfully();

        break;
    }
    case 0x10101:
    {
        ScAddr set1, set3;

        sc_bool found = SC_FALSE;
        ScIterator5Ptr iter = ms_context.Iterator5(operands[0]->GetValue(), operands[1]->GetType(), operands[2]->GetValue(), operands[3]->GetType(), operands[4]->GetValue());
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
            if (set_operands[3] != nullptr)
            {
                if (SC_FALSE == found)
                {
                    if (set_operands[3]->IsFixed())
                    {
                        set3 = set_operands[3]->GetValue();
                    }
                    else
                    {
                        set3 = ms_context.CreateNode(ScType::NodeConst);
                        set_operands[3]->SetValue(set3);
                    }
                    operands[3]->SetValue(iter->Get(3));
                }
                ms_context.CreateArc(ScType::EdgeAccessConstPosPerm, set3, iter->Get(3));
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
