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
#include "scp_operator_interpreter_agents.h"
#include "scp_operator_interpreter_functions.h"
#include "scp_interpreter_utils.h"
#include "scp_keynodes.h"
#include "scp_utils.h"
#include "malloc.h"

#include <stdio.h>

sc_event *event_interpreting_finished_succesfully;

sc_event *event_gen_operators_interpreter;
sc_event *event_search_operators_interpreter;
sc_event *event_erase_operators_interpreter;
sc_event *event_return_operator_interpreter;
sc_event *event_call_operator_interpreter;
sc_event *event_print_operators_interpreter;
#ifdef SCP_MATH
sc_event *event_content_arithmetic_operators_interpreter;
sc_event *event_content_trig_operators_interpreter;
#endif
#ifdef SCP_STRING
sc_event *event_content_string_operators_interpreter;
#endif
sc_event *event_if_operators_interpreter;
sc_event *event_other_operators_interpreter;
sc_event *event_system_operators_interpreter;
sc_event *event_event_operators_interpreter;
sc_event *event_waitReturn_operators_interpreter;
sc_event *event_syncronize_operators_interpreter;

sc_event *event_register_sc_agent;
sc_event *event_unregister_sc_agent;

GHashTable *scp_event_table;
GHashTable *scp_wait_event_table;

void print_debug_info(const char *info)
{
#ifdef SCP_DEBUG
    printf(">>%s\n", info);
#endif
}

void print_call_debug_info(sc_memory_context *context, scp_operand *proc)
{
#ifdef SCP_DEBUG
    scp_operand arc1, arc2, name;
    MAKE_COMMON_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(name);
    searchElStr5(context, proc, &arc1, &name, &arc2, &nrel_system_identifier);
    printf("====called program: ");
    printNl(context, &name);
#endif
}

scp_result resolve_sc_agent_event_type(sc_memory_context *context, scp_operand *event_type, sc_event_type *result)
{
    if (SCP_TRUE == ifCoin(context, event_type, &sc_event_add_output_arc))
    {
        (*result) = SC_EVENT_ADD_OUTPUT_ARC;
        return SCP_TRUE;
    }
    if (SCP_TRUE == ifCoin(context, event_type, &sc_event_add_input_arc))
    {
        (*result) = SC_EVENT_ADD_INPUT_ARC;
        return SCP_TRUE;
    }
    if (SCP_TRUE == ifCoin(context, event_type, &sc_event_remove_output_arc))
    {
        (*result) = SC_EVENT_REMOVE_OUTPUT_ARC;
        return SCP_TRUE;
    }
    if (SCP_TRUE == ifCoin(context, event_type, &sc_event_remove_input_arc))
    {
        (*result) = SC_EVENT_REMOVE_INPUT_ARC;
        return SCP_TRUE;
    }
    /*if (SCP_TRUE == ifCoin(event_type, &sc_event_change_link_content))
    {
        (*result) = SC_EVENT_CHANGE_LINK_CONTENT;
        return SCP_TRUE;
    }*/
    return SCP_FALSE;
}

sc_result interpreter_agent_search_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }
    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //searchElStr3 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_searchElStr3))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("searchElStr3");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = searchElStr3(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 3);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }
    //searchElStr5 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_searchElStr5))
    {
        scp_operand operands[5], operand_values[5];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("searchElStr5");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 5);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 5))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = searchElStr5(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3, operand_values + 4);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 5);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }
    //searchSetStr3 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_searchSetStr3))
    {
        scp_operand operands[3], operand_values[3], sets[3], sets_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("searchSetStr3");

        resolve_operands_modifiers_with_set(s_default_ctx, &operator_node, operands, sets, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }
        if (SCP_RESULT_TRUE != get_set_operands_values(s_default_ctx, sets, sets_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = searchSetStr3(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, sets_values);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 3);
                set_set_operands_values(s_default_ctx, sets, sets_values, 3);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }
    //searchSetStr5 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_searchSetStr5))
    {
        scp_operand operands[5], operand_values[5], sets[5], sets_values[5];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("searchSetStr5");

        resolve_operands_modifiers_with_set(s_default_ctx, &operator_node, operands, sets, 5);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 5))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }
        if (SCP_RESULT_TRUE != get_set_operands_values(s_default_ctx, sets, sets_values, 5))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = searchSetStr5(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3, operand_values + 4, sets_values);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 5);
                set_set_operands_values(s_default_ctx, sets, sets_values, 5);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_gen_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //genEl case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_genEl))
    {
        scp_operand operand, operand_value;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("genEl");

        resolve_operands_modifiers(s_default_ctx, &operator_node, &operand, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, &operand, &operand_value, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = genEl(s_default_ctx, &operand_value);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, &operand, &operand_value, 1);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //genElStr3 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_genElStr3))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("genElStr3");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = genElStr3(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //genElStr5 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_genElStr5))
    {
        scp_operand operands[5], operand_values[5];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("genElStr5");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 5);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 5))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = genElStr5(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3, operand_values + 4);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 5);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_erase_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //eraseEl case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_eraseEl))
    {
        scp_operand operand, operand_value;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("eraseEl");

        resolve_operands_modifiers(s_default_ctx, &operator_node, &operand, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, &operand, &operand_value, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = eraseEl(s_default_ctx, &operand_value);
        if (res == SCP_RESULT_TRUE)
        {
            goto_unconditional(s_default_ctx, &operator_node);
        }
        else
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }
        return SC_RESULT_OK;
    }

    //eraseElStr3 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_eraseElStr3))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("eraseElStr3");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = eraseElStr3(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //eraseElStr5 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_eraseElStr5))
    {
        scp_operand operands[5], operand_values[5];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("eraseElStr5");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 5);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 5))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = eraseElStr5(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3, operand_values + 4);
        if (res == SCP_RESULT_TRUE)
        {
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //eraseSetStr3 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_eraseSetStr3))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("eraseSetStr3");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = eraseSetStr3(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //eraseSetStr5 case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_eraseSetStr5))
    {
        scp_operand operands[5], operand_values[5];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("eraseSetStr5");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 5);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 5))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = eraseSetStr5(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3, operand_values + 4);
        if (res == SCP_RESULT_TRUE)
        {
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}

#ifdef SCP_MATH
sc_result interpreter_agent_content_arithmetic_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //contAdd case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contAdd))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contAdd");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contAdd(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contSub case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contSub))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contSub");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contSub(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contMult case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contMult))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contMult");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contMult(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contDiv case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contDiv))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contDiv");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contDiv(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contDivInt case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contDivInt))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contDivInt");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contDivInt(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contDivRem case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contDivRem))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contDivRem");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contDivRem(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contPow case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contPow))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contPow");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contPow(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contLn case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contLn))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contLn");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contLn(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //ifEq case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_ifEq))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("ifEq");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = ifEq(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }
    //ifGr case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_ifGr))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("ifGr");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = ifGr(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }

    return SC_RESULT_ERROR;
}
#endif

#ifdef SCP_MATH
sc_result interpreter_agent_content_trig_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //contSin case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contSin))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contSin");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contSin(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contCos case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contCos))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contCos");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contCos(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contTg case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contTg))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contTg");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contTg(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contASin case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contASin))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contASin");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contASin(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contACos case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contACos))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contACos");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contACos(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contATg case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contATg))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contATg");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contATg(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    return SC_RESULT_ERROR;
}
#endif

