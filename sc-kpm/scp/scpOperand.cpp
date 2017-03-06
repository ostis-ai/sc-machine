/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"

#include "scpOperand.hpp"

namespace scp
{

SCPOperand::SCPOperand(ScMemoryContext &ctx, ScAddr addr): arc_addr(addr), ms_context(ctx)
{
    addr = ms_context.GetArcEnd(arc_addr);
    resolveModifiers();
}

ScAddr SCPOperand::GetAddr()
{
    return addr;
}

ScAddr SCPOperand::GetValueAddr()
{
    return value_addr;
}

ScType SCPOperand::GetType()
{
    return element_type;
}

uint8_t SCPOperand::GetOrder()
{
    return order;
}

bool SCPOperand::isFixed()
{
    return (param_type == SCP_FIXED);
}

bool SCPOperand::isAssign()
{
    return (param_type == SCP_ASSIGN);
}

bool SCPOperand::isSCPConst()
{
    return (operand_type == SCP_CONST);
}

bool SCPOperand::isSCPVar()
{
    return (operand_type == SCP_VAR);
}

bool SCPOperand::hasSetModifier()
{
    return isSet;
}

bool SCPOperand::hasEraseModifier()
{
    return isErase;
}

void SCPOperand::resolveOrder(ScAddr modifier)
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
}

void SCPOperand::resolveModifiers()
{
    ScIterator5Ptr iter = ms_context.Iterator5(addr, sc_type_arc_pos_var_perm, SC_TYPE(sc_type_const | sc_type_node), sc_type_arc_pos_const_perm, SC_TYPE(sc_type_const | sc_type_node));
    while (iter->Next())
    {
        ScAddr modifier = iter->Get(4);
        if (ms_context.HelperCheckArc(Keynodes::order_role_relation, modifier, sc_type_arc_pos_const_perm))
        {
            resolveOrder(modifier);
            continue;
        }
        //!TODO All other modifiers
    }
}

}

