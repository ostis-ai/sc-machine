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
#include "scp_keynodes.h"
#include "scp_operator_syncronizer.h"
#include "scp_interpreter_utils.h"
#include <glib.h>

#include <stdio.h>

sc_event *event_operator_syncronizer_goto;
sc_event *event_operator_syncronizer_then;
sc_event *event_operator_syncronizer_else;

scp_result start_next_operator(sc_memory_context *context, scp_operand *scp_operator, scp_operand *curr_arc);

scp_result erase_executed_mark(sc_memory_context *context, scp_operand *scp_operator)
{
    scp_operand arc1, arc2, execute_result;
    scp_iterator3 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(execute_result);
    it = scp_iterator3_new(context, &execute_result, &arc1, scp_operator);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &execute_result, &arc1, scp_operator))
    {
        execute_result.param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == ifCoin(context, &execute_result, &executed_scp_operator)
            || SCP_RESULT_TRUE == ifCoin(context, &execute_result, &successfully_executed_scp_operator)
            || SCP_RESULT_TRUE == ifCoin(context, &execute_result, &unsuccessfully_executed_scp_operator))
        {
            arc1.param_type = SCP_FIXED;
            arc1.erase = SCP_TRUE;
            eraseEl(context, &arc1);
            scp_iterator3_free(it);
            return SCP_RESULT_TRUE;
        }
        execute_result.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

sc_result syncronize_scp_operator(const sc_event *event, sc_addr arg)
{
    scp_operand arc1, arc2, arc3, execute_result, operator_node, next_operator_node;
    scp_iterator5 *it;
    MAKE_DEFAULT_OPERAND_FIXED(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);

    arc1.addr = arg;
    arc1.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(execute_result);
    MAKE_DEFAULT_NODE_ASSIGN(operator_node);
    MAKE_DEFAULT_NODE_ASSIGN(next_operator_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &arc1))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &arc1))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &execute_result, &arc1, &operator_node))
    {
        return SCP_RESULT_ERROR;
    }
    operator_node.param_type = SCP_FIXED;
    execute_result.param_type = SCP_FIXED;
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &operator_node))
    {
        return SC_RESULT_ERROR;
    }

    if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &execute_result, &successfully_executed_scp_operator))
    {
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_COMMON_ARC_ASSIGN(arc3);
        it = scp_iterator5_new(s_default_ctx, &operator_node, &arc3, &next_operator_node, &arc2, &nrel_then);
        while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it, &operator_node, &arc3, &next_operator_node, &arc2, &nrel_then))
        {
            arc3.param_type = SCP_FIXED;
            next_operator_node.param_type = SCP_FIXED;
            if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &active_scp_operator, &arc2, &next_operator_node))
            {
                start_next_operator(s_default_ctx, &next_operator_node, &arc3);
            }
            arc3.param_type = SCP_ASSIGN;
            next_operator_node.param_type = SCP_ASSIGN;
        }
        scp_iterator5_free(it);
    }
    else if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &execute_result, &unsuccessfully_executed_scp_operator))
    {
        MAKE_DEFAULT_ARC_ASSIGN(arc2);
        MAKE_COMMON_ARC_ASSIGN(arc3);
        it = scp_iterator5_new(s_default_ctx, &operator_node, &arc3, &next_operator_node, &arc2, &nrel_else);
        while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it, &operator_node, &arc3, &next_operator_node, &arc2, &nrel_else))
        {
            arc3.param_type = SCP_FIXED;
            next_operator_node.param_type = SCP_FIXED;
            if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &active_scp_operator, &arc2, &next_operator_node))
            {
                start_next_operator(s_default_ctx, &next_operator_node, &arc3);
            }
            arc3.param_type = SCP_ASSIGN;
            next_operator_node.param_type = SCP_ASSIGN;
        }
        scp_iterator5_free(it);
    }

    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    it = scp_iterator5_new(s_default_ctx, &operator_node, &arc3, &next_operator_node, &arc2, &nrel_goto);
    while (SCP_RESULT_TRUE == scp_iterator5_next(s_default_ctx, it, &operator_node, &arc3, &next_operator_node, &arc2, &nrel_goto))
    {
        arc3.param_type = SCP_FIXED;
        next_operator_node.param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &active_scp_operator, &arc2, &next_operator_node))
        {
            start_next_operator(s_default_ctx, &next_operator_node, &arc3);
        }
        arc3.param_type = SCP_ASSIGN;
        next_operator_node.param_type = SCP_ASSIGN;
    }
    scp_iterator5_free(it);

	return SC_RESULT_OK;
}

