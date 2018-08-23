/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"

#include "scpParameter.hpp"

#include <iostream>

namespace scp
{

SCPParameter::SCPParameter(ScMemoryContext &ctx_, ScAddr addr_): arc_addr(addr_), ms_context(ctx_)
{
    addr = ms_context.GetArcEnd(arc_addr);
    resolveModifiers();
}

ScAddr SCPParameter::GetAddr()
{
    return addr;
}

uint8_t SCPParameter::GetOrder()
{
    return order;
}

bool SCPParameter::IsIn()
{
    return isIn;
}

bool SCPParameter::IsOut()
{
    return isOut;
}

void SCPParameter::resolveOrder(ScAddr modifier)
{
    if (modifier == Keynodes::rrel_1)
    {
        order = 1;
        return;
    }
    if (modifier == Keynodes::rrel_2)
    {
        order = 2;
        return;
    }
    if (modifier == Keynodes::rrel_3)
    {
        order = 3;
        return;
    }
    if (modifier == Keynodes::rrel_4)
    {
        order = 4;
        return;
    }
    if (modifier == Keynodes::rrel_5)
    {
        order = 5;
        return;
    }
    if (modifier == Keynodes::rrel_6)
    {
        order = 6;
        return;
    }
    if (modifier == Keynodes::rrel_7)
    {
        order = 7;
        return;
    }
    if (modifier == Keynodes::rrel_8)
    {
        order = 8;
        return;
    }
    if (modifier == Keynodes::rrel_9)
    {
        order = 9;
        return;
    }
    if (modifier == Keynodes::rrel_10)
    {
        order = 10;
        return;
    }
}

void SCPParameter::resolveModifiers()
{
    ScIterator3Ptr iter = ms_context.Iterator3(ScType::NodeConst, ScType::EdgeAccessVarPosPerm, arc_addr);
    while (iter->Next())
    {
        ScAddr modifier = iter->Get(0);
        if (ms_context.HelperCheckArc(Keynodes::order_role_relation, modifier, ScType::EdgeAccessConstPosPerm))
        {
            resolveOrder(modifier);
            continue;
        }

        if (modifier == Keynodes::rrel_in)
        {
            isIn = true;
            continue;
        }
        if (modifier == Keynodes::rrel_out)
        {
            isOut = true;
            continue;
        }
    }
}

}

