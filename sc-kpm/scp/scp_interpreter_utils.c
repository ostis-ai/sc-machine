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

#include "scp_interpreter_utils.h"
#include "scp_keynodes.h"
#include "stdio.h"


scp_result check_scp_interpreter_question(sc_memory_context *context, scp_operand *quest)
{
    scp_operand arc2, arc1, authors;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(authors);

    if (SCP_RESULT_TRUE != searchElStr5(context, &authors, &arc2, quest, &arc1, &nrel_authors))
    {
        return SCP_RESULT_FALSE;
    }
    authors.param_type = SCP_FIXED;
    if (SCP_RESULT_TRUE != searchElStr3(context, &authors, &arc1, &abstract_scp_machine))
    {
        return SCP_RESULT_FALSE;
    }

    return SCP_RESULT_TRUE;
}

scp_result resolve_operator_type(sc_memory_context *context, scp_operand *oper, scp_operand *type)
{
    scp_operand arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_DEFAULT_NODE_ASSIGN((*type));

    oper->param_type = SCP_FIXED;
    scp_iterator3 *it = scp_iterator3_new(context, type, &arc, oper);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, type, &arc, oper))
    {
        //! TODO Remove temporary check
        if (SCP_RESULT_TRUE != ifVarAssign(context, type))
        {
            continue;
        }
        type->param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == searchElStr3(context, &scp_operator_atomic_type, &arc, type))
        {
            scp_iterator3_free(it);
            return SCP_RESULT_TRUE;
        }
        type->param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

scp_result resolve_scp_event_type(sc_memory_context *context, scp_operand *element, sc_event_type *type)
{
    if (SCP_RESULT_TRUE == ifCoin(context, &sc_event_add_input_arc, element))
    {
        *type = SC_EVENT_ADD_INPUT_ARC;
        return SCP_RESULT_TRUE;
    }
    if (SCP_RESULT_TRUE == ifCoin(context, &sc_event_add_output_arc, element))
    {
        *type = SC_EVENT_ADD_OUTPUT_ARC;
        return SCP_RESULT_TRUE;
    }
    if (SCP_RESULT_TRUE == ifCoin(context, &sc_event_add_input_arc, element))
    {
        *type = SC_EVENT_ADD_INPUT_ARC;
        return SCP_RESULT_TRUE;
    }
    if (SCP_RESULT_TRUE == ifCoin(context, &sc_event_remove_input_arc, element))
    {
        *type = SC_EVENT_REMOVE_INPUT_ARC;
        return SCP_RESULT_TRUE;
    }
    if (SCP_RESULT_TRUE == ifCoin(context, &sc_event_remove_output_arc, element))
    {
        *type = SC_EVENT_REMOVE_OUTPUT_ARC;
        return SCP_RESULT_TRUE;
    }
    return SCP_RESULT_FALSE;
}

scp_result resolve_ordinal_rrel(sc_memory_context *context, scp_operand *arc_param, scp_operand *result)
{
    scp_operand arc;
    scp_iterator3 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_DEFAULT_NODE_ASSIGN((*result));
    arc_param->param_type = SCP_FIXED;
    it = scp_iterator3_new(context, result, &arc, arc_param);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, result, &arc, arc_param))
    {
        result->param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == searchElStr3(context, &order_role_relation, &arc, result))
        {
            scp_iterator3_free(it);
            return SCP_RESULT_TRUE;
        }
        result->param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

scp_result resolve_ordinal_set_rrel(sc_memory_context *context, scp_operand *arc_param, scp_operand *result)
{
    scp_operand arc;
    scp_iterator3 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_DEFAULT_NODE_ASSIGN((*result));
    arc_param->param_type = SCP_FIXED;
    it = scp_iterator3_new(context, result, &arc, arc_param);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, result, &arc, arc_param))
    {
        result->param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == searchElStr3(context, &ordinal_set_rrel, &arc, result))
        {
            scp_iterator3_free(it);
            return SCP_RESULT_TRUE;
        }
        result->param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SCP_RESULT_FALSE;
}


scp_result get_set_power(sc_memory_context *context, scp_operand *set, scp_uint32 *result)
{
    scp_operand arc, node;
    scp_iterator3 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    MAKE_DEFAULT_OPERAND_ASSIGN(node);
    (*result) = 0;
    it = scp_iterator3_new(context, set, &arc, &node);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, set, &arc, &node))
    {
        (*result)++;
    }
    scp_iterator3_free(it);
    return SCP_RESULT_TRUE;
}

