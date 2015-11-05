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
#include "scp_process_destroyer.h"
#include "scp_interpreter_utils.h"
#include "scp_keynodes.h"

#include <stdio.h>

sc_event *event_process_destroy;

void delete_vars_from_set(sc_memory_context *context, scp_operand *set, GHashTable *non_erasable_vars)
{
    scp_operand arc1, arc2, elem;
    scp_iterator5 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(elem);
    elem.erase = SCP_TRUE;
    it = scp_iterator5_new(context, set, &arc1, &elem, &arc2, &rrel_scp_var);
    while (SCP_RESULT_TRUE == scp_iterator5_next(context, it, set, &arc1, &elem, &arc2, &rrel_scp_var))
    {
        elem.param_type = SCP_FIXED;
        if (non_erasable_vars != null_ptr && FALSE == g_hash_table_contains(non_erasable_vars, MAKE_HASH(elem)))
        {
            eraseEl(context, &elem);
        }
        elem.param_type = SCP_ASSIGN;
    }
    scp_iterator5_free(it);
}

void delete_vars_from_relation(sc_memory_context *context, scp_operand *set, GHashTable *non_erasable_vars)
{
    scp_operand arc1, arc2, elem, rel_elem;
    scp_iterator5 *it;
    scp_iterator3 *it0;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(elem);
    MAKE_DEFAULT_OPERAND_ASSIGN(rel_elem);
    elem.erase = SCP_TRUE;
    rel_elem.erase = SCP_TRUE;

    it0 = scp_iterator3_new(context, set, &arc1, &rel_elem);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it0, set, &arc1, &rel_elem))
    {
        rel_elem.param_type = SCP_FIXED;
        it = scp_iterator5_new(context, &rel_elem, &arc1, &elem, &arc2, &rrel_scp_var);
        while (SCP_RESULT_TRUE == scp_iterator5_next(context, it, &rel_elem, &arc1, &elem, &arc2, &rrel_scp_var))
        {
            elem.param_type = SCP_FIXED;
            if (non_erasable_vars != null_ptr && FALSE == g_hash_table_contains(non_erasable_vars, MAKE_HASH(elem)))
            {
                eraseEl(context, &elem);
            }
            elem.param_type = SCP_ASSIGN;
        }
        scp_iterator5_free(it);
        eraseEl(context, &rel_elem);
        rel_elem.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it0);
}