#ifdef SCP_STRING
sc_result interpreter_agent_content_string_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //contStringConcat case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contStringConcat))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contStringConcat");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contStringConcat(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //stringIfEq case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_stringIfEq))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringIfEq");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringIfEq(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }

    //stringIfGr case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_stringIfGr))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringIfGr");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringIfGr(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }

    //stringSplit case
    if (SC_RESULT_OK == ifCoin(s_default_ctx, &operator_type, &op_stringSplit))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringSplit");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringSplit(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 3);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //stringLen case
    if (SC_RESULT_OK == ifCoin(s_default_ctx, &operator_type, &op_stringLen))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringLen");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringLen(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //stringSub case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_stringSub))
    {
        scp_operand operands[3], operand_values[3];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringSub");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 3))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringSub(s_default_ctx, operand_values, operand_values + 1, operand_values + 2);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 3);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 3);
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }

        return SC_RESULT_OK;
    }

    //stringSlice case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_stringSlice))
    {
        scp_operand operands[4], operand_values[4];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringSlice");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 4);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 4))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringSlice(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 4);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //stringStartsWith case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_stringStartsWith))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringStartsWith");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringStartsWith(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 2);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 2);
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }

        return SC_RESULT_OK;
    }

    //stringEndsWith case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_stringEndsWith))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringEndsWith");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringEndsWith(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 2);
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                set_operands_values(s_default_ctx, operands, operand_values, 2);
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }

        return SC_RESULT_OK;
    }

    //stringReplace case
    if (SC_RESULT_OK == ifCoin(s_default_ctx, &operator_type, &op_stringReplace))
    {
        scp_operand operands[4], operand_values[4];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringReplace");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 4);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 4))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringReplace(s_default_ctx, operand_values, operand_values + 1, operand_values + 2, operand_values + 3);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 4);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //stringToUpperCase case
    if (SC_RESULT_OK == ifCoin(s_default_ctx, &operator_type, &op_stringToUpperCase))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringToUpperCase");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringToUpperCase(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }

        return SC_RESULT_OK;
    }

    //stringToLowerCase case
    if (SC_RESULT_OK == ifCoin(s_default_ctx, &operator_type, &op_stringToLowerCase))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("stringToLowerCase");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = stringToLowerCase(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }

        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}
#endif

