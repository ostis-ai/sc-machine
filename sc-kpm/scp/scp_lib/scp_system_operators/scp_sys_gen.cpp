/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sc_system_gen.h"

extern "C"
{
#include "sc_memory_headers.h"

#include "scp_sys_gen.h"
#include "../scp_types.h"
}

extern "C" scp_result sys_gen(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand_pair *parameters, sc_uint32 param_count, scp_operand *param4)
{
    sc_type_result params;
    sc_addr addr1, addr2, arc;
    sc_type_result result;
    sc_type_result::iterator it;
    sc_uint i;

    for (i = 0; i < param_count; i++)
    {
        params.insert(sc_addr_pair(parameters[i].operand1->addr, parameters[i].operand2->addr));
    }
    system_sys_gen(context, param1->addr, params, &result);

    for (it = result.begin() ; it != result.end(); it++)
    {
        addr1 = (*it).first;
        addr2 = (*it).second;
        arc = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, addr1, addr2);
        sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param2->addr, arc);
        if (param4 != null_ptr)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param4->addr, addr2);
        }
    }

    return SCP_RESULT_TRUE;
}

extern "C" scp_result sys_gen_for_variables(sc_memory_context *context, scp_operand *param1, scp_operand_pair *variables, sc_uint32 var_count, scp_operand_pair *parameters, sc_uint32 param_count, scp_operand *param4)
{
    sc_type_result params;
    sc_type_result result;
    sc_addr_vector vars;
    sc_uint i;

    for (i = 0; i < param_count; i++)
    {
        params.insert(sc_addr_pair(parameters[i].operand1->addr, parameters[i].operand2->addr));
    }

    for (i = 0; i < var_count; i++)
    {
        vars.push_back(variables[i].operand1->addr);
    }

    system_sys_gen_for_variables(context, param1->addr, params, vars, &result);

    for (i = 0; i < var_count; i++)
    {
        scp_operand *op1 = variables[i].operand1;
        scp_operand *op2 = variables[i].operand2;
        find_result_pair_for_var(&result, op1->addr, &(op2->addr));
        if (param4 != null_ptr)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param4->addr, op2->addr);
        }
    }

    return SCP_RESULT_TRUE;
}