sc_result destroy_scp_process(const sc_event *event, sc_addr arg)
{
    scp_operand arc1, arc2, node1, curr_operator, node3, scp_process_node, operator_type, question_node, call_parameters;
    scp_iterator3 *it;
    GHashTable *params = null_ptr;
    scp_result params_found;
    MAKE_DEFAULT_OPERAND_FIXED(arc1);
    MAKE_DEFAULT_NODE_ASSIGN(scp_process_node);
    arc1.addr = arg;
    arc1.element_type = scp_type_arc_pos_const_perm;
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &arc1))
    {
        return SC_RESULT_OK;
    }

    MAKE_DEFAULT_OPERAND_ASSIGN(node1);
    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &arc1, &scp_process_node))
    {
        return SC_RESULT_ERROR;
    }

    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    scp_process_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(curr_operator);
    MAKE_DEFAULT_OPERAND_ASSIGN(question_node);
    MAKE_DEFAULT_OPERAND_ASSIGN(call_parameters);
    MAKE_DEFAULT_OPERAND_ASSIGN(node3);
    node1.erase = SCP_TRUE;
    curr_operator.erase = SCP_TRUE;
    node3.erase = SCP_TRUE;

    MAKE_DEFAULT_OPERAND_ASSIGN(question_node);
    MAKE_COMMON_ARC_ASSIGN(arc1);
    params_found = searchElStr5(s_default_ctx, &question_node, &arc1, &scp_process_node, &arc2, &nrel_scp_process);
    question_node.param_type = SCP_FIXED;

    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    if (params_found == SCP_RESULT_TRUE)
    {
        MAKE_DEFAULT_OPERAND_ASSIGN(call_parameters);
        params_found = searchElStr5(s_default_ctx, &question_node, &arc1, &call_parameters, &arc2, &ordinal_rrels[2]);
        call_parameters.param_type = SCP_FIXED;
        if (params_found == SCP_RESULT_TRUE)
        {
            params = g_hash_table_new(NULL, NULL);
            load_set_to_hash(s_default_ctx, &call_parameters, params);
        }
    }

    MAKE_DEFAULT_NODE_ASSIGN(operator_type);

    it = scp_iterator3_new(s_default_ctx, &scp_process_node, &arc1, &curr_operator);
    while (SCP_RESULT_TRUE == scp_iterator3_next(s_default_ctx, it, &scp_process_node, &arc1, &curr_operator))
    {
        curr_operator.param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE != resolve_operator_type(s_default_ctx, &curr_operator, &operator_type))
        {
            curr_operator.param_type = SCP_ASSIGN;
            delete_vars_from_set(s_default_ctx, &curr_operator, params);
            eraseEl(s_default_ctx, &curr_operator);
            continue;
        }
        operator_type.param_type = SCP_FIXED;

        if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_call))
        {
            //printf("OPERATOR CALL\n");
            if (SCP_RESULT_TRUE == searchElStr5(s_default_ctx, &curr_operator, &arc1, &node3, &arc2, &(ordinal_rrels[2])))
            {
                node3.param_type = SCP_FIXED;
                delete_vars_from_set(s_default_ctx, &node3, params);
                eraseEl(s_default_ctx, &node3);
                node3.param_type = SCP_ASSIGN;
            }
            delete_vars_from_set(s_default_ctx, &curr_operator, params);
            eraseEl(s_default_ctx, &curr_operator);
            operator_type.param_type = SCP_ASSIGN;
            curr_operator.param_type = SCP_ASSIGN;
            continue;
        }

        if (SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_sys_gen) ||
            SCP_RESULT_TRUE == ifCoin(s_default_ctx, &operator_type, &op_sys_search))
        {
            if (SCP_RESULT_TRUE == searchElStr5(s_default_ctx, &curr_operator, &arc1, &node3, &arc2, &(ordinal_rrels[2])))
            {
                node3.param_type = SCP_FIXED;
                delete_vars_from_relation(s_default_ctx, &node3, params);
                eraseEl(s_default_ctx, &node3);
                node3.param_type = SCP_ASSIGN;
            }
            if (SCP_RESULT_TRUE == searchElStr5(s_default_ctx, &curr_operator, &arc1, &node3, &arc2, &(ordinal_rrels[3])))
            {
                node3.param_type = SCP_FIXED;
                delete_vars_from_relation(s_default_ctx, &node3, params);
                eraseEl(s_default_ctx, &node3);
                node3.param_type = SCP_ASSIGN;
            }
            delete_vars_from_set(s_default_ctx, &curr_operator, params);
            eraseEl(s_default_ctx, &curr_operator);
            operator_type.param_type = SCP_ASSIGN;
            curr_operator.param_type = SCP_ASSIGN;
            continue;
        }
        delete_vars_from_set(s_default_ctx, &curr_operator, params);
        eraseEl(s_default_ctx, &curr_operator);
        operator_type.param_type = SCP_ASSIGN;
        curr_operator.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    scp_process_node.erase = SCP_TRUE;
    eraseEl(s_default_ctx, &scp_process_node);

    if (SCP_RESULT_TRUE == params_found)
    {
        g_hash_table_destroy(params);
    }

    //printf("PROCESS DESTROYED SUCCESSFULLY\n");
    return SC_RESULT_OK;
}

scp_result scp_process_destroyer_init()
{
    event_process_destroy = sc_event_new(s_default_ctx, useless_scp_process.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, destroy_scp_process, 0);
    if (event_process_destroy == null_ptr)
        return SCP_RESULT_ERROR;
    return SCP_RESULT_TRUE;
}

scp_result scp_process_destroyer_shutdown()
{
    sc_event_destroy(event_process_destroy);
    return SCP_RESULT_TRUE;
}

#include <stdio.h>