sc_result interpreter_agent_if_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //ifVarAssign case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_ifVarAssign))
    {
        scp_operand operand, operand_value;
        scp_param_type type;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("ifVarAssign");

        resolve_operands_modifiers(s_default_ctx, &operator_node, &operand, 1);
        type = operand.param_type;

        //Operator body
        if (operand.operand_type == SCP_CONST)
        {
            if (type == SCP_ASSIGN)
            {
                printEl(s_default_ctx, &operand);
                print_error("scp-operator interpreting", "Constant has ASSIGN modifier");
                operator_interpreting_crash(s_default_ctx, &operator_node);
                return SCP_RESULT_ERROR;
            }
            res = ifVarAssign(s_default_ctx, &operand);
        }
        else
        {
            scp_operand arc3, arc2;
            MAKE_DEFAULT_ARC_ASSIGN(arc2);
            MAKE_COMMON_ARC_ASSIGN(arc3);
            MAKE_DEFAULT_OPERAND_ASSIGN(operand_value);

            operand.param_type = SCP_FIXED;
            if (SCP_RESULT_TRUE == searchElStr5(s_default_ctx, &operand, &arc3, &operand_value, &arc2, &nrel_value))
            {
                res = ifVarAssign(s_default_ctx, &operand_value);
            }
            else
            {
                res = SCP_RESULT_FALSE;
            }
            if (type == SCP_ASSIGN)
            {
                operand_value.param_type = SCP_ASSIGN;
                eraseElStr5(s_default_ctx, &operand, &arc3, &operand_value, &arc2, &nrel_value);
            }
        }

        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_OK;
    }
    //ifFormCont case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_ifFormCont))
    {
        scp_operand operands, operand_values;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("ifFormCont");

        resolve_operands_modifiers(s_default_ctx, &operator_node, &operands, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, &operands, &operand_values, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = ifFormCont(s_default_ctx, &operand_values);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_OK;
    }
    //ifType case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_ifType))
    {
        scp_operand operands, operand_values;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("ifType");

        resolve_operands_modifiers(s_default_ctx, &operator_node, &operands, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, &operands, &operand_values, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body

        res = ifType(s_default_ctx, &operand_values);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_OK;
    }
    //ifCoin case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_ifCoin))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("ifCoin");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = ifCoin(s_default_ctx, operand_values, operand_values + 1);
        switch (res)
        {
            case SCP_RESULT_TRUE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_success(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_FALSE:
            {
                if (SCP_RESULT_TRUE != goto_conditional_unsuccess(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
            case SCP_RESULT_ERROR:
            {
                if (SCP_RESULT_TRUE != goto_conditional_error(s_default_ctx, &operator_node))
                {
                    return SC_RESULT_ERROR;
                }
                return SC_RESULT_OK;
            }
        }
        return SC_RESULT_ERROR;
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_other_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;
    scp_result res;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //varAssign case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_varAssign))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("varAssign");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = varAssign(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contAssign case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contAssign))
    {
        scp_operand operands[2], operand_values[2];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contAssign");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 2))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contAssign(s_default_ctx, operand_values, operand_values + 1);
        if (res == SCP_RESULT_TRUE)
        {
            set_operands_values(s_default_ctx, operands, operand_values, 2);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }
    //contErase case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_contErase))
    {
        scp_operand operands, operand_values;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("contErase");

        resolve_operands_modifiers(s_default_ctx, &operator_node, &operands, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, &operands, &operand_values, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        res = contErase(s_default_ctx, &operand_values);
        if (res == SCP_RESULT_TRUE)
        {
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_return_operator(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //return case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_return))
    {
        scp_operand scp_process_node, arc1, arc2, quest;
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("return");
        MAKE_COMMON_ARC_ASSIGN(arc1);
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_DEFAULT_NODE_ASSIGN(quest);
        find_scp_process_for_scp_operator(s_default_ctx, &operator_node, &scp_process_node);
        searchElStr5(s_default_ctx, &quest, &arc1, &scp_process_node, &arc2, &nrel_scp_process);
        quest.param_type = SCP_FIXED;
        finish_question_successfully(s_default_ctx, &quest);
        scp_process_node.param_type = SCP_FIXED;
        mark_scp_process_as_useless(s_default_ctx, &scp_process_node);
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_call_operator(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //call case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_call))
    {
        scp_operand arc1, arc2, arc3, quest, modifier, new_params_node,
                    prog_params_node, param, curr_ordinal, value, operands[3], operands_values[3];
        scp_iterator3 *it, *it1;
        scp_operand_type operand_type = SCP_CONST;
        sc_char in_out_params[ORDINAL_RRELS_COUNT + 1];
        sc_char in_out_value = 0;
        scp_uint32 ordinal_index = 0;
        scp_param_type param_type = SCP_FIXED;

        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("call");

        MAKE_DEFAULT_ARC_ASSIGN(arc1);
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_COMMON_ARC_ASSIGN(arc3);
        MAKE_DEFAULT_OPERAND_ASSIGN(prog_params_node);
        MAKE_DEFAULT_OPERAND_ASSIGN(param);
        MAKE_DEFAULT_OPERAND_ASSIGN(value);
        MAKE_DEFAULT_OPERAND_ASSIGN(curr_ordinal);
        MAKE_DEFAULT_OPERAND_ASSIGN(modifier);

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 3);
        get_operands_values(s_default_ctx, operands, operands_values, 3);
        operands[2].param_type = SCP_FIXED;
        print_call_debug_info(s_default_ctx, operands_values);

        searchElStr5(s_default_ctx, operands_values, &arc1, &prog_params_node, &arc2, &rrel_params);
        prog_params_node.param_type = SCP_FIXED;
        erase_var_set_values(s_default_ctx, operands_values + 1);

        it = scp_iterator3_new(s_default_ctx, &prog_params_node, &arc1, &param);
        while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it, &prog_params_node, &arc1, &param))
        {
            arc1.param_type = SCP_FIXED;
            in_out_value = 0;
            ordinal_index = 0;
            it1 = scp_iterator3_new(s_default_ctx, &modifier, &arc2, &arc1);
            while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it1, &modifier, &arc2, &arc1))
            {
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &modifier, &rrel_in))
                {
                    in_out_value++;
                    continue;
                }
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &modifier, &rrel_out))
                {
                    in_out_value += 2;
                    continue;
                }

                // Ordinal
                if (ordinal_index == 0)
                {
                    modifier.param_type = SCP_FIXED;
                    ordinal_index = check_ordinal_rrel(s_default_ctx, &modifier, ORDINAL_RRELS_COUNT);
                    modifier.param_type = SCP_ASSIGN;
                }
            }
            scp_iterator3_free(it1);
            in_out_params[ordinal_index] = in_out_value;
            arc1.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it);

        MAKE_DEFAULT_NODE_ASSIGN(quest);
        MAKE_DEFAULT_NODE_ASSIGN(new_params_node);
        arc1.erase = SCP_TRUE;
        eraseSetStr5(s_default_ctx, operands + 2, &arc3, &quest, &arc2, &nrel_value);
        genElStr5(s_default_ctx, &quest, &arc1, operands_values, &arc2, &ordinal_rrels[1]);
        quest.param_type = SCP_FIXED;
        genElStr5(s_default_ctx, operands + 2, &arc3, &quest, &arc2, &nrel_value);
        genElStr5(s_default_ctx, &quest, &arc1, &new_params_node, &arc2, &ordinal_rrels[2]);
        new_params_node.param_type = SCP_FIXED;

        param.param_type = SCP_ASSIGN;
        it = scp_iterator3_new(s_default_ctx, operands_values + 1, &arc1, &param);
        while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it, operands_values + 1, &arc1, &param))
        {
            param.param_type = SCP_FIXED;
            arc1.param_type = SCP_FIXED;
            operand_type = SCP_CONST;
            param_type = SCP_FIXED;
            curr_ordinal.param_type = SCP_ASSIGN;
            modifier.param_type = SCP_ASSIGN;
            it1 = scp_iterator3_new(s_default_ctx, &modifier, &arc2, &arc1);
            while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it1, &modifier, &arc2, &arc1))
            {
                modifier.param_type = SCP_FIXED;
                // Param type
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &rrel_scp_const, &modifier))
                {
                    operand_type = SCP_CONST;
                    modifier.param_type = SCP_ASSIGN;
                    continue;
                }
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &rrel_scp_var, &modifier))
                {
                    operand_type = SCP_VAR;
                    modifier.param_type = SCP_ASSIGN;
                    continue;
                }

                // Operand type
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &rrel_fixed, &modifier))
                {
                    param_type = SCP_FIXED;
                    modifier.param_type = SCP_ASSIGN;
                    continue;
                }
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &rrel_assign, &modifier))
                {
                    param_type = SCP_ASSIGN;
                    modifier.param_type = SCP_ASSIGN;
                    continue;
                }

                // Ordinal
                if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &order_role_relation, &arc2, &modifier))
                {
                    curr_ordinal.param_type = SCP_ASSIGN;
                    varAssign(s_default_ctx, &curr_ordinal, &modifier);
                    curr_ordinal.param_type = SCP_FIXED;
                }
                modifier.param_type = SCP_ASSIGN;
            }
            scp_iterator3_free(it1);
            arc1.param_type = SCP_ASSIGN;

            ordinal_index = check_ordinal_rrel(s_default_ctx, &curr_ordinal, ORDINAL_RRELS_COUNT);
            if (in_out_params[ordinal_index] == 1)
            {
                if (param_type == SCP_ASSIGN)
                {
                    print_error("scp-operator interpreting", "In-parameter has ASSIGN modifier");
                    printf("wrong call parameter - %d\n", ordinal_index);
                    operator_interpreting_crash(s_default_ctx, &operator_node);
                    return SC_RESULT_ERROR;
                }
                if (operand_type == SCP_CONST)
                {
                    genElStr5(s_default_ctx, &new_params_node, &arc1, &param, &arc2, &curr_ordinal);
                }
                else
                {
                    value.param_type = SCP_ASSIGN;
                    if (SCP_RESULT_TRUE != searchElStr5(s_default_ctx, &param, &arc3, &value, &arc2, &nrel_value))
                    {
                        print_error("scp-operator interpreting", "Variable has FIXED modifier, but has no value");
                        printf("wrong call parameter - %d\n", ordinal_index);
                        operator_interpreting_crash(s_default_ctx, &operator_node);
                        return SC_RESULT_ERROR;
                    }
                    value.param_type = SCP_FIXED;
                    genElStr5(s_default_ctx, &new_params_node, &arc1, &value, &arc2, &curr_ordinal);
                }
            }
            else
            {
                if (param_type == SCP_ASSIGN)
                {
                    eraseElStr5(s_default_ctx, &param, &arc3, &value, &arc1, &nrel_value);
                }
                else
                {
                    if (operand_type == SCP_VAR && SCP_RESULT_TRUE != searchElStr5(s_default_ctx, &param, &arc3, &value, &arc2, &nrel_value))
                    {
                        print_error("scp-operator interpreting", "Variable has FIXED modifier, but has no value");
                        printf("wrong call parameter - %d\n", ordinal_index);
                        operator_interpreting_crash(s_default_ctx, &operator_node);
                        return SC_RESULT_ERROR;
                    }
                }
                genElStr5(s_default_ctx, &new_params_node, &arc1, &param, &arc2, &curr_ordinal);
            }

            param.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it);

        genElStr3(s_default_ctx, &question_scp_interpretation_request, &arc1, &quest);
        set_author(s_default_ctx, &quest, &abstract_scp_machine);
        genElStr3(s_default_ctx, &question_initiated, &arc1, &quest);
        goto_unconditional(s_default_ctx, &operator_node);
    }

    return SC_RESULT_OK;
}

