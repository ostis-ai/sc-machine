/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorPrint.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"
#include <iostream>

namespace scp
{

SCPOperatorPrint::SCPOperatorPrint(ScMemoryContext &ctx, ScAddr addr, sc_bool newline_): SCPOperatorElStr1(ctx, addr)
{
    newline = newline_;
}

std::string SCPOperatorPrint::GetTypeName()
{
    if (SC_TRUE == newline)
        return "printNl";
    else
        return "print";
}

sc_result SCPOperatorPrint::Parse()
{
    return SCPOperatorElStr1::Parse();
}

sc_result SCPOperatorPrint::Execute()
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

    ScStream stream;
    if (ms_context.GetLinkContent(operands[0]->GetValue(), stream))
    {
        std::string str;
        if (ScStreamConverter::StreamToString(stream, str))
        {
            std::cout << str;
            if (SC_TRUE == newline)
                std::cout << std::endl;
            FinishExecutionSuccessfully();
        }
        else
            FinishExecutionWithError();
    }
    else
        FinishExecutionWithError();

    return SC_RESULT_OK;
}

}
