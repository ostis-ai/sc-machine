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
#include "sc_helper.h"

#include "scp_functions.h"
#include "scp_searchElStr3.h"
#include "scp_searchElStr5.h"
#include "scp_searchSetStr3.h"
#include "scp_searchSetStr5.h"
#include "scp_genElStr3.h"
#include "scp_genElStr5.h"
#include "scp_eraseElStr3.h"
#include "scp_eraseElStr5.h"
#include "scp_eraseSetStr3.h"
#include "scp_eraseSetStr5.h"
#include "scp_utils.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <glib.h>

scp_result scp_lib_init()
{
    //! TODO Add keynodes loading
    return SCP_RESULT_TRUE;
}

scp_result scp_lib_shutdown()
{
    return SCP_RESULT_TRUE;
}

scp_result scp_lib_resolve_system_identifier(sc_memory_context *context, const char *idtf, scp_operand *param)
{
    if (SC_TRUE == sc_helper_resolve_system_identifier(context, idtf, &(param->addr)))
    {
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_ERROR;
    }
}

scp_result genEl(sc_memory_context *context, scp_operand *param)
{
    if (param->param_type != SCP_ASSIGN)
    {
        return print_error("genEl", "Parameter must have ASSIGN modifier");
    }
    if ((param->element_type & scp_type_node) != scp_type_node)
    {
        return print_error("genEl", "Only node element can be generated. Use genElStr3 for arcs");
    }
    param->addr = sc_memory_node_new(context, param->element_type);
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_error("genEl", "Element cannot be generated");
    }
    return SCP_RESULT_TRUE;
}

