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

#include "sc_system_search.h"
#include <glib.h>

extern "C"
{
#include "sc_memory_headers.h"

#include "scp_sys_search.h"
#include "../scp_types.h"
}

extern "C" scp_result sys_search_for_variables(sc_memory_context *context, scp_operand *param1, scp_operand_pair *variables, sc_uint32 var_count, scp_operand_pair *parameters, sc_uint32 param_count, scp_operand *param4)
{
    sc_type_result params;
    sc_addr addr1;
    sc_addr_vector vars;
    sc_type_result_vector result;
    sc_uint i, j;
    scp_result res = SCP_RESULT_FALSE;
    GHashTable *table;

    for (i = 0; i < param_count; i++)
    {
        params.insert(sc_addr_pair(parameters[i].operand1->addr, parameters[i].operand2->addr));
    }
    for (i = 0; i < var_count; i++)
    {
        vars.push_back(variables[i].operand1->addr);
    }

    if (SC_RESULT_OK != system_sys_search_for_variables(context, param1->addr, params, vars, &result))
    {
        return SCP_RESULT_ERROR;
    }
    if (result.size() > 0)
    {
        res = SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
    for (i = 0; i < var_count; i++)
    {
        scp_operand *op1 = variables[i].operand1;
        scp_operand *op2 = variables[i].operand2;
        if (op2->set == SCP_TRUE)
        {
            for (j = 0; j < result.size(); j++)
            {
                find_result_pair_for_var(result[j], op1->addr, &addr1);
                sc_memory_arc_new(context, sc_type_arc_pos_const_perm, op2->addr, addr1);
            }
        }
        else
        {
            find_result_pair_for_var(result[i], op1->addr, &(op2->addr));
        }
    }
    if (param4 != null_ptr)
    {
        table = g_hash_table_new(NULL, NULL);
        sc_type_result::iterator it;
        for (i = 0; i < result.size(); i++)
        {
            for (it = result[i]->begin() ; it != result[i]->end(); it++)
            {
                addr1 = (*it).second;
                if (FALSE == g_hash_table_contains(table, MAKE_SC_ADDR_HASH(addr1)))
                {
                    sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param4->addr, addr1);
                    g_hash_table_add(table, MAKE_SC_ADDR_HASH(addr1));
                }
            }
        }
        g_hash_table_destroy(table);
    }
    free_result_vector(&result);
    return res;
}

extern "C" scp_result sys_search(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand_pair *parameters, sc_uint32 param_count, scp_operand *param4, scp_bool full_only)
{
    sc_type_result params;
    sc_addr curr_result_node, addr1, addr2, arc;
    sc_type_result_vector result;
    sc_type_result::iterator it;
    sc_uint i;
    scp_result res = SCP_RESULT_FALSE;
    GHashTable *table;

    for (i = 0; i < param_count; i++)
    {
        params.insert(sc_addr_pair(parameters[i].operand1->addr, parameters[i].operand2->addr));
    }
    if (SCP_TRUE == full_only)
    {
        if (SC_RESULT_OK != system_sys_search_only_full(context, param1->addr, params, &result))
        {
            return SCP_RESULT_ERROR;
        }
    }
    else
    {
        if (SC_RESULT_OK != system_sys_search(context, param1->addr, params, &result))
        {
            return SCP_RESULT_ERROR;
        }
    }
    if (result.size() > 0)
    {
        res = SCP_RESULT_TRUE;
    }
    else
    {
        free_result_vector(&result);
        return SCP_RESULT_FALSE;
    }
    if (param4 != null_ptr)
    {
        table = g_hash_table_new(NULL, NULL);
    }
    for (i = 0; i < result.size(); i++)
    {
        curr_result_node = sc_memory_node_new(context, sc_type_const);
        for (it = result[i]->begin() ; it != result[i]->end(); it++)
        {
            addr1 = (*it).first;
            addr2 = (*it).second;
            arc = sc_memory_arc_new(context, sc_type_arc_common | sc_type_const, addr1, addr2);
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, curr_result_node, arc);
            if (param4 != null_ptr)
            {
                if (FALSE == g_hash_table_contains(table, MAKE_SC_ADDR_HASH(addr2)))
                {
                    sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param4->addr, addr2);
                    g_hash_table_add(table, MAKE_SC_ADDR_HASH(addr2));
                }

            }
        }
        sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param2->addr, curr_result_node);
    }
    free_result_vector(&result);
    if (param4 != null_ptr)
    {
        g_hash_table_destroy(table);
    }
    return res;
}
