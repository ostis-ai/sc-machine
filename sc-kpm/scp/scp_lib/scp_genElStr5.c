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

#include "scp_types.h"
#include "scp_genElStr5.h"

scp_result genElStr5_a_a_a_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param1->addr = sc_memory_node_new(context, param1->element_type);
    param3->addr = sc_memory_node_new(context, param3->element_type);
    param5->addr = sc_memory_node_new(context, param5->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_f_a_a_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param3->addr = sc_memory_node_new(context, param3->element_type);
    param5->addr = sc_memory_node_new(context, param5->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_a_a_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param1->addr = sc_memory_node_new(context, param1->element_type);
    param5->addr = sc_memory_node_new(context, param5->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_a_a_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param1->addr = sc_memory_node_new(context, param1->element_type);
    param3->addr = sc_memory_node_new(context, param3->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_f_a_f_a_a(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param5->addr = sc_memory_node_new(context, param5->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_f_a_a_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param3->addr = sc_memory_node_new(context, param3->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_a_a_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param1->addr = sc_memory_node_new(context, param1->element_type);
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}

scp_result genElStr5_f_a_f_a_f(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    param2->addr = sc_memory_arc_new(context, param2->element_type, param1->addr, param3->addr);
    param4->addr = sc_memory_arc_new(context, param4->element_type, param5->addr, param2->addr);
    return SCP_RESULT_TRUE;
}