void finish_question_successfully(sc_memory_context *context, scp_operand *param)
{
    scp_operand arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    genElStr3(context, &question_finished_successfully, &arc, param);
}

void finish_question_unsuccessfully(sc_memory_context *context, scp_operand *param)
{
    scp_operand arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    genElStr3(context, &question_finished_unsuccessfully, &arc, param);
}

void mark_scp_process_as_useless(sc_memory_context *context, scp_operand *param)
{
    scp_operand arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    genElStr3(context, &useless_scp_process, &arc, param);
    //printf("PROCESS DESTROYING...\n");
}

void set_active_operator(sc_memory_context *context, scp_operand *scp_operator_node)
{
    scp_operand arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    genElStr3(context, &active_scp_operator, &arc, scp_operator_node);
}

void set_author(sc_memory_context *context, scp_operand *quest, scp_operand *author)
{
    scp_operand authors, arc1, arc3;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_NODE_ASSIGN(authors);
    genElStr5(context, &authors, &arc3, quest, &arc1, &nrel_authors);
    authors.param_type = SCP_FIXED;
    genElStr3(context, &authors, &arc1, author);
}

void run_scp_program(sc_memory_context *context, scp_operand *scp_program)
{
    scp_operand arc1, arc2, quest, params;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_NODE_ASSIGN(quest);
    scp_program->param_type = SCP_FIXED;
    genElStr5(context, &quest, &arc1, scp_program, &arc2, ordinal_rrels + 1);
    quest.param_type = SCP_FIXED;
    MAKE_DEFAULT_NODE_ASSIGN(params);
    genElStr5(context, &quest, &arc1, &params, &arc2, ordinal_rrels + 2);
    set_author(context, &quest, &abstract_scp_machine);
    genElStr3(context, &question_scp_interpretation_request, &arc1, &quest);
    genElStr3(context, &question_initiated, &arc1, &quest);
}

void load_set_to_hash(sc_memory_context *context, scp_operand *set, GHashTable *table)
{
    scp_operand arc1, elem;
    scp_iterator3 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_OPERAND_ASSIGN(elem);
    it = scp_iterator3_new(context, set, &arc1, &elem);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, set, &arc1, &elem))
    {
        g_hash_table_add(table, MAKE_HASH(elem));
    }
    scp_iterator3_free(it);
}

void cantorize_set(sc_memory_context *context, scp_operand *set)
{
    GHashTable *table;
    scp_operand elem, arc;
    MAKE_DEFAULT_ARC_ASSIGN(arc);
    arc.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(elem);
    table = g_hash_table_new(NULL, NULL);
    scp_iterator3 *it = scp_iterator3_new(context, set, &arc, &elem);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, set, &arc, &elem))
    {
        if (TRUE == g_hash_table_contains(table, MAKE_HASH(elem)))
        {
            arc.param_type = SCP_FIXED;
            eraseEl(context, &arc);
            arc.param_type = SCP_ASSIGN;
        }
        else
        {
            g_hash_table_add(table, MAKE_HASH(elem));
        }
    }
    scp_iterator3_free(it);
    g_hash_table_destroy(table);
}

void erase_var_set_values(sc_memory_context *context, scp_operand *set)
{
    scp_operand arc1, arc2, elem, value;
    scp_iterator5 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    arc1.erase = SCP_TRUE;
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(elem);
    MAKE_DEFAULT_OPERAND_ASSIGN(value);
    it = scp_iterator5_new(context, set, &arc1, &elem, &arc2, &rrel_assign);
    while (SCP_RESULT_TRUE == scp_iterator5_next(context, it, set, &arc1, &elem, &arc2, &rrel_assign))
    {
        elem.param_type = SCP_FIXED;
        arc1.param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == searchElStr3(context, &rrel_scp_var, &arc2, &arc1))
        {
            arc1.param_type = SCP_ASSIGN;
            eraseSetStr5(context, &elem, &arc1, &value, &arc2, &nrel_value);
        }
        arc1.param_type = SCP_ASSIGN;
        elem.param_type = SCP_ASSIGN;
    }
    scp_iterator5_free(it);
}

sc_addr resolve_sc_addr_from_pointer(gpointer data)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(GPOINTER_TO_INT(data));
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT(GPOINTER_TO_INT(data));
    return elem;
}

sc_addr resolve_sc_addr_from_int(scp_uint32 data)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(data);
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT(data);
    return elem;
}
