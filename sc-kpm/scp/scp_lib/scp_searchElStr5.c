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

#include "scp_searchElStr5.h"

#include <stdio.h>

scp_result searchElStr5_f_a_a_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator5 *it = sc_iterator5_f_a_a_a_a_new(context, param1->addr, param2->element_type, param3->element_type, param4->element_type, param5->element_type);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        param2->addr = sc_iterator5_value(it, 1);
        param3->addr = sc_iterator5_value(it, 2);
        param4->addr = sc_iterator5_value(it, 3);
        param5->addr = sc_iterator5_value(it, 4);
        sc_iterator5_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator5_free(it);
    return SCP_RESULT_FALSE;
}
//scp_result searchElStr5_a_f_a_a_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
scp_result searchElStr5_a_a_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator5 *it = sc_iterator5_a_a_f_a_a_new(context, param1->element_type, param2->element_type, param3->addr, param4->element_type, param5->element_type);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        param1->addr = sc_iterator5_value(it, 0);
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        param5->addr = sc_iterator5_value(it, 4);
        sc_iterator5_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator5_free(it);
    return SCP_RESULT_FALSE;
}
//scp_result searchElStr5_a_a_a_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
scp_result searchElStr5_a_a_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, param5->addr, param4->element_type, param2->element_type);
    if (SC_TRUE == sc_iterator3_next(it))
    {
        param4->addr = sc_iterator3_value(it, 1);
        param2->addr = sc_iterator3_value(it, 2);
        sc_memory_get_arc_begin(context, param2->addr, &(param1->addr));
        sc_memory_get_arc_end(context, param2->addr, &(param3->addr));
        sc_iterator3_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

//scp_result searchElStr5_f_f_a_a_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
scp_result searchElStr5_f_a_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator5 *it = sc_iterator5_f_a_f_a_a_new(context, param1->addr, param2->element_type, param3->addr, param4->element_type, param5->element_type);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        param5->addr = sc_iterator5_value(it, 4);
        sc_iterator5_free(it);        
        return SCP_RESULT_TRUE;
    }
    sc_iterator5_free(it);
    return SCP_RESULT_FALSE;
}
//scp_result searchElStr5_f_a_a_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
scp_result searchElStr5_f_a_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator5 *it = sc_iterator5_f_a_a_a_f_new(context, param1->addr, param2->element_type, param3->element_type, param4->element_type, param5->addr);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        param2->addr = sc_iterator5_value(it, 1);
        param3->addr = sc_iterator5_value(it, 2);
        param4->addr = sc_iterator5_value(it, 3);
        sc_iterator5_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator5_free(it);
    return SCP_RESULT_FALSE;
}
//scp_result searchElStr5_a_f_f_a_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_f_a_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_f_a_a_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_a_f_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
scp_result searchElStr5_a_a_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator5 *it = sc_iterator5_a_a_f_a_f_new(context, param1->element_type, param2->element_type, param3->addr, param4->element_type, param5->addr);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        param1->addr = sc_iterator5_value(it, 0);
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        sc_iterator5_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator5_free(it);
    return SCP_RESULT_FALSE;
}
//scp_result searchElStr5_a_a_a_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}

//scp_result searchElStr5_f_f_f_a_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_f_f_a_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_f_f_a_a_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_f_a_f_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
scp_result searchElStr5_f_a_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_iterator5 *it = sc_iterator5_f_a_f_a_f_new(context, param1->addr, param2->element_type, param3->addr, param4->element_type, param5->addr);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        param2->addr = sc_iterator5_value(it, 1);
        param4->addr = sc_iterator5_value(it, 3);
        sc_iterator5_free(it);
        return SCP_RESULT_TRUE;
    }
    sc_iterator5_free(it);
    return SCP_RESULT_FALSE;
}
//scp_result searchElStr5_f_a_a_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_f_f_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_f_f_a_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_f_a_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_a_f_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}

//scp_result searchElStr5_f_f_f_f_a(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_f_f_f_a_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_f_f_a_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_f_a_f_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}
//scp_result searchElStr5_a_f_f_f_f(scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5){}

scp_result searchElStr5_f_f_f_f_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_addr arc1 = param2->addr;
    sc_addr arc2 = param4->addr;
    sc_addr begin1;
    sc_addr end1;
    sc_addr begin2;
    sc_addr end2;
    if (SC_RESULT_OK != sc_memory_get_arc_begin(context, arc1, &begin1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_RESULT_OK != sc_memory_get_arc_end(context, arc1, &end1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_RESULT_OK != sc_memory_get_arc_begin(context, arc2, &begin2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_RESULT_OK != sc_memory_get_arc_end(context, arc2, &end2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SC_ADDR_IS_EQUAL(param1->addr, begin1) && SC_ADDR_IS_EQUAL(param3->addr, end2) && SC_ADDR_IS_EQUAL(param5->addr, begin2) && SC_ADDR_IS_EQUAL(param2->addr, end2))
    {
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
}