sc_result interpreter_agent_waitReturn_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //waitReturn case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_waitReturn))
    {
        scp_operand arc1, arc2, quest, descr_node, params, authors;

        print_debug_info("waitReturn");

        MAKE_DEFAULT_ARC_ASSIGN(arc1);
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_DEFAULT_OPERAND_ASSIGN(descr_node);
        MAKE_DEFAULT_OPERAND_ASSIGN(quest);

        resolve_operands_modifiers(s_default_ctx, &operator_node, &descr_node, 1);
        get_operands_values(s_default_ctx, &descr_node, &quest, 1);
        if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &question_finished_successfully, &arc1, &quest))
        {
            quest.param_type = SCP_FIXED;

            MAKE_COMMON_ARC_ASSIGN(arc1);
            MAKE_DEFAULT_OPERAND_ASSIGN(authors);
            authors.erase = SCP_TRUE;
            quest.erase = SCP_FALSE;
            eraseElStr5(s_default_ctx, &authors, &arc1, &quest, &arc2, &nrel_authors);
            MAKE_DEFAULT_ARC_ASSIGN(arc1);

            MAKE_DEFAULT_OPERAND_ASSIGN(params);
            params.erase = SCP_TRUE;
            quest.erase = SCP_TRUE;
            eraseElStr5(s_default_ctx, &quest, &arc1, &params, &arc2, &ordinal_rrels[2]);
            input_arc.erase = SCP_TRUE;
            eraseEl(s_default_ctx, &input_arc);

            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    //waitReturnSet case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_waitReturnSet))
    {
        scp_operand arc1, arc2, quest, descr_set, params, authors;
        scp_iterator3 *it;

        print_debug_info("waitReturnSet");

        MAKE_DEFAULT_ARC_ASSIGN(arc1);
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_DEFAULT_OPERAND_ASSIGN(descr_set);
        MAKE_DEFAULT_OPERAND_ASSIGN(quest);
        MAKE_DEFAULT_OPERAND_ASSIGN(params);
        params.erase = SCP_TRUE;

        resolve_operands_modifiers(s_default_ctx, &operator_node, &descr_set, 1);
        get_operands_values(s_default_ctx, &descr_set, &quest, 1);
        it = scp_iterator3_new(s_default_ctx, &descr_set, &arc1, &quest);
        while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it, &descr_set, &arc1, &quest))
        {
            quest.param_type = SCP_FIXED;
            if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &question_finished_successfully, &arc1, &quest))
            {
                MAKE_COMMON_ARC_ASSIGN(arc1);
                MAKE_DEFAULT_OPERAND_ASSIGN(authors);
                authors.erase = SCP_TRUE;
                quest.erase = SCP_FALSE;
                eraseElStr5(s_default_ctx, &authors, &arc1, &quest, &arc2, &nrel_authors);
                MAKE_DEFAULT_ARC_ASSIGN(arc1);
                quest.erase = SCP_TRUE;
                eraseElStr5(s_default_ctx, &quest, &arc1, &params, &arc2, &ordinal_rrels[2]);
            }
            quest.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it);
        if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &descr_set, &arc1, &params))
        {
            descr_set.param_type = SCP_FIXED;
            descr_set.erase = SCP_TRUE;
            eraseEl(s_default_ctx, &descr_set);
            input_arc.erase = SCP_TRUE;
            eraseEl(s_default_ctx, &input_arc);
            goto_unconditional(s_default_ctx, &operator_node);
        }
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}

sc_result scp_event_procedure_processor(const sc_event *event, sc_addr arg)
{
    scp_operand arc1, arc2, agent_scp_program, quest, params, second_param, node, input_arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_NODE_ASSIGN(quest);
    MAKE_DEFAULT_OPERAND_ASSIGN(agent_scp_program);

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_OPERAND_ASSIGN(node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node, &input_arc, &quest))
    {
        return SC_RESULT_ERROR;
    }
    quest.param_type = SCP_FIXED;
    if (SCP_RESULT_TRUE == check_scp_interpreter_question(s_default_ctx, &quest))
    {
        return SC_RESULT_OK;
    }
    quest.param_type = SCP_ASSIGN;
    agent_scp_program.addr = resolve_sc_addr_from_int((scp_uint32)sc_event_get_data(event));
    agent_scp_program.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(quest);
    genElStr5(s_default_ctx, &quest, &arc1, &agent_scp_program, &arc2, ordinal_rrels + 1);
    quest.param_type = SCP_FIXED;
    MAKE_DEFAULT_NODE_ASSIGN(params);
    genElStr5(s_default_ctx, &quest, &arc1, &params, &arc2, ordinal_rrels + 2);
    params.param_type = SCP_FIXED;
    genElStr5(s_default_ctx, &params, &arc1, &agent_scp_program, &arc2, ordinal_rrels + 1);
    MAKE_DEFAULT_OPERAND_FIXED(second_param);
    second_param.addr = arg;
    genElStr5(s_default_ctx, &params, &arc1, &second_param, &arc2, ordinal_rrels + 2);

    genElStr3(s_default_ctx, &question_scp_interpretation_request, &arc1, &quest);
    set_author(s_default_ctx, &quest, &abstract_scp_machine);
    genElStr3(s_default_ctx, &question_initiated, &arc1, &quest);

    return SC_RESULT_OK;
}

sc_result sys_wait_processor(const sc_event *event, sc_addr arg)
{
    scp_operand operator_node, arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_DEFAULT_OPERAND_FIXED(operator_node);
    operator_node.addr = resolve_sc_addr_from_int((scp_uint32)sc_event_get_data(event));
    if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &active_scp_operator, &arc, &operator_node))
    {
        g_hash_table_remove(scp_wait_event_table, MAKE_HASH(operator_node));
        arc.param_type = SCP_FIXED;
        arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &arc);
        goto_unconditional(s_default_ctx, &operator_node);
    }
    return SC_RESULT_OK;
}

scp_result create_sys_wait_event(sc_memory_context *context, scp_operand *operands, scp_operand *operator_node)
{
    sc_event_type type;
    sc_event *event;

    if (SCP_RESULT_TRUE != resolve_scp_event_type(context, operands, &type))
    {
        return print_error("Event processing", "Can't resolve event type");
    }

    event = sc_event_new(context, operands[1].addr, type, (sc_pointer)SC_ADDR_LOCAL_TO_INT(operator_node->addr), (fEventCallback)sys_wait_processor, NULL);
    g_hash_table_insert(scp_wait_event_table, MAKE_PHASH(operator_node), (gpointer)event);
    return SC_RESULT_OK;
}

sc_result interpreter_agent_event_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        operator_interpreting_crash(s_default_ctx, &operator_node);
        return SC_RESULT_ERROR;
    }

    //sys_wait case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_sys_wait))
    {
        scp_operand operands[2], operands_values[2];
        print_debug_info("sys_wait");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 2);
        get_operands_values(s_default_ctx, operands, operands_values, 2);
        if (SCP_RESULT_TRUE != create_sys_wait_event(s_default_ctx, operands_values, &operator_node))
        {
            return SC_RESULT_ERROR;
        }
        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_print_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        return SC_RESULT_ERROR;
    }

    //print case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_print))
    {
        scp_operand operands[1], operand_values[1];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("print");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        print(s_default_ctx, operand_values);
        goto_unconditional(s_default_ctx, &operator_node);

        return SC_RESULT_OK;
    }
    //printNl case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_printNl))
    {
        scp_operand operands[1], operand_values[1];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("printNl");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        printNl(s_default_ctx, operand_values);
        goto_unconditional(s_default_ctx, &operator_node);

        return SC_RESULT_OK;
    }
    //printEl case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_printEl))
    {
        scp_operand operands[1], operand_values[1];
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("printEl");

        resolve_operands_modifiers(s_default_ctx, &operator_node, operands, 1);

        if (SCP_RESULT_TRUE != get_operands_values(s_default_ctx, operands, operand_values, 1))
        {
            operator_interpreting_crash(s_default_ctx, &operator_node);
            return SC_RESULT_ERROR;
        }

        //Operator body
        printEl(s_default_ctx, operand_values);
        goto_unconditional(s_default_ctx, &operator_node);

        return SC_RESULT_OK;
    }

    return SC_RESULT_ERROR;
}

