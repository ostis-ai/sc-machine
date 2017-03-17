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

namespace scp
{

SCPOperator::SCPOperator(ScMemoryContext &ctx, ScAddr addr): addr(addr), ms_context(ctx)
{

}

ScAddr SCPOperator::GetAddr()
{
    return addr;
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

