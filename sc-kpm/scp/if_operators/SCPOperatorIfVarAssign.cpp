/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "scpKeynodes.hpp"
#include "scpUtils.hpp"
#include "SCPOperatorIfVarAssign.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include <iostream>

namespace scp
{

SCPOperatorIfVarAssign::SCPOperatorIfVarAssign(ScMemoryContext &ctx, ScAddr addr): SCPOperatorElStr1(ctx, addr)
{
}

std::string SCPOperatorIfVarAssign::GetTypeName()
{
    return "ifVarAssign";
}

sc_result SCPOperatorIfVarAssign::Parse()
{
    return SCPOperatorElStr1::Parse();
}

sc_result SCPOperatorIfVarAssign::Execute()
{
    if (operands[0]->GetValue().IsValid())
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