scp_result genElStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed = 0;
    if (param2->param_type == SCP_FIXED)
    {
        return print_error("genElStr3", "Parameter 2 must have ASSIGN modifier");
    }
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("genElStr3", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("genElStr3", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    fixed = (fixed1 | fixed3);
    switch (fixed)
    {
        case 0x101:
            return genElStr3_f_a_f(context, param1, param2, param3);
        case 0x100:
            return genElStr3_a_a_f(context, param1, param2, param3);
        case 0x001:
            return genElStr3_f_a_a(context, param1, param2, param3);
        case 0x000:
            return genElStr3_a_a_a(context, param1, param2, param3);
        default:
            return print_error("genElStr3", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result genElStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed5 = 0;
    sc_uint32 fixed = 0;
    if (param2->param_type == SCP_FIXED)
    {
        return print_error("genElStr5", "Parameter 2 must have ASSIGN modifier");
    }
    if (param4->param_type == SCP_FIXED)
    {
        return print_error("genElStr5", "Parameter 4 must have ASSIGN modifier");
    }
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("genElStr5", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            printEl(context, param1);
            return print_error("genElStr5", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    if (param5->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param5->addr))
        {
            return print_error("genElStr5", "Parameter 5 has modifier FIXED, but has not value");
        }
        fixed5 = 0x10000;
    }
    fixed = (fixed1 | fixed3 | fixed5);
    switch (fixed)
    {
        case 0x10101:
            return genElStr5_f_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x00101:
            return genElStr5_f_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x10001:
            return genElStr5_f_a_a_a_f(context, param1, param2, param3, param4, param5);
        case 0x10100:
            return genElStr5_a_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x10000:
            return genElStr5_a_a_a_a_f(context, param1, param2, param3, param4, param5);
        case 0x00100:
            return genElStr5_a_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x00001:
            return genElStr5_f_a_a_a_a(context, param1, param2, param3, param4, param5);
        case 0x00000:
            return genElStr5_a_a_a_a_a(context, param1, param2, param3, param4, param5);
        default:
            return print_error("genElStr5", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result searchElStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed = 0;
    if (param1->param_type == SCP_ASSIGN && param2->param_type == SCP_ASSIGN && param3->param_type == SCP_ASSIGN)
    {
        return print_error("searchElStr3", "At least one operand must have FIXED modifier");
    }
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("searchElStr3", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param2->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param2->addr))
        {
            return print_error("searchElStr3", "Parameter 2 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param2->addr, scp_type_node))
        {
            return print_error("searchElStr3", "Parameter 2 is not an arc");
        }
        fixed2 = 0x10;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("searchElStr3", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    fixed = (fixed1 | fixed2 | fixed3);
    switch (fixed)
    {
        case 0x001:
            return searchElStr3_f_a_a(context, param1, param2, param3);
        case 0x010:
            return searchElStr3_a_f_a(context, param1, param2, param3);
        case 0x100:
            return searchElStr3_a_a_f(context, param1, param2, param3);
        case 0x011:
            return searchElStr3_f_f_a(context, param1, param2, param3);
        case 0x101:
            return searchElStr3_f_a_f(context, param1, param2, param3);
        case 0x110:
            return searchElStr3_a_f_f(context, param1, param2, param3);
        case 0x111:
            return searchElStr3_f_f_f(context, param1, param2, param3);
        default:
            return print_error("searchElStr3", "Unsupported parameter type combination");
    }

    return SCP_RESULT_ERROR;
}

scp_result searchSetStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *sets)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed = 0;
    if (param2->param_type == SCP_FIXED)
    {
        return print_error("searchSetStr3", "Parameter 2 must have ASSIGN modifier");
    }
    if (param1->param_type == SCP_ASSIGN && param2->param_type == SCP_ASSIGN && param3->param_type == SCP_ASSIGN)
    {
        return print_error("searchSetStr3", "At least one operand must have FIXED modifier");
    }
    if (param1->param_type == SCP_FIXED)
    {
        if (SCP_TRUE == sets[0].set)
        {
            return print_error("searchSetStr3", "Parameter 1 has modifier FIXED, but marked as SET");
        }
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("searchSetStr3", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SCP_TRUE == sets[2].set)
        {
            return print_error("searchSetStr3", "Parameter 3 has modifier FIXED, but marked as SET");
        }
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("searchSetStr3", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    fixed = (fixed1 | fixed2 | fixed3);
    switch (fixed)
    {
        case 0x001:
            return searchSetStr3_f_a_a(context, param1, param2, param3, sets);
        case 0x100:
            return searchSetStr3_a_a_f(context, param1, param2, param3, sets);
        case 0x101:
            return searchSetStr3_f_a_f(context, param1, param2, param3, sets);
        default:
            return print_error("searchElStr3", "Unsupported parameter type combination");
    }

    return SCP_RESULT_ERROR;
}

scp_result searchElStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed4 = 0;
    sc_uint32 fixed5 = 0;
    sc_uint32 fixed = 0;
    if (param1->param_type == SCP_ASSIGN && param2->param_type == SCP_ASSIGN && param3->param_type == SCP_ASSIGN && param4->param_type == SCP_ASSIGN && param5->param_type == SCP_ASSIGN)
    {
        return print_error("searchElStr5", "At least one operand must have FIXED modifier");
    }
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("searchElStr5", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param2->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param2->addr))
        {
            return print_error("searchElStr5", "Parameter 2 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param2->addr, scp_type_node))
        {
            return print_error("searchElStr5", "Parameter 2 is not an arc");
        }
        fixed2 = 0x10;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("searchElStr5", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    if (param4->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param4->addr))
        {
            return print_error("searchElStr5", "Parameter 4 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param4->addr, scp_type_node))
        {
            return print_error("searchElStr5", "Parameter 4 is not an arc");
        }
        fixed4 = 0x1000;
    }
    if (param5->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param5->addr))
        {
            return print_error("searchElStr5", "Parameter 5 has modifier FIXED, but has not value");
        }
        fixed5 = 0x10000;
    }
    fixed = (fixed1 | fixed2 | fixed3 | fixed4 | fixed5);
    switch (fixed)
    {
        case 0x11111:
            return searchElStr5_f_f_f_f_f(context, param1, param2, param3, param4, param5);
        case 0x10101:
            return searchElStr5_f_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x10001:
            return searchElStr5_f_a_a_a_f(context, param1, param2, param3, param4, param5);
        case 0x00101:
            return searchElStr5_f_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x10100:
            return searchElStr5_a_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x00100:
            return searchElStr5_a_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x00001:
            return searchElStr5_f_a_a_a_a(context, param1, param2, param3, param4, param5);
        case 0x10000:
            return searchElStr5_a_a_a_a_f(context, param1, param2, param3, param4, param5);
        default:
            return print_error("searchElStr5", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result searchSetStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5, scp_operand *sets)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed4 = 0;
    sc_uint32 fixed5 = 0;
    sc_uint32 fixed = 0;
    if (param2->param_type == SCP_FIXED)
    {
        return print_error("searchSetStr5", "Parameter 2 must have ASSIGN modifier");
    }
    if (param4->param_type == SCP_FIXED)
    {
        return print_error("searchSetStr5", "Parameter 4 must have ASSIGN modifier");
    }

    if (param1->param_type == SCP_FIXED)
    {
        if (SCP_TRUE == sets[0].set)
        {
            return print_error("searchSetStr5", "Parameter 1 has modifier FIXED, but marked as SET");
        }
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("searchSetStr5", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SCP_TRUE == sets[2].set)
        {
            return print_error("searchSetStr5", "Parameter 3 has modifier FIXED, but marked as SET");
        }
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("searchSetStr5", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    if (param5->param_type == SCP_FIXED)
    {
        if (SCP_TRUE == sets[4].set)
        {
            return print_error("searchSetStr5", "Parameter 5 has modifier FIXED, but marked as SET");
        }
        if (SC_FALSE == sc_memory_is_element(context, param5->addr))
        {
            return print_error("searchSetStr5", "Parameter 5 has modifier FIXED, but has not value");
        }
        fixed5 = 0x10000;
    }
    fixed = (fixed1 | fixed2 | fixed3 | fixed4 | fixed5);
    switch (fixed)
    {
        case 0x10101:
            return searchSetStr5_f_a_f_a_f(context, param1, param2, param3, param4, param5, sets);
        case 0x10001:
            return searchSetStr5_f_a_a_a_f(context, param1, param2, param3, param4, param5, sets);
        case 0x00101:
            return searchSetStr5_f_a_f_a_a(context, param1, param2, param3, param4, param5, sets);
        case 0x10100:
            return searchSetStr5_a_a_f_a_f(context, param1, param2, param3, param4, param5, sets);
        case 0x00100:
            return searchSetStr5_a_a_f_a_a(context, param1, param2, param3, param4, param5, sets);
        case 0x00001:
            return searchSetStr5_f_a_a_a_a(context, param1, param2, param3, param4, param5, sets);
        case 0x10000:
            return searchSetStr5_a_a_a_a_f(context, param1, param2, param3, param4, param5, sets);
        default:
            return print_error("searchSetStr5", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result eraseEl(sc_memory_context *context, scp_operand *param)
{
    if (param->param_type != SCP_FIXED)
    {
        return print_error("eraseEl", "Parameter must have FIXED modifier");
    }
    if (param->erase != SCP_TRUE)
    {
        return print_error("eraseEl", "Parameter must have ERASE modifier");
    }
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_error("eraseEl", "Element has not value");
    }
    sc_memory_element_free(context, param->addr);
    SC_ADDR_MAKE_EMPTY(param->addr);
    return SCP_RESULT_TRUE;
}

scp_result eraseElStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed = 0;
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("eraseElStr3", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param2->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param2->addr))
        {
            return print_error("eraseElStr3", "Parameter 2 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param2->addr, scp_type_node))
        {
            return print_error("eraseElStr3", "Parameter 2 is not an arc");
        }
        fixed2 = 0x10;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("eraseElStr3", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    fixed = (fixed1 | fixed2 | fixed3);
    switch (fixed)
    {
        case 0x001:
            return eraseElStr3_f_a_a(context, param1, param2, param3);
        case 0x100:
            return eraseElStr3_a_a_f(context, param1, param2, param3);
        case 0x101:
            return eraseElStr3_f_a_f(context, param1, param2, param3);
        default:
            return print_error("eraseElStr3", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result eraseElStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed4 = 0;
    sc_uint32 fixed5 = 0;
    sc_uint32 fixed = 0;
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("eraseElStr5", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param2->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param2->addr))
        {
            return print_error("eraseElStr5", "Parameter 2 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param2->addr, scp_type_node))
        {
            return print_error("eraseElStr5", "Parameter 2 is not an arc");
        }
        fixed2 = 0x10;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("eraseElStr5", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    if (param4->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param4->addr))
        {
            return print_error("eraseElStr5", "Parameter 4 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param4->addr, scp_type_node))
        {
            return print_error("eraseElStr5", "Parameter 4 is not an arc");
        }
        fixed4 = 0x1000;
    }
    if (param5->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param5->addr))
        {
            return print_error("eraseElStr5", "Parameter 5 has modifier FIXED, but has not value");
        }
        fixed5 = 0x10000;
    }
    fixed = (fixed1 | fixed2 | fixed3 | fixed4 | fixed5);
    switch (fixed)
    {
        case 0x10101:
            return eraseElStr5_f_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x10001:
            return eraseElStr5_f_a_a_a_f(context, param1, param2, param3, param4, param5);
        case 0x00101:
            return eraseElStr5_f_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x10100:
            return eraseElStr5_a_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x00100:
            return eraseElStr5_a_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x00001:
            return eraseElStr5_f_a_a_a_a(context, param1, param2, param3, param4, param5);
        case 0x10000:
            return eraseElStr5_a_a_a_a_f(context, param1, param2, param3, param4, param5);
        default:
            return print_error("eraseElStr5", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result eraseSetStr3(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed = 0;
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("eraseSetStr3", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param2->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param2->addr))
        {
            return print_error("eraseSetStr3", "Parameter 2 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param2->addr, scp_type_node))
        {
            return print_error("eraseSetStr3", "Parameter 2 is not an arc");
        }
        fixed2 = 0x10;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("eraseSetStr3", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    fixed = (fixed1 | fixed2 | fixed3);
    switch (fixed)
    {
        case 0x001:
            return eraseSetStr3_f_a_a(context, param1, param2, param3);
        case 0x100:
            return eraseSetStr3_a_a_f(context, param1, param2, param3);
        case 0x101:
            return eraseSetStr3_f_a_f(context, param1, param2, param3);
        default:
            return print_error("eraseSetStr3", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result eraseSetStr5(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4, scp_operand *param5)
{
    sc_uint32 fixed1 = 0;
    sc_uint32 fixed2 = 0;
    sc_uint32 fixed3 = 0;
    sc_uint32 fixed4 = 0;
    sc_uint32 fixed5 = 0;
    sc_uint32 fixed = 0;
    if (param1->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error("eraseSetStr5", "Parameter 1 has modifier FIXED, but has not value");
        }
        fixed1 = 0x1;
    }
    if (param2->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param2->addr))
        {
            return print_error("eraseSetStr5", "Parameter 2 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param2->addr, scp_type_node))
        {
            return print_error("eraseSetStr5", "Parameter 2 is not an arc");
        }
        fixed2 = 0x10;
    }
    if (param3->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param3->addr))
        {
            return print_error("eraseSetStr5", "Parameter 3 has modifier FIXED, but has not value");
        }
        fixed3 = 0x100;
    }
    if (param4->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param4->addr))
        {
            return print_error("eraseSetStr5", "Parameter 4 has modifier FIXED, but has not value");
        }
        if (SCP_RESULT_TRUE == check_type(context, param4->addr, scp_type_node))
        {
            return print_error("eraseSetStr5", "Parameter 4 is not an arc");
        }
        fixed4 = 0x1000;
    }
    if (param5->param_type == SCP_FIXED)
    {
        if (SC_FALSE == sc_memory_is_element(context, param5->addr))
        {
            return print_error("eraseSetStr5", "Parameter 5 has modifier FIXED, but has not value");
        }
        fixed5 = 0x10000;
    }
    fixed = (fixed1 | fixed2 | fixed3 | fixed4 | fixed5);
    switch (fixed)
    {
        case 0x10101:
            return eraseSetStr5_f_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x10001:
            return eraseSetStr5_f_a_a_a_f(context, param1, param2, param3, param4, param5);
        case 0x00101:
            return eraseSetStr5_f_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x10100:
            return eraseSetStr5_a_a_f_a_f(context, param1, param2, param3, param4, param5);
        case 0x00100:
            return eraseSetStr5_a_a_f_a_a(context, param1, param2, param3, param4, param5);
        case 0x00001:
            return eraseSetStr5_f_a_a_a_a(context, param1, param2, param3, param4, param5);
        case 0x10000:
            return eraseSetStr5_a_a_a_a_f(context, param1, param2, param3, param4, param5);
        default:
            return print_error("eraseSetStr5", "Unsupported parameter type combination");
    }
    return SCP_RESULT_ERROR;
}

scp_result ifVarAssign(sc_memory_context *context, scp_operand *param)
{
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
        return SCP_RESULT_FALSE;
    else
        return SCP_RESULT_TRUE;
}

scp_result ifCoin(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    if (SC_FALSE == sc_memory_is_element(context, param1->addr))
    {
        return print_error("ifCoin", "Parameter 1 has no value");
    }
    if (SC_FALSE == sc_memory_is_element(context, param2->addr))
    {
        return print_error("ifCoin", "Parameter 2 has no value");
    }
    if (SC_ADDR_IS_EQUAL(param1->addr, param2->addr))
        return SCP_RESULT_TRUE;
    else
        return SCP_RESULT_FALSE;
}

scp_result ifType(sc_memory_context *context, scp_operand *param)
{
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_error("ifType", "Parameter has no value");
    }
    return check_type(context, param->addr, param->element_type);
}

void printIdentifier(sc_memory_context *context, sc_addr element)
{
    sc_addr idtf;
    sc_type type;
    sc_memory_get_element_type(context, element, &type);

    if ((sc_type_node & type) == sc_type_node || (sc_type_link & type) == sc_type_link)
    {

		if (SC_RESULT_OK == sc_helper_get_system_identifier_link(context, element, &idtf))
        {
            sc_stream *stream;
            sc_uint32 length = 0, read_length = 0;
            sc_char *data;
            sc_memory_get_link_content(context, idtf, &stream);
            sc_stream_get_length(stream, &length);
            data = (sc_char *)calloc(length + 1, sizeof(sc_char));
            sc_stream_read_data(stream, data, length, &read_length);
            data[length] = '\0';
            printf("%s", data);
            sc_stream_free(stream);
            free(data);
        }
        else
        {
            printf("%u|%u", element.seg, element.offset);
        }
    }
    else
    {
        sc_addr elem1, elem2;
        sc_memory_get_arc_begin(context, element, &elem1);
        sc_memory_get_arc_end(context, element, &elem2);
        printf("(");
        printIdentifier(context, elem1);
        printf(" -> ");
        printIdentifier(context, elem2);
        printf(")");
    }
}

scp_result printEl(sc_memory_context *context, scp_operand *param)
{
    sc_addr element = param->addr;
    sc_addr addr2, addr3;
    sc_iterator3 *it = null_ptr;
    sc_uint32 out_c = 0, in_c = 0;
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_error("printEl", "Parameter has not value");
    }
    printIdentifier(context, element);
    printf("\n");

    it = sc_iterator3_a_a_f_new(context, 0, 0, element);
    if (it == 0)
    {
        return SCP_RESULT_ERROR;
    }
    printf("Input arcs:\n");
    while (SC_TRUE == sc_iterator3_next(it))
    {
        in_c++;
        addr2 = sc_iterator3_value(it, 0);
        addr3 = sc_iterator3_value(it, 1);


        if (SCP_RESULT_TRUE == check_type(context, addr3, scp_type_arc_access))
        {
            printf("\t%u|%u <- ", addr3.seg, addr3.offset);
        }
        else
        {
            printf("\t%u|%u <= ", addr3.seg, addr3.offset);
        }
        printIdentifier(context, addr2);
        printf("\n");
    }
    sc_iterator3_free(it);
    printf("Total input arcs: %d\n", in_c);

    it = sc_iterator3_f_a_a_new(context, element, 0, 0);
    if (it == 0)
    {
        return SCP_RESULT_ERROR;
    }
    printf("Output arcs:\n");
    while (SC_TRUE == sc_iterator3_next(it))
    {
        out_c++;
        addr2 = sc_iterator3_value(it, 1);
        addr3 = sc_iterator3_value(it, 2);

        if (SCP_RESULT_TRUE == check_type(context, addr2, scp_type_arc_access))
        {
            printf("\t%u|%u -> ", addr2.seg, addr2.offset);
        }
        else
        {
            printf("\t%u|%u => ", addr2.seg, addr2.offset);
        }

        printIdentifier(context, addr3);
        printf("\n");
    }
    sc_iterator3_free(it);
    printf("Total output arcs: %d\n", out_c);
    return SCP_RESULT_TRUE;
}

scp_result printL(sc_memory_context *context, scp_operand *param, sc_bool new_line)
{
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_char *data;
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_error("print", "Parameter has not value");
    }
    /*if (SCP_RESULT_FALSE == check_type(param->addr, sc_type_link))
    {
        return print_error("print", "Parameter is not an sc-link");
    }*/
    if (sc_memory_get_link_content(context, param->addr, &stream) != SC_RESULT_OK)
    {
        return print_error("print", "Content reading error");
    }
    sc_stream_get_length(stream, &length);
    data = calloc(length + 1, sizeof(sc_char));
    sc_stream_read_data(stream, data, length, &read_length);
    data[length] = '\0';
    printf("%s", data);
    if (SC_TRUE == new_line)
    {
        printf("\n");
    }
    sc_stream_free(stream);
    free(data);
    return SCP_RESULT_TRUE;
}

scp_result print(sc_memory_context *context, scp_operand *param)
{
    return printL(context, param, SC_FALSE);
}

scp_result printNl(sc_memory_context *context, scp_operand *param)
{
    return printL(context, param, SC_TRUE);
}

scp_result varAssign(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    if (SCP_FIXED == param1->param_type)
    {
        return print_error("varAssign", "Parameter 1 must have ASSIGN modifier");
    }
    if (SCP_ASSIGN == param2->param_type)
    {
        return print_error("varAssign", "Parameter 2 must have FIXED modifier");
    }
    if (SC_FALSE == sc_memory_is_element(context, param2->addr))
    {
        return print_error("varAssign", "Parameter 2 has not value");
    }
    param1->addr = param2->addr;
    return SCP_RESULT_TRUE;
}

scp_result ifFormCont(sc_memory_context *context, scp_operand *param)
{
    sc_stream *stream;
    if (SCP_ASSIGN == param->param_type)
    {
        return print_error("ifFormCont", "Parameter must have FIXED modifier");
    }
    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_error("ifFormCont", "Parameter has not value");
    }
    if (check_type(context, param->addr, scp_type_link) == SCP_RESULT_FALSE)
    {
        return print_error("ifFormCont", "Parameter must have link type");
    }
    sc_result res = sc_memory_get_link_content(context, param->addr, &stream);
    if (res == SC_RESULT_OK)
    {
        sc_stream_free(stream);
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
    return SCP_RESULT_ERROR;
}

scp_result contAssign(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    sc_stream *stream;
    sc_char *data;
    sc_uint32 length, read_length;
    if (SCP_ASSIGN == param2->param_type)
    {
        return print_error("contAssign", "Parameter 2 must have FIXED modifier");
    }
    if (SC_FALSE == sc_memory_is_element(context, param2->addr))
    {
        return print_error("contAssign", "Parameter 2 has not value");
    }
    if (check_type(context, param2->addr, scp_type_link) == SCP_RESULT_FALSE)
    {
        return print_error("contAssign", "Parameter 2 must have link type");
    }
    if (sc_memory_get_link_content(context, param2->addr, &stream) != SC_RESULT_OK)
    {
        return print_error("contAssign", "Parameter 2 content reading error");
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contAssign", param1))
    {
        return SCP_RESULT_ERROR;
    }
    sc_stream_get_length(stream, &length);
    data = calloc(length, sizeof(sc_char));
    sc_stream_read_data(stream, data, length, &read_length);
    sc_stream_free(stream);

    sc_stream_memory_new(data, length, SC_STREAM_FLAG_READ, SC_FALSE);
    sc_memory_set_link_content(context, param1->addr, stream);
    sc_stream_free(stream);
    return SCP_RESULT_TRUE;
}

scp_result contErase(sc_memory_context *context, scp_operand *param1)
{
    return SCP_RESULT_TRUE;
}

#ifdef SCP_MATH
scp_result ifEq(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num1, num2;
    if (SCP_RESULT_ERROR == resolve_numbers_1_2(context, "ifEq", param1, param2, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (num1 == num2)
    {
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
    return SCP_RESULT_ERROR;
}
#endif

#ifdef SCP_MATH
scp_result ifGr(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num1, num2;
    if (SCP_RESULT_ERROR == resolve_numbers_1_2(context, "ifGr", param1, param2, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (num1 > num2)
    {
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
    return SCP_RESULT_ERROR;
}
#endif

#ifdef SCP_MATH
scp_result contLn(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;
    if (SCP_RESULT_ERROR == resolve_number_2(context, "contLn", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contLn", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, log(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contCos(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;
    if (SCP_RESULT_ERROR == resolve_number_2(context, "contCos", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contCos", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, cos(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contSin(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;
    if (SCP_RESULT_ERROR == resolve_number_2(context, "contSin", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contSin", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, sin(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contTg(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;

    if (SCP_RESULT_ERROR == resolve_number_2(context, "contTg", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contTg", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, tan(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contACos(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;

    if (SCP_RESULT_ERROR == resolve_number_2(context, "contACos", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contACos", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, acos(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contASin(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contASin", param1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == resolve_number_2(context, "contASin", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, asin(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contATg(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    double num;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contATg", param1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == resolve_number_2(context, "contATg", param2, &num))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, atan(num), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contAdd(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;
    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contAdd", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contAdd", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, num1 + num2, param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contSub(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;

    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contSub", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contSub", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, num1 - num2, param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contMult(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;

    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contMult", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contMult", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, num1 * num2, param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contDiv(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;

    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contDiv", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contDiv", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, num1 / num2, param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contDivInt(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;
    int num1int, num2int;

    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contDivInt", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contDivInt", param1))
    {
        return SCP_RESULT_ERROR;
    }
    num1int = (int)num1;
    num2int = (int)num2;
    write_link_content_number(context, num1int / num2int, param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result contDivRem(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;
    int num1int, num2int;

    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contDivRem", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contDivRem", param1))
    {
        return SCP_RESULT_ERROR;
    }
    num1int = (int)num1;
    num2int = (int)num2;
    write_link_content_number(context, num1int % num2int, param1->addr);
    return SCP_RESULT_TRUE;
}
#endif


#ifdef SCP_MATH
scp_result contPow(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    double num1, num2;

    if (SCP_RESULT_ERROR == resolve_numbers_2_3(context, "contPow", param2, param3, &num1, &num2))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contPow", param1))
    {
        return SCP_RESULT_ERROR;
    }
    write_link_content_number(context, pow(num1, num2), param1->addr);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result contStringConcat(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    char *str1, *str2;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "contStringConcat", param1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_strings_2_3(context, "contStringConcat", param2, param3, &str1, &str2))
    {
        return SCP_RESULT_ERROR;
    }

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    char * result = (char*)malloc(len1 + len2 + 1);
    if (result == NULL) {
        print_error("contStringConcat", "unable to allocate memory");
        free(str1);
        free(str2);
        return SCP_RESULT_ERROR;
    }
    memcpy(result, str1, len1);
    memcpy(result+len1, str2, len2 + 1);

    write_link_content_string(context, result, param1->addr);

    free(str1);
    free(str2);
    free(result);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result stringIfEq(sc_memory_context *context, scp_operand *param1, scp_operand *param2) {
    char *str1, *str2;
    scp_result result;

    if (SCP_RESULT_ERROR == resolve_strings_1_2(context, "stringIfEq", param1, param2, &str1, &str2))
    {
        return SCP_RESULT_ERROR;
    }
    if (strcmp(str1, str2) == 0)
    {
        result = SCP_RESULT_TRUE;
    }
    else
    {
        result = SCP_RESULT_FALSE;
    }

    free(str1);
    free(str2);
    return result;
}
#endif

#ifdef SCP_STRING
scp_result stringIfGr(sc_memory_context *context, scp_operand *param1, scp_operand *param2) {
    char *str1, *str2;
    scp_result result;

    if (SCP_RESULT_ERROR == resolve_strings_1_2(context, "stringIfGr", param1, param2, &str1, &str2))
    {
        return SCP_RESULT_ERROR;
    }
    if (strcmp(str1, str2) > 0)
    {
        result = SCP_RESULT_TRUE;
    }
    else
    {
        result = SCP_RESULT_FALSE;
    }

    free(str1);
    free(str2);
    return result;
}
#endif

#ifdef SCP_STRING
scp_result stringSplit(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3) {
    sc_addr rrel_1, nrel_substring_base_order, prev_link, link, arc;
    char *str, *delimiter_str;
    char **result_set;
    int i, result_set_length;

    if (SC_RESULT_ERROR == sc_helper_resolve_system_identifier(context, "rrel_1", &rrel_1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SC_RESULT_ERROR == sc_helper_resolve_system_identifier(context, "nrel_substring_base_order", &nrel_substring_base_order))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == check_node_parameter_1(context, "stringSplit", param1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_strings_2_3(context, "stringSplit", param2, param3, &str, &delimiter_str))
    {
        return SCP_RESULT_ERROR;
    }

    result_set = g_strsplit(str, delimiter_str, -1);
    result_set_length = g_strv_length(result_set);

    for (i = 0; i < result_set_length; ++i)
    {
        link = sc_memory_link_new(context);
        write_link_content_string(context, result_set[i], link);
        arc = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, param1->addr, link);
        if (i == 0)
        {
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, rrel_1, arc);
        }
        else
        {
            arc = sc_memory_arc_new(context, sc_type_arc_common|sc_type_const, prev_link, link);
            sc_memory_arc_new(context, sc_type_arc_pos_const_perm, nrel_substring_base_order, arc);
        }
        prev_link = link;
    }

    g_strfreev(result_set);
    free(str);
    free(delimiter_str);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result stringLen(sc_memory_context *context, scp_operand *param1, scp_operand *param2) {
    char *str;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringLen", param1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string_2(context, "stringLen", param2, &str))
    {
        return SCP_RESULT_ERROR;
    }

    write_link_content_number(context, strlen(str), param1->addr);

    free(str);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result stringSub(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3)
{
    char *str1, *str2;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringSub", param1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == resolve_strings_2_3(context, "stringSub", param2, param3, &str1, &str2))
    {
        return SCP_RESULT_ERROR;
    }

    int position = -1;
    scp_result result = SCP_RESULT_FALSE;

    char *sub_string = strstr(str1, str2);

    if (sub_string != NULL) {
        position = sub_string - str1;
        result = SCP_RESULT_TRUE;
    }

    write_link_content_number(context, position, param1->addr);

    free(str1);
    free(str2);

    return result;
}
#endif

#ifdef SCP_STRING
scp_result stringSlice(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4)
{
    char *str, *sub_string;
    double num3, num4;
    int start_index, end_index;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringSlice", param1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string_2(context, "stringSlice", param2, &str))
    {
        return SCP_RESULT_ERROR;
    }


    if (SCP_RESULT_ERROR == resolve_number(context, "stringSlice", "Parameter 3", param3, &num3))
    {
        free(str);
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_number(context, "stringSlice", "Parameter 4", param4, &num4))
    {
        free(str);
        return SCP_RESULT_ERROR;
    }

    size_t length = strlen(str);
    start_index = (int)num3;
    end_index = (int)num4;

    if (start_index > end_index)
    {
        print_error("stringSlice", "invalid range");
        free(str);
        return SCP_RESULT_ERROR;
    }

    if (length <= end_index)
    {
        print_error("stringSlice", "end index out of range");
        free(str);
        return SCP_RESULT_ERROR;
    }

    int sub_string_length = end_index - start_index + 1;

    sub_string = (char*)malloc(sub_string_length);
    if (sub_string == NULL)
    {
        print_error("stringSlice", "unable to allocate memory");
        free(str);
        return SCP_RESULT_ERROR;
    }
    strncpy(sub_string, str + start_index, sub_string_length);

    write_link_content_string(context, sub_string, param1->addr);

    free(str);
    free(sub_string);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result stringStartsWith(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    char *str1, *str2;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringStartsWith", param1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == resolve_strings_1_2(context, "stringStartsWith", param1, param2, &str1, &str2))
    {
        return SCP_RESULT_ERROR;
    }

    scp_result result = SCP_RESULT_FALSE;

    size_t len_sub = strlen(str2), len_source = strlen(str1);
    if (len_source < len_sub) {
        print_error("stringStartsWith", "Length of parameter 1 should be greater than length of parameter 2");
        result = SCP_RESULT_ERROR;
    }

    if (strncmp(str1, str2, len_sub) == 0) {
        result = SCP_TRUE;
    }

    free(str1);
    free(str2);

    return result;
}
#endif

#ifdef SCP_STRING
scp_result stringEndsWith(sc_memory_context *context, scp_operand *param1, scp_operand *param2)
{
    char *str1, *str2;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringEndsWith", param1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == resolve_strings_1_2(context, "stringEndsWith", param1, param2, &str1, &str2))
    {
        return SCP_RESULT_ERROR;
    }

    scp_result result = SCP_RESULT_FALSE;

    size_t len_sub = strlen(str2), len_source = strlen(str1);
    if (len_source < len_sub) {
        print_error("stringEndsWith", "Length of parameter 1 should be greater than length of parameter 2");
        result = SCP_RESULT_ERROR;
    }

    if (0 == strcmp(str1 + len_source - len_sub, str2)) {
        result = SCP_TRUE;
    }

    free(str1);
    free(str2);

    return result;
}
#endif

#ifdef SCP_STRING
scp_result stringReplace(sc_memory_context *context, scp_operand *param1, scp_operand *param2, scp_operand *param3, scp_operand *param4)
{
    char *str1, *str2, *str3;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringReplace", param1))
    {
        return SCP_RESULT_ERROR;
    }
    if (SCP_RESULT_ERROR == resolve_strings_2_3_4(context, "stringReplace", param2, param3, param4, &str1, &str2, &str3))
    {
        return SCP_RESULT_ERROR;
    }

    char *result, *ins, *tmp, *source_string = str1;
    size_t len_str2, len_str3, len_front;
    int count;  // number of replacements

    len_str2 = strlen(str2);
    len_str3 = strlen(str3);

    ins = source_string;
    for (count = 0; tmp = strstr(ins, str2); ++count) {
        ins = tmp + len_str2;
    }

    tmp = result = malloc(strlen(source_string) + (len_str3 - len_str2) * count + 1);

    if (!result)
    {
        print_error("stringReplace", "Unable to allocate memeory");
        free(str1);
        free(str2);
        free(str3);
        return SCP_RESULT_ERROR;
    }
    while (count--) {
        ins = strstr(source_string, str2);
        len_front = ins - source_string;
        tmp = strncpy(tmp, source_string, len_front) + len_front;
        tmp = strcpy(tmp, str3) + len_str3;
        source_string += len_front + len_str2; // move to next "end of str2"
    }
    strcpy(tmp, source_string);
    write_link_content_string(context, result, param1->addr);

    free(str1);
    free(str2);
    free(str3);
    free(result);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result stringToUpperCase(sc_memory_context *context, scp_operand *param1, scp_operand *param2) {
    char *str, *result;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringToUpperCase", param1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string_2(context, "stringToUpperCase", param2, &str))
    {
        return SCP_RESULT_ERROR;
    }

    result = g_ascii_strup(str, -1);
    write_link_content_string(context, result, param1->addr);

    free(str);
    free(result);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result stringToLowerCase(sc_memory_context *context, scp_operand *param1, scp_operand *param2) {
    char *str, *result;

    if (SCP_RESULT_ERROR == check_link_parameter_1(context, "stringToUpperCase", param1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string_2(context, "stringToUpperCase", param2, &str))
    {
        return SCP_RESULT_ERROR;
    }

    result = g_ascii_strdown(str, -1);
    write_link_content_string(context, result, param1->addr);

    free(str);
    free(result);

    return SCP_RESULT_TRUE;
}
#endif