sc_result sys_search_agent_body(sc_memory_context *context, scp_operand *operator_node)
{
    scp_operand param1, param2, param3, param2_node, param4, param4_node, operand, modifier, arc1, arc2, operand_elem;
    scp_operand **pair_elems2 = null_ptr, **pair_values2 = null_ptr, **pair_elems3 = null_ptr, **pair_values3 = null_ptr;
    scp_operand_pair *params = null_ptr, *results = null_ptr;
    scp_iterator3 *it0, *it, *it1;
    scp_uint32 rrel_number = 0, size2 = 0, size3 = 0, i = 0;
    scp_bool is4 = SCP_FALSE, isresult = SCP_FALSE;
    scp_result res;
    sc_result fun_result = SC_RESULT_OK;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand);
    MAKE_DEFAULT_NODE_ASSIGN(modifier);

    it0 = scp_iterator3_new(context, operator_node, &arc1, &operand);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it0, operator_node, &arc1, &operand))
    {
        arc1.param_type = SCP_FIXED;
        rrel_number = 0;
        it = scp_iterator3_new(context, &modifier, &arc2, &arc1);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &modifier, &arc2, &arc1))
        {
            if (rrel_number == 0)
            {
                rrel_number = check_ordinal_rrel(context, &modifier, 4);
                if (rrel_number > 0)
                {
                    continue;
                }
            }

            // Operand type
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_scp_const))
            {
                operand.operand_type = SCP_CONST;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_scp_var))
            {
                operand.operand_type = SCP_VAR;
                continue;
            }

            // Param type
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_assign))
            {
                operand.param_type = SCP_ASSIGN;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_fixed))
            {
                operand.param_type = SCP_FIXED;
                continue;
            }
        }
        scp_iterator3_free(it);

        if (rrel_number == 1)
        {
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param1))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }
        if (rrel_number == 2)
        {
            param2_node = operand;
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param2))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            if (param2.param_type == SCP_ASSIGN)
            {
                arc1.param_type = SCP_ASSIGN;
                MAKE_DEFAULT_OPERAND_ASSIGN(operand);
                continue;
            }
            isresult = SCP_TRUE;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand_elem);
            get_set_power(context, &param2, &size2);
            if (size2 == 0)
            {
                arc1.param_type = SCP_ASSIGN;
                MAKE_DEFAULT_OPERAND_ASSIGN(operand);
                continue;
            }
            results = (scp_operand_pair *)calloc(sizeof(scp_operand_pair), size2);
            pair_elems2 = (scp_operand **)calloc(sizeof(scp_operand), size2);
            pair_values2 = (scp_operand **)calloc(sizeof(scp_operand), size2);
            for (i = 0; i < size2; i++)
            {
                pair_elems2[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
                pair_values2[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
            }
            i = 0;
            it1 = scp_iterator3_new(context, &param2, &arc2, &operand_elem);
            while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &param2, &arc2, &operand_elem))
            {
                operand_elem.param_type = SCP_FIXED;
                resolve_operands_modifiers(context, &operand_elem, pair_elems2[i], 2);
                if (SCP_RESULT_TRUE != get_operands_values(context, pair_elems2[i], pair_values2[i], 2))
                {
                    operator_interpreting_crash(context, operator_node);
                    fun_result = SC_RESULT_ERROR;
                    goto exit;
                }
                results[i].operand1 = pair_values2[i];
                results[i].operand2 = pair_values2[i] + 1;
                operand_elem.param_type = SCP_ASSIGN;
                i++;
            }
            scp_iterator3_free(it1);
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }
        if (rrel_number == 3)
        {
            MAKE_DEFAULT_OPERAND_ASSIGN(operand_elem);
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param3))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            get_set_power(context, &param3, &size3);
            if (size3 == 0)
            {
                arc1.param_type = SCP_ASSIGN;
                MAKE_DEFAULT_OPERAND_ASSIGN(operand);
                continue;
            }
            params = (scp_operand_pair *)calloc(sizeof(scp_operand_pair), size3);
            pair_elems3 = (scp_operand **)calloc(sizeof(scp_operand), size3);
            pair_values3 = (scp_operand **)calloc(sizeof(scp_operand), size3);
            for (i = 0; i < size3; i++)
            {
                pair_elems3[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
                pair_values3[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
            }
            i = 0;
            it1 = scp_iterator3_new(context, &param3, &arc2, &operand_elem);
            while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &param3, &arc2, &operand_elem))
            {
                operand_elem.param_type = SCP_FIXED;
                resolve_operands_modifiers(context, &operand_elem, pair_elems3[i], 2);
                if (SCP_RESULT_TRUE != get_operands_values(context, pair_elems3[i], pair_values3[i], 2))
                {
                    operator_interpreting_crash(context, operator_node);
                    fun_result = SC_RESULT_ERROR;
                    goto exit;
                }
                params[i].operand1 = pair_values3[i];
                params[i].operand2 = pair_values3[i] + 1;
                operand_elem.param_type = SCP_ASSIGN;
                i++;
            }
            scp_iterator3_free(it1);
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }
        if (rrel_number == 4)
        {
            is4 = SCP_TRUE;
            param4_node = operand;
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param4))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }

        arc1.param_type = SCP_ASSIGN;
        MAKE_DEFAULT_OPERAND_ASSIGN(operand);
    }
    scp_iterator3_free(it0);
    if (isresult == SCP_TRUE)
    {
        if (is4 == SCP_TRUE)
        {
            res = scp_sys_search_for_variables(context, &param1, results, size2, params, size3, &param4);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    for (i = 0; i < size2; i++)
                    {
                        set_operands_values(context, pair_elems2[i] + 1, pair_values2[i] + 1, 2);
                    }
                    set_operands_values(context, &param4_node, &param4, 1);
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
        else
        {
            res = scp_sys_search_for_variables(context, &param1, results, size2, params, size3, null_ptr);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    for (i = 0; i < size2; i++)
                    {
                        set_operands_values(context, pair_elems2[i] + 1, pair_values2[i] + 1, 2);
                    }
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
    }
    else
    {
        if (is4 == SCP_TRUE)
        {
            res = scp_sys_search(context, &param1, &param2, params, size3, &param4, SCP_TRUE);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    set_operands_values(context, &param2_node, &param2, 1);
                    set_operands_values(context, &param4_node, &param4, 1);
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
        else
        {
            res = scp_sys_search(context, &param1, &param2, params, size3, null_ptr, SCP_TRUE);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    set_operands_values(context, &param2_node, &param2, 1);
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
    }
exit:
    //Memory deallocating
    for (i = 0; i < size2; i++)
    {
        free(pair_elems2[i]);
        free(pair_values2[i]);
    }
    for (i = 0; i < size3; i++)
    {
        free(pair_elems3[i]);
        free(pair_values3[i]);
    }
    free(pair_elems2);
    free(pair_values2);
    free(pair_elems3);
    free(pair_values3);
    free(results);
    free(params);
    return fun_result;
}

sc_result sys_gen_agent_body(sc_memory_context *context, scp_operand *operator_node)
{
    scp_operand param1, param2, param3, param2_node, param4, param4_node, operand, modifier, arc1, arc2, operand_elem;
    scp_operand **pair_elems2 = null_ptr, **pair_values2 = null_ptr, **pair_elems3 = null_ptr, **pair_values3 = null_ptr;
    scp_operand_pair *params = null_ptr, *results = null_ptr;
    scp_iterator3 *it0, *it, *it1;
    scp_uint32 rrel_number = 0, size2 = 0, size3 = 0, i = 0;
    scp_bool is4 = SCP_FALSE, isresult = SCP_FALSE;
    scp_result res;
    sc_result fun_result = SC_RESULT_OK;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand);
    MAKE_DEFAULT_NODE_ASSIGN(modifier);

    it0 = scp_iterator3_new(context, operator_node, &arc1, &operand);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it0, operator_node, &arc1, &operand))
    {
        arc1.param_type = SCP_FIXED;
        rrel_number = 0;
        it = scp_iterator3_new(context, &modifier, &arc2, &arc1);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &modifier, &arc2, &arc1))
        {
            if (rrel_number == 0)
            {
                rrel_number = check_ordinal_rrel(context, &modifier, 4);
                if (rrel_number > 0)
                {
                    continue;
                }
            }

            // Operand type
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_scp_const))
            {
                operand.operand_type = SCP_CONST;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_scp_var))
            {
                operand.operand_type = SCP_VAR;
                continue;
            }

            // Param type
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_assign))
            {
                operand.param_type = SCP_ASSIGN;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_fixed))
            {
                operand.param_type = SCP_FIXED;
                continue;
            }
        }
        scp_iterator3_free(it);

        if (rrel_number == 1)
        {
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param1))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }
        if (rrel_number == 2)
        {
            param2_node = operand;
            if (SCP_RESULT_TRUE != get_operand_value(context, &param2_node, &param2))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            if (param2.param_type == SCP_ASSIGN)
            {
                arc1.param_type = SCP_ASSIGN;
                MAKE_DEFAULT_OPERAND_ASSIGN(operand);
                continue;
            }
            isresult = SCP_TRUE;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand_elem);
            get_set_power(context, &param2, &size2);
            if (size2 == 0)
            {
                arc1.param_type = SCP_ASSIGN;
                MAKE_DEFAULT_OPERAND_ASSIGN(operand);
                continue;
            }
            results = (scp_operand_pair *)calloc(sizeof(scp_operand_pair), size2);
            pair_elems2 = (scp_operand **)calloc(sizeof(scp_operand), size2);
            pair_values2 = (scp_operand **)calloc(sizeof(scp_operand), size2);
            for (i = 0; i < size2; i++)
            {
                pair_elems2[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
                pair_values2[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
            }
            i = 0;
            it1 = scp_iterator3_new(context, &param2, &arc2, &operand_elem);
            while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &param2, &arc2, &operand_elem))
            {
                operand_elem.param_type = SCP_FIXED;
                resolve_operands_modifiers(context, &operand_elem, pair_elems2[i], 2);
                if (SCP_RESULT_TRUE != get_operands_values(context, pair_elems2[i], pair_values2[i], 2))
                {
                    operator_interpreting_crash(context, operator_node);
                    fun_result = SC_RESULT_ERROR;
                    goto exit;
                }
                results[i].operand1 = pair_values2[i];
                results[i].operand2 = pair_values2[i] + 1;
                operand_elem.param_type = SCP_ASSIGN;
                i++;
            }
            scp_iterator3_free(it1);
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }
        if (rrel_number == 3)
        {
            MAKE_DEFAULT_OPERAND_ASSIGN(operand_elem);
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param3))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            get_set_power(context, &param3, &size3);
            if (size3 == 0)
            {
                arc1.param_type = SCP_ASSIGN;
                MAKE_DEFAULT_OPERAND_ASSIGN(operand);
                continue;
            }
            params = (scp_operand_pair *)calloc(sizeof(scp_operand_pair), size3);
            pair_elems3 = (scp_operand **)calloc(sizeof(scp_operand), size3);
            pair_values3 = (scp_operand **)calloc(sizeof(scp_operand), size3);
            for (i = 0; i < size3; i++)
            {
                pair_elems3[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
                pair_values3[i] = (scp_operand *)calloc(sizeof(scp_operand), 2);
            }
            i = 0;
            it1 = scp_iterator3_new(context, &param3, &arc2, &operand_elem);
            while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &param3, &arc2, &operand_elem))
            {
                operand_elem.param_type = SCP_FIXED;
                resolve_operands_modifiers(context, &operand_elem, pair_elems3[i], 2);
                if (SCP_RESULT_TRUE != get_operands_values(context, pair_elems3[i], pair_values3[i], 2))
                {
                    operator_interpreting_crash(context, operator_node);
                    fun_result = SC_RESULT_ERROR;
                    goto exit;
                }
                params[i].operand1 = pair_values3[i];
                params[i].operand2 = pair_values3[i] + 1;
                operand_elem.param_type = SCP_ASSIGN;
                i++;
            }
            scp_iterator3_free(it1);
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }
        if (rrel_number == 4)
        {
            is4 = SCP_TRUE;
            param4_node = operand;
            if (SCP_RESULT_TRUE != get_operand_value(context, &operand, &param4))
            {
                operator_interpreting_crash(context, operator_node);
                fun_result = SC_RESULT_ERROR;
                goto exit;
            }
            arc1.param_type = SCP_ASSIGN;
            MAKE_DEFAULT_OPERAND_ASSIGN(operand);
            continue;
        }

        arc1.param_type = SCP_ASSIGN;
        MAKE_DEFAULT_OPERAND_ASSIGN(operand);
    }
    scp_iterator3_free(it0);
    if (isresult == SCP_TRUE)
    {
        if (is4 == SCP_TRUE)
        {
            res = scp_sys_gen_for_variables(context, &param1, results, size2, params, size3, &param4);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    for (i = 0; i < size2; i++)
                    {
                        set_operands_values(context, pair_elems2[i] + 1, pair_values2[i] + 1, 2);
                    }
                    set_operands_values(context, &param4_node, &param4, 1);
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
        else
        {
            res = scp_sys_gen_for_variables(context, &param1, results, size2, params, size3, null_ptr);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    for (i = 0; i < size2; i++)
                    {
                        set_operands_values(context, pair_elems2[i] + 1, pair_values2[i] + 1, 2);
                    }
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
    }
    else
    {
        if (is4 == SCP_TRUE)
        {
            res = scp_sys_gen(context, &param1, &param2, params, size3, &param4);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    set_operands_values(context, &param2_node, &param2, 1);
                    set_operands_values(context, &param4_node, &param4, 1);
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
        else
        {
            res = scp_sys_gen(context, &param1, &param2, params, size3, null_ptr);
            switch (res)
            {
                case SCP_RESULT_TRUE:
                {
                    set_operands_values(context, &param2_node, &param2, 1);
                    if (SCP_RESULT_TRUE != goto_conditional_success(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_FALSE:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_unsuccess(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
                case SCP_RESULT_ERROR:
                {
                    if (SCP_RESULT_TRUE != goto_conditional_error(context, operator_node))
                    {
                        fun_result = SC_RESULT_ERROR;
                        goto exit;
                    }
                    fun_result = SC_RESULT_OK;
                    goto exit;
                }
            }
        }
    }
exit:
    //Memory deallocating
    for (i = 0; i < size2; i++)
    {
        free(pair_elems2[i]);
        free(pair_values2[i]);
    }
    for (i = 0; i < size3; i++)
    {
        free(pair_elems3[i]);
        free(pair_values3[i]);
    }
    free(pair_elems2);
    free(pair_values2);
    free(pair_elems3);
    free(pair_values3);
    free(results);
    free(params);
    return fun_result;
}

sc_result interpreter_agent_system_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        return SC_RESULT_ERROR;
    }

    //sys_search case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_sys_search))
    {
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("sys_search");
        return sys_search_agent_body(s_default_ctx, &operator_node);
    }
    //sys_gen case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_sys_gen))
    {
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("sys_gen");
        return sys_gen_agent_body(s_default_ctx, &operator_node);
    }

    return SC_RESULT_ERROR;
}

sc_result interpreter_agent_syncronize_operators(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, operator_node, operator_type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &operator_node))
    {
        //print_error("scp-operator interpreting", "Can't find operator node");
        return SC_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);
    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
    {
        printEl(s_default_ctx, &operator_node);
        print_error("scp-operator interpreting", "Can't resolve operator type");
        return SC_RESULT_ERROR;
    }

    //syncronize case
    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_syncronize))
    {
        input_arc.erase = SCP_TRUE;
        eraseEl(s_default_ctx, &input_arc);
        print_debug_info("syncronize");
        goto_unconditional(s_default_ctx, &operator_node);
    }

    return SC_RESULT_ERROR;
}


