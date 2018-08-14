/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "SCPOperatorVarErase.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorVarErase::SCPOperatorVarErase(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr1(ctx, addr)
{
}

std::string SCPOperatorVarErase::GetTypeName()
{
    return "varErase";
}

sc_result SCPOperatorVarErase::Parse()
{
    return SCPOperatorElStr1::Parse();
}

sc_result SCPOperatorVarErase::Execute()
{
    if (SC_RESULT_OK != ResetValues())
        return SC_RESULT_ERROR;

    FinishExecutionSuccessfully();

    return SC_RESULT_OK;
}
}
