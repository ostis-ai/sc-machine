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

bool compareSCPOperandPtr(SCPOperand* a, SCPOperand* b)
{
    return (a->GetOrder() < b->GetOrder());
}

SCPOperator::SCPOperator(ScMemoryContext &ctx, ScAddr addr): addr(addr), ms_context(ctx)
{
    if (!Utils::resolveOperatorType(ms_context, addr, type))
        Utils::logUnknownOperatorTypeError(ms_context, addr);
    ScIterator3Ptr iter_operator = ms_context.Iterator3(addr, sc_type_arc_pos_const_perm, SC_TYPE(sc_type_node | sc_type_const));
    while (iter_operator->Next())
    {
        operands.push_back(new SCPOperand(ms_context, iter_operator->Get(1)));
    }
    std::sort(operands.begin(), operands.end(), compareSCPOperandPtr);
}

ScAddr SCPOperator::GetAddr()
{
    return addr;
}

ScAddr SCPOperator::GetType()
{
    return type;
}

SCPOperator::~SCPOperator()
{
    for (std::vector<SCPOperand*>::iterator i = operands.begin(); i != operands.end(); ++i)
    {
        delete *i;
    }
}

}