sc_result interpreting_question_finished_successfully(const sc_event *event, sc_addr arg)
{
    scp_operand input_arc, node1, quest;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_NODE_ASSIGN(quest);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &input_arc, &quest))
    {
        return SC_RESULT_ERROR;
    }
    quest.param_type = SCP_FIXED;
    MAKE_DEFAULT_ARC_ASSIGN(input_arc);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &quest))
    {
        return SC_RESULT_OK;
    }
    if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &question_scp_interpretation_request, &input_arc, &quest))
    {
        scp_operand arc1, arc2, act_arc, arc3, descr_node, descr_set, operator_node, operator_type, params, authors;
        scp_iterator3 *it1;
        scp_iterator5 *it2, *it3;
        MAKE_DEFAULT_ARC_ASSIGN(arc1);
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_DEFAULT_ARC_ASSIGN(act_arc);
        MAKE_COMMON_ARC_ASSIGN(arc3);
        MAKE_DEFAULT_OPERAND_ASSIGN(descr_node);
        MAKE_DEFAULT_OPERAND_ASSIGN(operator_type);

        //waitReturn case
        it2 = scp_iterator5_new(s_default_ctx, &descr_node, &arc3, &quest, &arc2, &nrel_value);
        while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it2, &descr_node, &arc3, &quest, &arc2, &nrel_value))
        {
            descr_node.param_type = SCP_FIXED;
            MAKE_DEFAULT_OPERAND_ASSIGN(operator_node);
            it3 = scp_iterator5_new(s_default_ctx, &operator_node, &arc1, &descr_node, &arc2, &ordinal_rrels[1]);
            while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it3, &operator_node, &arc1, &descr_node, &arc2, &ordinal_rrels[1]))
            {
                operator_node.param_type = SCP_FIXED;
                if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
                {
                    scp_iterator5_free(it2);
                    scp_iterator5_free(it3);
                    return SC_RESULT_ERROR;
                }
                if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_waitReturn))
                {
                    if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &active_scp_operator, &act_arc, &operator_node) &&
                        SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &executed_scp_operator, &arc1, &operator_node))
                    {
                        MAKE_COMMON_ARC_ASSIGN(arc1);
                        MAKE_DEFAULT_OPERAND_ASSIGN(authors);
                        authors.erase = SCP_TRUE;
                        quest.erase = SCP_FALSE;
                        eraseElStr5(s_default_ctx, &authors, &arc1, &quest, &arc2, &nrel_authors);
                        MAKE_DEFAULT_ARC_ASSIGN(arc1);
                        MAKE_DEFAULT_OPERAND_ASSIGN(params);
                        params.erase = SCP_TRUE;
                        quest.erase = SCP_TRUE;
                        eraseElStr5(s_default_ctx, &quest, &arc1, &params, &arc2, &ordinal_rrels[2]);
                        act_arc.param_type = SCP_FIXED;
                        act_arc.erase = SCP_TRUE;
                        eraseEl(s_default_ctx, &act_arc);
                        goto_unconditional(s_default_ctx, &operator_node);
                        scp_iterator5_free(it2);
                        scp_iterator5_free(it3);
                        return SC_RESULT_OK;
                    }
                }
                operator_node.param_type = SCP_ASSIGN;
            }
            scp_iterator5_free(it3);
            descr_node.param_type = SCP_ASSIGN;
        }
        scp_iterator5_free(it2);

        //waitReturnSet case
        MAKE_DEFAULT_OPERAND_ASSIGN(descr_set);
        it1 = scp_iterator3_new(s_default_ctx, &descr_set, &arc1, &quest);
        while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it1, &descr_set, &arc1, &quest))
        {
            descr_set.param_type = SCP_FIXED;
            descr_node.param_type = SCP_ASSIGN;
            it2 = scp_iterator5_new(s_default_ctx, &descr_node, &arc3, &descr_set, &arc2, &nrel_value);
            while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it2, &descr_node, &arc3, &descr_set, &arc2, &nrel_value))
            {
                descr_node.param_type = SCP_FIXED;
                operator_node.param_type = SCP_ASSIGN;
                it3 = scp_iterator5_new(s_default_ctx, &operator_node, &arc1, &descr_node, &arc2, &ordinal_rrels[1]);
                while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it3, &operator_node, &arc1, &descr_node, &arc2, &ordinal_rrels[1]))
                {
                    operator_node.param_type = SCP_FIXED;
                    operator_type.param_type = SCP_ASSIGN;
                    if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &operator_node, &operator_type))
                    {
                        scp_iterator5_free(it2);
                        scp_iterator5_free(it3);
                        return SC_RESULT_ERROR;
                    }
                    operator_type.param_type = SCP_FIXED;
                    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_waitReturnSet))
                    {
                        if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &active_scp_operator, &act_arc, &operator_node) &&
                            SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &executed_scp_operator, &arc1, &operator_node))
                        {
                            MAKE_COMMON_ARC_ASSIGN(arc1);
                            MAKE_DEFAULT_OPERAND_ASSIGN(authors);
                            authors.erase = SCP_TRUE;
                            quest.erase = SCP_FALSE;
                            eraseElStr5(s_default_ctx, &authors, &arc1, &quest, &arc2, &nrel_authors);
                            MAKE_DEFAULT_ARC_ASSIGN(arc1);
                            MAKE_DEFAULT_OPERAND_ASSIGN(params);
                            params.erase = SCP_TRUE;
                            quest.erase = SCP_TRUE;
                            eraseElStr5(s_default_ctx, &quest, &arc1, &params, &arc2, &ordinal_rrels[2]);
                            if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &descr_set, &arc1, &params))
                            {
                                act_arc.param_type = SCP_FIXED;
                                act_arc.erase = SCP_TRUE;
                                eraseEl(s_default_ctx, &act_arc);
                                goto_unconditional(s_default_ctx, &operator_node);
                                scp_iterator3_free(it1);
                                scp_iterator5_free(it2);
                                scp_iterator5_free(it3);
                                return SC_RESULT_OK;
                            }
                            else
                            {
                                scp_iterator3_free(it1);
                                scp_iterator5_free(it2);
                                scp_iterator5_free(it3);
                                return SC_RESULT_OK;
                            }
                        }
                    }
                    operator_node.param_type = SCP_ASSIGN;
                }
                descr_node.param_type = SCP_ASSIGN;
            }
            scp_iterator5_free(it2);
            descr_set.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it1);

        //printf("INTERPRETATION FINISHED SUCCESSFULLY\n");
    }
    return SC_RESULT_OK;
}

