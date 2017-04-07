/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"

#include "scpOperator.hpp"

#include <algorithm>
#include <iostream>

namespace scp
{

SCPOperator::SCPOperator(ScMemoryContext &ctx_, ScAddr addr_): addr(addr_), ms_context(ctx_)
{
}

std::string SCPOperator::GetTypeName()
{
    return "unknown operator";
}

ScAddr SCPOperator::GetAddr()
{
    return addr;
}

sc_result SCPOperator::ResetValues()
{
    for (std::vector<SCPOperand*>::iterator i = operands.begin(); i != operands.end(); ++i)
    {
        if ((*i)->IsAssign())
        {
            if ((*i)->IsSCPConst())
            {
#ifdef SCP_DEBUG
                Utils::logSCPError(ms_context, "SCP CONST must have FIXED modifier", addr);
#endif
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
            (*i)->ResetValue();
        }
    }
    return SC_RESULT_OK;
}

sc_result SCPOperator::Parse()
{
    std::fill(operands.begin(), operands.end(), nullptr);
    return SC_RESULT_OK;
}

sc_result SCPOperator::Execute()
{
    return SC_RESULT_OK;
}

SCPOperator::~SCPOperator()
{
    for (std::vector<SCPOperand*>::iterator i = operands.begin(); i != operands.end(); ++i)
    {
        delete *i;
    }
}

}