scp_result start_next_operator(sc_memory_context *context, scp_operand *scp_operator, scp_operand *curr_arc)
{
    scp_operand arc1, arc2, arc3, prev_operator, execute_result, relation;
    scp_iterator3 *it, *it1;
    sc_char exec_type = -1; //0 - executed, 1 - succesfully, 2 - unsuccesfully
    sc_char rel_type = -1; //0 - goto, 1 - then, 2 - else
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_NODE_ASSIGN(prev_operator);
    if (SCP_RESULT_TRUE != searchElStr3(context, &scp_operator_executable_after_all_previous, &arc1, scp_operator))
    {
        erase_executed_mark(context, scp_operator);
        genElStr3(context, &active_scp_operator, &arc1, scp_operator);
        return SCP_RESULT_TRUE;
    }
    it = scp_iterator3_new(context, &prev_operator, &arc3, scp_operator);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &prev_operator, &arc3, scp_operator))
    {
        arc3.param_type = SCP_FIXED;

        if (SCP_RESULT_TRUE == ifCoin(context, curr_arc, &arc3))
        {
            arc3.param_type = SCP_ASSIGN;
            continue;
        }

        rel_type = -1;
        MAKE_DEFAULT_NODE_ASSIGN(relation);
        it1 = scp_iterator3_new(context, &relation, &arc1, &arc3);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &relation, &arc1, &arc3))
        {
            relation.param_type = SCP_TRUE;
            if (SCP_RESULT_TRUE == ifCoin(context, &relation, &nrel_goto))
            {
                rel_type = 0;
                break;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &relation, &nrel_then))
            {
                rel_type = 1;
                break;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &relation, &nrel_else))
            {
                rel_type = 2;
                break;
            }
            relation.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it1);
        if (rel_type == -1)
        {
            arc3.param_type = SCP_ASSIGN;
            continue;
        }

        prev_operator.param_type = SCP_FIXED;

        exec_type = -1;
        MAKE_DEFAULT_NODE_ASSIGN(execute_result);
        it1 = scp_iterator3_new(context, &execute_result, &arc1, &prev_operator);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &execute_result, &arc1, &prev_operator))
        {
            execute_result.param_type = SCP_FIXED;
            if (SCP_RESULT_TRUE == ifCoin(context, &execute_result, &executed_scp_operator))
            {
                exec_type = 0;
                break;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &execute_result, &successfully_executed_scp_operator))
            {
                exec_type = 1;
                break;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &execute_result, &unsuccessfully_executed_scp_operator))
            {
                exec_type = 2;
                break;
            }
            execute_result.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it1);

        if (exec_type == -1)
        {
            scp_iterator3_free(it);
            return SCP_RESULT_FALSE;
        }

        if (rel_type == 0 || (rel_type == 1 && exec_type == 1) || (rel_type == 2 && exec_type == 2))
        {
            arc3.param_type = SCP_ASSIGN;
            prev_operator.param_type = SCP_ASSIGN;
            continue;
        }
        else
        {
            scp_iterator3_free(it);
            return SCP_RESULT_FALSE;
        }

        arc3.param_type = SCP_ASSIGN;
        prev_operator.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);

    erase_executed_mark(context, scp_operator);
    genElStr3(context, &active_scp_operator, &arc1, scp_operator);
    return SCP_RESULT_TRUE;
}

scp_result scp_operator_syncronizer_init()
{
    event_operator_syncronizer_goto = sc_event_new(s_default_ctx, executed_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)syncronize_scp_operator, 0);
    if (event_operator_syncronizer_goto == null_ptr)
        return SCP_RESULT_ERROR;
    event_operator_syncronizer_then  = sc_event_new(s_default_ctx, successfully_executed_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)syncronize_scp_operator, 0);
    if (event_operator_syncronizer_then == null_ptr)
        return SCP_RESULT_ERROR;
    event_operator_syncronizer_else = sc_event_new(s_default_ctx, unsuccessfully_executed_scp_operator.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)syncronize_scp_operator, 0);
    if (event_operator_syncronizer_else == null_ptr)
        return SCP_RESULT_ERROR;
    return SCP_RESULT_TRUE;
}

scp_result scp_operator_syncronizer_shutdown()
{
    sc_event_destroy(event_operator_syncronizer_goto);
    sc_event_destroy(event_operator_syncronizer_then);
    sc_event_destroy(event_operator_syncronizer_else);
    return SCP_RESULT_TRUE;
}
