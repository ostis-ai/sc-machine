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

#include "sc_memory_headers.h"

#include "scp_eraseSetStr3.h"
#include "scp_utils.h"

scp_result eraseSetStr3F(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    if (param1->param_type == SCP_ASSIGN && param1->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param1->addr);
    }
    if (param3->param_type == SCP_ASSIGN && param3->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param3->addr);
    }
    if (SC_TRUE == sc_memory_is_element(context, param2->addr) && param2->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param2->addr);
    }
    return SCP_RESULT_TRUE;
}

scp_result eraseSetStr3_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_iterator3 *it = sc_iterator3_a_a_f_new(context, param1->element_type, param2->element_type, param3->addr);
    sc_bool flag = SC_FALSE;
    while (SC_TRUE == sc_iterator3_next(it))
    {
        flag = SC_TRUE;
        param1->addr = sc_iterator3_value(it, 0);
        param2->addr = sc_iterator3_value(it, 1);
        eraseSetStr3F(context, param1, param2, param3);
    }
    if (param3->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param3->addr);
    }
    sc_iterator3_free(it);
    if (flag == SC_TRUE)
        return SC_TRUE;
    else
        return SCP_RESULT_FALSE;
}

scp_result eraseSetStr3_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, param1->addr, param2->element_type, param3->element_type);
    sc_bool flag = SC_FALSE;
    while (SC_TRUE == sc_iterator3_next(it))
    {
        flag = SC_TRUE;
        param2->addr = sc_iterator3_value(it, 1);
        param3->addr = sc_iterator3_value(it, 2);
        eraseSetStr3F(context, param1, param2, param3);
    }
    if (param1->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param1->addr);
    }
    sc_iterator3_free(it);
    if (flag == SC_TRUE)
        return SC_TRUE;
    else
        return SCP_RESULT_FALSE;
}

scp_result eraseSetStr3_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_iterator3 *it = sc_iterator3_f_a_f_new(context, param1->addr, param2->element_type, param3->addr);
    sc_bool flag = SC_FALSE;
    while (SC_TRUE == sc_iterator3_next(it))
    {
        flag = SC_TRUE;
        param2->addr = sc_iterator3_value(it, 1);
        eraseSetStr3F(context, param1, param2, param3);
    }
    if (param1->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param1->addr);
    }
    if (param3->erase == SCP_TRUE)
    {
        sc_memory_element_free(context, param3->addr);
    }
    sc_iterator3_free(it);
    if (flag == SC_TRUE)
        return SC_TRUE;
    else
        return SCP_RESULT_FALSE;
}
