/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorIfType.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorIfType::SCPOperatorIfType(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr1(ctx, addr)
{
}

std::string SCPOperatorIfType::GetTypeName()
{
    return "ifType";
}

sc_result SCPOperatorIfType::Parse()
{
    return SCPOperatorElStr1::Parse();
}

sc_result SCPOperatorIfType::Execute()
{
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    if (!operands[0]->IsFixed())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand must have FIXED modifier", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
    if (!operands[0]->GetValue().IsValid())
    {
#ifdef SCP_DEBUG
        Utils::logSCPError(ms_context, "Operand has modifier FIXED, but has no value", addr);
#endif
        FinishExecutionWithError();
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }

    ScType type = ms_context.GetElementType(operands[0]->GetValue());
    if ((operands[0]->GetType() & type) == operands[0]->GetType())
    {
        FinishExecutionSuccessfully();
    }
    else
    {
        FinishExecutionUnsuccessfully();
    }
    return SC_RESULT_OK;
}

}