void destroy_sys_event_table_item(gpointer data)
{
    sc_event *event = (sc_event *)data;
    sc_event_destroy(event);
}

void destroy_sys_wait_event_table_item(gpointer data)
{
    sc_event_destroy((sc_event *)data);
}

sc_result sc_agent_activator(sc_event *in_event, sc_addr arg)
{
    scp_operand scp_sc_agent, arc, event_type, event_elem, agent_program, node1, arc3, input_arc;
    sc_event *event;
    sc_event_type type;

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    input_arc.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_OPERAND_ASSIGN(node1);
    MAKE_DEFAULT_OPERAND_ASSIGN(scp_sc_agent);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &input_arc))
    {
        return SC_RESULT_OK;
    }

    searchElStr3(s_default_ctx, &node1, &input_arc, &scp_sc_agent);
    scp_sc_agent.param_type = SCP_FIXED;

    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_OPERAND_ASSIGN(agent_program);
    MAKE_DEFAULT_OPERAND_ASSIGN(event_elem);
    MAKE_DEFAULT_OPERAND_ASSIGN(event_type);
    MAKE_DEFAULT_OPERAND_ASSIGN(node1);

    if (SCP_TRUE == register_scp_based_sc_agent(s_default_ctx, &scp_sc_agent, &agent_program, &event_type, &event_elem))
    {
        if (SCP_TRUE != resolve_sc_agent_event_type(s_default_ctx, &event_type, &type))
            return SC_RESULT_OK;
        event = sc_event_new(s_default_ctx, event_elem.addr, type, (sc_pointer)SC_ADDR_LOCAL_TO_INT(agent_program.addr), (fEventCallback)scp_event_procedure_processor, NULL);
        g_hash_table_insert(scp_event_table, MAKE_HASH(agent_program), (gpointer)event);

        printf("REGISTERING SCP AGENT PROGRAM: ");
        MAKE_DEFAULT_OPERAND_ASSIGN(node1);
        if (SCP_TRUE == searchElStr5(s_default_ctx, &agent_program, &arc3, &node1, &arc, &nrel_system_identifier))
        {
            printNl(s_default_ctx, &node1);
        }
    }
    scp_sc_agent.param_type = SCP_ASSIGN;
    return SC_RESULT_OK;
}

