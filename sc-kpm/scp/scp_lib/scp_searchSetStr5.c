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
but WITHOUT ANY WARRANTY{} without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sc_memory_headers.h"

#include <stdio.h>

#include "scp_searchSetStr5.h"

scp_result searchSetStr5_f_a_a_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[2].set == SCP_TRUE && sets[2].param_type == SCP_ASSIGN)
    {
        sets[2].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[4].set == SCP_TRUE && sets[4].param_type == SCP_ASSIGN)
    {
        sets[4].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator5 *it = sc_iterator5_f_a_a_a_a_new(context, param1->addr, param2->element_type, param3->element_type, param4->element_type, param5->element_type);
    while (SC_TRUE == sc_iterator5_next(it))
    {
        res = SCP_RESULT_TRUE;
        param2->addr = sc_iterator5_value(it, 1);
        param3->addr = sc_iterator5_value(it, 2);
        param4->addr = sc_iterator5_value(it, 3);
        param5->addr = sc_iterator5_value(it, 4);
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[2].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[2].addr, param3->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
        if (sets[4].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[4].addr, param5->addr);
        }
    }
    sc_iterator5_free(it);
    return res;
}

scp_result searchSetStr5_a_a_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[0].set == SCP_TRUE && sets[0].param_type == SCP_ASSIGN)
    {
        sets[0].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[4].set == SCP_TRUE && sets[4].param_type == SCP_ASSIGN)
    {
        sets[4].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator5 *it = sc_iterator5_a_a_f_a_a_new(context, param1->element_type, param2->element_type, param3->addr, param4->element_type, param5->element_type);
    while (SC_TRUE == sc_iterator5_next(it))
    {
        res = SCP_RESULT_TRUE;
        param1->addr = sc_iterator5_value(it, 0);
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        param5->addr = sc_iterator5_value(it, 4);
        if (sets[0].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[0].addr, param1->addr);
        }
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
        if (sets[4].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[4].addr, param5->addr);
        }
    }
    sc_iterator5_free(it);
    return res;
}

scp_result searchSetStr5_a_a_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[0].set == SCP_TRUE && sets[0].param_type == SCP_ASSIGN)
    {
        sets[0].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[2].set == SCP_TRUE && sets[2].param_type == SCP_ASSIGN)
    {
        sets[2].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, param5->addr, param4->element_type, param2->element_type);
    while (SC_TRUE == sc_iterator3_next(it))
    {
        res = SCP_RESULT_TRUE;
        param4->addr = sc_iterator3_value(it, 1);
        param2->addr = sc_iterator3_value(it, 2);
        sc_memory_get_arc_begin(context, param2->addr, &(param1->addr));
        sc_memory_get_arc_end(context, param2->addr, &(param3->addr));
        if (sets[0].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[0].addr, param1->addr);
        }
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[2].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[2].addr, param3->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
    }
    sc_iterator3_free(it);
    return res;
}

scp_result searchSetStr5_f_a_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[4].set == SCP_TRUE && sets[4].param_type == SCP_ASSIGN)
    {
        sets[4].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator5 *it = sc_iterator5_f_a_f_a_a_new(context, param1->addr, param2->element_type, param3->addr, param4->element_type, param5->element_type);
    while (SC_TRUE == sc_iterator5_next(it))
    {
        res = SCP_RESULT_TRUE;
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        param5->addr = sc_iterator5_value(it, 4);
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
        if (sets[4].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[4].addr, param5->addr);
        }
    }
    sc_iterator5_free(it);
    return res;
}

scp_result searchSetStr5_f_a_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[2].set == SCP_TRUE && sets[2].param_type == SCP_ASSIGN)
    {
        sets[2].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator5 *it = sc_iterator5_f_a_a_a_f_new(context, param1->addr, param2->element_type, param3->element_type, param4->element_type, param5->addr);
    while (SC_TRUE == sc_iterator5_next(it))
    {
        res = SCP_RESULT_TRUE;
        param2->addr = sc_iterator5_value(it, 1);
        param3->addr = sc_iterator5_value(it, 2);
        param4->addr = sc_iterator5_value(it, 3);
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[2].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[2].addr, param3->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
    }
    sc_iterator5_free(it);
    return res;
}

scp_result searchSetStr5_a_a_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[0].set == SCP_TRUE && sets[0].param_type == SCP_ASSIGN)
    {
        sets[0].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator5 *it = sc_iterator5_a_a_f_a_f_new(context, param1->element_type, param2->element_type, param3->addr, param4->element_type, param5->addr);
    while (SC_TRUE == sc_iterator5_next(it))
    {
        res = SCP_RESULT_TRUE;
        param1->addr = sc_iterator5_value(it, 0);
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        if (sets[0].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[0].addr, param1->addr);
        }
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
    }
    sc_iterator5_free(it);
    return res;
}

scp_result searchSetStr5_f_a_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    scp_result res = SCP_RESULT_FALSE;
    if (sets[1].set == SCP_TRUE && sets[1].param_type == SCP_ASSIGN)
    {
        sets[1].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    if (sets[3].set == SCP_TRUE && sets[3].param_type == SCP_ASSIGN)
    {
        sets[3].addr = sc_memory_node_new(context, sc_type_node | sc_type_const);
    }
    sc_iterator5 *it = sc_iterator5_f_a_f_a_f_new(context, param1->addr, param2->element_type, param3->addr, param4->element_type, param5->addr);
    while (SC_TRUE == sc_iterator5_next(it))
    {
        res = SCP_RESULT_TRUE;
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        if (sets[1].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[1].addr, param2->addr);
        }
        if (sets[3].set == SCP_TRUE)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, sets[3].addr, param4->addr);
        }
    }
    sc_iterator5_free(it);
    return res;
}
