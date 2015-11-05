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

#include "scp_searchElStr3.h"
#include "scp_utils.h"

scp_result searchElStr3_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_iterator3 *it = sc_iterator3_a_a_f_new(context, param1->element_type, param2->element_type, param3->addr);
    if (SC_TRUE == sc_iterator3_next(it))
    {
        param1->addr = sc_iterator3_value(it, 0);
        param2->addr = sc_iterator3_value(it, 1);
        sc_iterator3_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator3_free(it);
    return SCP_RESULT_FALSE;
}
scp_result searchElStr3_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, param1->addr, param2->element_type, param3->element_type);
    if (SC_TRUE == sc_iterator3_next(it))
    {
        param2->addr = sc_iterator3_value(it, 1);
        param3->addr = sc_iterator3_value(it, 2);
        sc_iterator3_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

scp_result searchElStr3_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_iterator3 *it = sc_iterator3_f_a_f_new(context, param1->addr, param2->element_type, param3->addr);
    if (SC_TRUE == sc_iterator3_next(it))
    {
        param2->addr = sc_iterator3_value(it, 1);
        sc_iterator3_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

scp_result searchElStr3_a_f_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_addr arc = param2->addr;
    sc_addr begin;
    sc_addr end;
    if (SC_RESULT_OK != sc_memory_get_arc_begin(context, arc, &begin))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_RESULT_OK != sc_memory_get_arc_end(context, arc, &end))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE == check_type(context, begin, param1->element_type) && SCP_RESULT_TRUE == check_type(context, end, param3->element_type))
    {
        param1->addr = begin;
        param3->addr = end;
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
}
scp_result searchElStr3_f_f_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_addr arc = param2->addr;
    sc_addr begin;
    if (SC_RESULT_OK != sc_memory_get_arc_begin(context, arc, &begin))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_ADDR_IS_EQUAL(param1->addr, begin))
    {
        sc_addr end;
        if (SC_RESULT_OK != sc_memory_get_arc_end(context, arc, &end))
        {
            return SCP_RESULT_ERROR;
        }
        if (SCP_RESULT_TRUE == check_type(context, end, param3->element_type))
        {
            return SCP_RESULT_FALSE;
        }
        param3->addr = end;
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
}
scp_result searchElStr3_a_f_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_addr arc = param2->addr;
    sc_addr end;
    if (SC_RESULT_OK != sc_memory_get_arc_end(context, arc, &end))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_ADDR_IS_EQUAL(param3->addr, end))
    {
        sc_addr begin;
        if (SC_RESULT_OK != sc_memory_get_arc_begin(context, arc, &begin))
        {
            return SCP_RESULT_ERROR;
        }
        if (SCP_RESULT_TRUE == check_type(context, begin, param1->element_type))
        {
            return SCP_RESULT_FALSE;
        }
        param1->addr = begin;
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
}

scp_result searchElStr3_f_f_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_addr arc = param2->addr;
    sc_addr begin;
    sc_addr end;
    if (SC_RESULT_OK != sc_memory_get_arc_begin(context, arc, &begin))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_RESULT_OK != sc_memory_get_arc_end(context, arc, &end))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_ADDR_IS_EQUAL(param1->addr, begin) && SC_ADDR_IS_EQUAL(param3->addr, end))
    {
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
}