sc_result sc_agent_deactivator(sc_event *in_event, sc_addr arg)
{
    scp_operand scp_sc_agent, node1, input_arc, arc, arc3;

    //!TODO Solve deactivation problem

    MAKE_DEFAULT_OPERAND_FIXED(input_arc);
    input_arc.addr = arg;
    MAKE_DEFAULT_OPERAND_ASSIGN(node1);
    MAKE_DEFAULT_OPERAND_ASSIGN(scp_sc_agent);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &input_arc))
    {
        return SC_RESULT_ERROR;
    }
    searchElStr3(s_default_ctx, &node1, &input_arc, &scp_sc_agent);
    scp_sc_agent.param_type = SCP_FIXED;

    g_hash_table_remove(scp_event_table, MAKE_HASH(scp_sc_agent));

    printf("UNREGISTERING SCP AGENT PROGRAM: ");
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_OPERAND_ASSIGN(node1);
    if (SCP_TRUE == searchElStr5(s_default_ctx, &scp_sc_agent, &arc3, &node1, &arc, &nrel_system_identifier))
    {
        printNl(s_default_ctx, &node1);
    }

    return SC_RESULT_OK;
}

scp_result init_all_scp_agents(sc_memory_context *context)
{
    scp_operand scp_sc_agent, arc, event_type, event_elem, agent_program, node1, arc3;
    scp_iterator3 *it;
    sc_event *event;
    sc_event_type type;

    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_OPERAND_ASSIGN(scp_sc_agent);
    MAKE_DEFAULT_OPERAND_ASSIGN(agent_program);
    MAKE_DEFAULT_OPERAND_ASSIGN(event_elem);
    MAKE_DEFAULT_OPERAND_ASSIGN(event_type);
    MAKE_DEFAULT_OPERAND_ASSIGN(node1);
    it = scp_iterator3_new(context, &active_sc_agent, &arc, &scp_sc_agent);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &active_sc_agent, &arc, &scp_sc_agent))
    {
        scp_sc_agent.param_type = SCP_FIXED;
        if (SCP_TRUE == register_scp_based_sc_agent(context, &scp_sc_agent, &agent_program, &event_type, &event_elem))
        {
            if (SCP_TRUE != resolve_sc_agent_event_type(context, &event_type, &type))
                continue;
            event = sc_event_new(s_default_ctx, event_elem.addr, type, SC_ADDR_LOCAL_TO_INT(agent_program.addr), (fEventCallback)scp_event_procedure_processor, NULL);
            g_hash_table_insert(scp_event_table, MAKE_HASH(agent_program), (gpointer)event);

            printf("REGISTERING SCP AGENT PROGRAM: ");
            MAKE_DEFAULT_OPERAND_ASSIGN(node1);
            if (SCP_TRUE == searchElStr5(context, &agent_program, &arc3, &node1, &arc, &nrel_system_identifier))
            {
                printNl(context, &node1);
            }
        }
        scp_sc_agent.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SC_RESULT_OK;
}

scp_result scp_operator_interpreter_agents_init()
{
    event_interpreting_finished_succesfully = sc_event_new(s_default_ctx, question_finished_successfully.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreting_question_finished_successfully, 0);
    if (event_interpreting_finished_succesfully == null_ptr)
        return SCP_RESULT_ERROR;

    event_search_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_search_operators, 0);
    if (event_search_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_gen_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_gen_operators, 0);
    if (event_gen_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_erase_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_erase_operators, 0);
    if (event_erase_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;

#ifdef SCP_MATH
    event_content_arithmetic_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_content_arithmetic_operators, 0);
    if (event_content_arithmetic_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_content_trig_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_content_trig_operators, 0);
    if (event_content_trig_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
#endif

#ifdef SCP_STRING
    event_content_string_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_content_string_operators, 0);
    if (event_content_string_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
#endif

    event_if_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_if_operators, 0);
    if (event_if_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_other_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_other_operators, 0);
    if (event_other_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_system_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_system_operators, 0);
    if (event_system_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_event_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_event_operators, 0);
    if (event_event_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_print_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_print_operators, 0);
    if (event_print_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_return_operator_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_return_operator, 0);
    if (event_return_operator_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_call_operator_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_call_operator, 0);
    if (event_call_operator_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_waitReturn_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_waitReturn_operators, 0);
    if (event_waitReturn_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_syncronize_operators_interpreter = sc_event_new(s_default_ctx, active_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)interpreter_agent_syncronize_operators, 0);
    if (event_syncronize_operators_interpreter == null_ptr)
        return SCP_RESULT_ERROR;
    event_register_sc_agent = sc_event_new(s_default_ctx, active_sc_agent.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)sc_agent_activator, 0);
    if (event_register_sc_agent == null_ptr)
        return SCP_RESULT_ERROR;
    event_unregister_sc_agent = sc_event_new(s_default_ctx, active_sc_agent.addr, SC_EVENT_REMOVE_OUTPUT_ARC, 0, (fEventCallback)sc_agent_deactivator, 0);
    if (event_unregister_sc_agent == null_ptr)
        return SCP_RESULT_ERROR;

    scp_event_table = g_hash_table_new_full(NULL, NULL, NULL, destroy_sys_event_table_item);
    scp_wait_event_table = g_hash_table_new_full(NULL, NULL, NULL, destroy_sys_wait_event_table_item);

    init_all_scp_agents(s_default_ctx);

    return SCP_RESULT_TRUE;
}

scp_result scp_operator_interpreter_agents_shutdown()
{
    sc_event_destroy(event_interpreting_finished_succesfully);

    sc_event_destroy(event_search_operators_interpreter);
    sc_event_destroy(event_gen_operators_interpreter);
    sc_event_destroy(event_erase_operators_interpreter);
    sc_event_destroy(event_return_operator_interpreter);
    sc_event_destroy(event_print_operators_interpreter);

#ifdef SCP_MATH
    sc_event_destroy(event_content_arithmetic_operators_interpreter);
    sc_event_destroy(event_content_trig_operators_interpreter);
#endif

#ifdef SCP_STRING
    sc_event_destroy(event_content_string_operators_interpreter);
#endif

    sc_event_destroy(event_if_operators_interpreter);
    sc_event_destroy(event_other_operators_interpreter);
    sc_event_destroy(event_system_operators_interpreter);
    sc_event_destroy(event_event_operators_interpreter);
    sc_event_destroy(event_call_operator_interpreter);
    sc_event_destroy(event_waitReturn_operators_interpreter);
    sc_event_destroy(event_syncronize_operators_interpreter);
    sc_event_destroy(event_register_sc_agent);
    sc_event_destroy(event_unregister_sc_agent);

    g_hash_table_destroy(scp_event_table);
    g_hash_table_destroy(scp_wait_event_table);

    return SCP_RESULT_TRUE;
}
