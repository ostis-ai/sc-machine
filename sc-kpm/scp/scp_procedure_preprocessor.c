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
#include "scp_procedure_preprocessor.h"
#include "scp_interpreter_utils.h"
#include <glib.h>

#include <stdio.h>

sc_event *event_procedure_preprocessing;

void append_to_hash(GHashTable *table, scp_operand *elem)
{
    g_hash_table_add(table, MAKE_PHASH(elem));
}

void append_to_set(gpointer key, gpointer value, gpointer set)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(GPOINTER_TO_INT(key));
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT(GPOINTER_TO_INT(key));
    sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, ((scp_operand *)set)->addr, elem);
}

void gen_set_from_hash(GHashTable *table, scp_operand *set)
{
    g_hash_table_foreach(table, append_to_set, set);
}

void append_all_relation_elements_to_hash_with_modifiers(sc_memory_context *context, GHashTable *table, scp_operand *set, scp_operand *parameter_set, scp_operand *const_set, scp_operand *vars_set)
{
    scp_operand arc1, arc2, curr_operand, operand_arc, modifier, modifier_arc,
                operand_element, operand_element_arc, operand_element_modifier, operand_element_modifier_arc;
    scp_iterator3 *it1, *it2, *it3;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_ARC_ASSIGN(operand_arc);
    MAKE_DEFAULT_ARC_ASSIGN(modifier_arc);
    MAKE_DEFAULT_ARC_ASSIGN(operand_element_arc);
    MAKE_DEFAULT_ARC_ASSIGN(operand_element_modifier_arc);
    modifier_arc.erase = SCP_TRUE;
    operand_element_modifier_arc.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(curr_operand);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand_element);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand_element_modifier);
    MAKE_DEFAULT_OPERAND_ASSIGN(modifier);
    // Operands loop
    it1 = scp_iterator3_new(context, set, &arc1, &curr_operand);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, set, &operand_arc, &curr_operand))
    {
        curr_operand.param_type = SCP_FIXED;
        operand_arc.param_type = SCP_FIXED;

        append_to_hash(table, &curr_operand);
        append_to_hash(table, &operand_arc);

        // Operand modifiers loop
        it2 = scp_iterator3_new(context, &modifier, &arc1, &operand_arc);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it2, &modifier, &modifier_arc, &operand_arc))
        {
            modifier.param_type = SCP_FIXED;
            modifier_arc.param_type = SCP_FIXED;

            append_to_hash(table, &modifier_arc);

            modifier_arc.param_type = SCP_ASSIGN;
            modifier.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it2);

        // Operand elements loop
        it2 = scp_iterator3_new(context, &curr_operand, &operand_element_arc, &operand_element);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it2, &curr_operand, &operand_element_arc, &operand_element))
        {
            operand_element.param_type = SCP_FIXED;
            operand_element_arc.param_type = SCP_FIXED;

            append_to_hash(table, &operand_element);
            append_to_hash(table, &operand_element_arc);

            // Operand element modifiers loop
            it3 = scp_iterator3_new(context, &operand_element_modifier, &operand_element_modifier_arc, &operand_element_arc);
            while (SCP_RESULT_TRUE == scp_iterator3_next(context, it3, &operand_element_modifier, &operand_element_modifier_arc, &operand_element_arc))
            {
                operand_element_modifier.param_type = SCP_FIXED;
                operand_element_modifier_arc.param_type = SCP_FIXED;

                // Variable case
                if (SCP_RESULT_TRUE == ifCoin(context, &rrel_scp_var, &operand_element_modifier))
                {
                    if (SCP_RESULT_TRUE == searchElStr5(context, parameter_set, &arc1, &operand_element, &arc2, &rrel_in))
                    {
                        if (SCP_RESULT_TRUE == ifVarAssign(context, &operand_element_modifier_arc))
                        {
                            eraseEl(context, &operand_element_modifier_arc);
                        }
                        genElStr3(context, &rrel_scp_const, &arc1, &operand_element_arc);

                        arc1.param_type = SCP_FIXED;
                        append_to_hash(table, &arc1);
                        arc1.param_type = SCP_ASSIGN;

                        genElStr3(context, const_set, &arc1, &operand_element);
                    }
                    else
                    {
                        append_to_hash(table, &operand_element_modifier_arc);
                        genElStr3(context, vars_set, &arc1, &operand_element);
                    }
                }
                else
                {
                    append_to_hash(table, &operand_element_modifier_arc);
                    // Constant case
                    if (SCP_RESULT_TRUE == ifCoin(context, &rrel_scp_const, &operand_element_modifier))
                    {
                        genElStr3(context, const_set, &arc1, &operand_element);
                    }
                }

                operand_element_modifier_arc.param_type = SCP_ASSIGN;
                operand_element_modifier.param_type = SCP_ASSIGN;
            }
            scp_iterator3_free(it3);

            operand_element_arc.param_type = SCP_ASSIGN;
            operand_element.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it2);

        operand_arc.param_type = SCP_ASSIGN;
        curr_operand.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it1);
}

void append_all_set_elements_to_hash_with_modifiers(sc_memory_context *context, GHashTable *table, scp_operand *set, scp_operand *parameter_set, scp_operand *const_set, scp_operand *vars_set)
{
    scp_operand arc1, arc2, curr_operand, operand_arc, modifier, modifier_arc;
    scp_iterator3 *it1, *it2;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_ARC_ASSIGN(operand_arc);
    MAKE_DEFAULT_ARC_ASSIGN(modifier_arc);
    modifier_arc.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(curr_operand);
    MAKE_DEFAULT_OPERAND_ASSIGN(modifier);
    // Elements loop
    it1 = scp_iterator3_new(context, set, &operand_arc, &curr_operand);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, set, &operand_arc, &curr_operand))
    {
        curr_operand.param_type = SCP_FIXED;
        operand_arc.param_type = SCP_FIXED;

        append_to_hash(table, &curr_operand);
        append_to_hash(table, &operand_arc);

        // Operand modifiers loop
        it2 = scp_iterator3_new(context, &modifier, &modifier_arc, &operand_arc);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it2, &modifier, &modifier_arc, &operand_arc))
        {
            modifier.param_type = SCP_FIXED;
            modifier_arc.param_type = SCP_FIXED;

            // Variable case
            if (SCP_RESULT_TRUE == ifCoin(context, &rrel_scp_var, &modifier))
            {
                if (SCP_RESULT_TRUE == searchElStr5(context, parameter_set, &arc1, &curr_operand, &arc2, &rrel_in))
                {
                    if (SCP_RESULT_TRUE == ifVarAssign(context, &modifier_arc))
                    {
                        eraseEl(context, &modifier_arc);
                    }
                    genElStr3(context, &rrel_scp_const, &arc1, &operand_arc);

                    arc1.param_type = SCP_FIXED;
                    append_to_hash(table, &arc1);
                    arc1.param_type = SCP_ASSIGN;

                    genElStr3(context, const_set, &arc1, &curr_operand);
                }
                else
                {
                    append_to_hash(table, &modifier_arc);
                    genElStr3(context, vars_set, &arc1, &curr_operand);
                }
            }
            else
            {
                append_to_hash(table, &modifier_arc);
                // Constant case
                if (SCP_RESULT_TRUE == ifCoin(context, &rrel_scp_const, &modifier))
                {
                    genElStr3(context, const_set, &arc1, &curr_operand);
                }
            }

            modifier_arc.param_type = SCP_ASSIGN;
            modifier.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it2);

        operand_arc.param_type = SCP_ASSIGN;
        curr_operand.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it1);
}

scp_result gen_system_structures(sc_memory_context *context, scp_operand *operator_set, scp_operand *parameter_set, scp_operand *vars_set, scp_operand *const_set, scp_operand *copying_consts_set, scp_operand *operators_copying_pattern)
{
    scp_operand arc1, arc2, curr_operator, operator_arc, operator_type, operator_input, curr_operand, operand_arc, modifier, modifier_arc,
                operand_arc_pattern;
    scp_iterator3 *it, *it1, *it2;
    GHashTable *table = g_hash_table_new(NULL, NULL);
    scp_bool cop_const = SCP_FALSE, is_const = SCP_FALSE, order_rel = SCP_FALSE;

    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_ARC_ASSIGN(operator_arc);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand_arc);
    MAKE_DEFAULT_ARC_ASSIGN(modifier_arc);
    modifier_arc.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(curr_operator);
    MAKE_DEFAULT_OPERAND_ASSIGN(curr_operand);
    MAKE_DEFAULT_OPERAND_ASSIGN(operator_arc);
    MAKE_DEFAULT_OPERAND_ASSIGN(operator_type);
    MAKE_DEFAULT_OPERAND_ASSIGN(operator_input);
    MAKE_DEFAULT_OPERAND_ASSIGN(modifier);
    MAKE_DEFAULT_OPERAND_FIXED(operand_arc_pattern);
    operand_arc_pattern.element_type = scp_type_arc_common | scp_type_const;
    append_to_hash(table, operator_set);
    it = scp_iterator3_new(context, operator_set, &operator_arc, &curr_operator);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, operator_set, &operator_arc, &curr_operator))
    {
        curr_operator.param_type = SCP_FIXED;
        operator_arc.param_type = SCP_FIXED;

        if (SCP_RESULT_TRUE == searchElStr3(context, &rrel_init, &arc1, &operator_arc))
        {
            append_to_hash(table, &arc1);
        }

        append_to_hash(table, &curr_operator);
        append_to_hash(table, &operator_arc);

        // Operator type loop
        operator_type.param_type = SCP_ASSIGN;
        operator_input.param_type = SCP_ASSIGN;
        it1 = scp_iterator3_new(context, &operator_input, &arc1, &curr_operator);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &operator_input, &arc1, &curr_operator))
        {
            operator_input.param_type = SCP_FIXED;

            if (SCP_RESULT_TRUE == searchElStr3(context, &scp_operator_atomic_type, &arc2, &operator_input))
            {
                append_to_hash(table, &arc1);
                operator_type.addr = operator_input.addr;
                operator_input.param_type = SCP_ASSIGN;
                continue;
            }

            if (SCP_RESULT_TRUE == ifCoin(context, &scp_operator_executable_after_all_previous, &operator_input)
                || SCP_RESULT_TRUE == ifCoin(context, &scp_operator_executable_after_one_of_previous, &operator_input))
            {
                append_to_hash(table, &arc1);
                operator_input.param_type = SCP_ASSIGN;
                continue;
            }

            operator_input.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it1);
        operator_type.param_type = SCP_FIXED;

        // Operands loop
        it1 = scp_iterator3_new(context, &curr_operator, &operand_arc, &curr_operand);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it1, &curr_operator, &operand_arc, &curr_operand))
        {
            curr_operand.param_type = SCP_FIXED;
            operand_arc.param_type = SCP_FIXED;
            cop_const = SCP_FALSE;
            is_const = SCP_FALSE;
            order_rel = SCP_FALSE;

            //Order relation case
            operand_arc_pattern.addr = operand_arc.addr;
            if (SCP_TRUE == ifType(context, &operand_arc_pattern))
            {
                if (SCP_TRUE != searchElStr3(context, operator_set, &arc1, &curr_operand))
                {
                    operand_arc.param_type = SCP_ASSIGN;
                    curr_operand.param_type = SCP_ASSIGN;
                    continue;
                }
                else
                {
                    order_rel = SCP_TRUE;
                }
            }

            append_to_hash(table, &curr_operand);
            append_to_hash(table, &operand_arc);

            // Operand modifiers loop
            it2 = scp_iterator3_new(context, &modifier, &modifier_arc, &operand_arc);
            while (SCP_RESULT_TRUE == scp_iterator3_next(context, it2, &modifier, &modifier_arc, &operand_arc))
            {
                modifier.param_type = SCP_FIXED;
                modifier_arc.param_type = SCP_FIXED;

                if (SCP_TRUE == order_rel)
                {
                    if (SCP_TRUE == ifCoin(context, &nrel_goto, &modifier)
                        || SCP_TRUE == ifCoin(context, &nrel_else, &modifier)
                        || SCP_TRUE == ifCoin(context, &nrel_then, &modifier)
                        || SCP_TRUE == ifCoin(context, &nrel_error, &modifier))
                    {
                        append_to_hash(table, &modifier_arc);
                        modifier_arc.param_type = SCP_ASSIGN;
                        modifier.param_type = SCP_ASSIGN;
                        break;
                    }
                    else
                    {
                        modifier_arc.param_type = SCP_ASSIGN;
                        modifier.param_type = SCP_ASSIGN;
                        continue;
                    }
                }

                if (SCP_RESULT_TRUE == ifCoin(context, &operator_type, &op_call)
                    && SCP_RESULT_TRUE == ifCoin(context, &modifier, ordinal_rrels + 2))
                {
                    cop_const = SCP_TRUE;
                    append_all_set_elements_to_hash_with_modifiers(context, table, &curr_operand, parameter_set, const_set, vars_set);
                }

                if (((SCP_RESULT_TRUE == ifCoin(context, &operator_type, &op_sys_gen)) || (SCP_RESULT_TRUE == ifCoin(context, &operator_type, &op_sys_search))) &&
                    ((SCP_RESULT_TRUE == ifCoin(context, &modifier, ordinal_rrels + 2)) || (SCP_RESULT_TRUE == ifCoin(context, &modifier, ordinal_rrels + 3))))
                {
                    cop_const = SCP_TRUE;
                    append_all_relation_elements_to_hash_with_modifiers(context, table, &curr_operand, parameter_set, const_set, vars_set);
                }

                // Variable case
                if (SCP_RESULT_TRUE == ifCoin(context, &rrel_scp_var, &modifier))
                {
                    if (SCP_RESULT_TRUE == searchElStr5(context, parameter_set, &arc1, &curr_operand, &arc2, &rrel_in))
                    {
                        if (SCP_RESULT_TRUE == ifVarAssign(context, &modifier_arc))
                        {
                            eraseEl(context, &modifier_arc);
                        }
                        genElStr3(context, &rrel_scp_const, &arc1, &operand_arc);

                        arc1.param_type = SCP_FIXED;
                        append_to_hash(table, &arc1);
                        arc1.param_type = SCP_ASSIGN;

                        genElStr3(context, const_set, &arc1, &curr_operand);
                    }
                    else
                    {
                        append_to_hash(table, &modifier_arc);
                        genElStr3(context, vars_set, &arc1, &curr_operand);
                    }
                }
                else
                {
                    //!TODO Check unnesseccary arcs
                    append_to_hash(table, &modifier_arc);
                    // Constant case
                    if (SCP_RESULT_TRUE == ifCoin(context, &rrel_scp_const, &modifier))
                    {
                        is_const = SCP_TRUE;
                    }
                }

                modifier_arc.param_type = SCP_ASSIGN;
                modifier.param_type = SCP_ASSIGN;
            }
            scp_iterator3_free(it2);

            if (SCP_TRUE == order_rel)
            {
                operand_arc.param_type = SCP_ASSIGN;
                curr_operand.param_type = SCP_ASSIGN;
                continue;
            }

            if (is_const == SCP_TRUE)
            {
                if (cop_const == SCP_TRUE)
                {
                    genElStr3(context, copying_consts_set, &arc1, &curr_operand);
                }
                else
                {
                    genElStr3(context, const_set, &arc1, &curr_operand);
                }
            }

            operand_arc.param_type = SCP_ASSIGN;
            curr_operand.param_type = SCP_ASSIGN;
        }
        scp_iterator3_free(it1);

        operator_arc.param_type = SCP_ASSIGN;
        curr_operator.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);

    cantorize_set(context, const_set);
    cantorize_set(context, copying_consts_set);
    cantorize_set(context, vars_set);
    //printEl(copying_consts_set);
    //printEl(context, vars_set);printEl(context, vars_set);
    //printEl(context, const_set);
    //printf("SIZE: %d\n", g_hash_table_size(table));
    gen_set_from_hash(table, operators_copying_pattern);
    g_hash_table_destroy(table);
    return SCP_RESULT_TRUE;
}

sc_result preprocess_scp_procedure(const sc_event *event, sc_addr arg)
{
    scp_operand arc1, arc2, arc3, scp_procedure_node, node1, scp_procedure_params,
                scp_procedure_consts, scp_procedure_copying_consts, scp_procedure_vars, scp_procedure_operators, scp_procedure_operators_copying_pattern;
    MAKE_DEFAULT_OPERAND_FIXED(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);

    arc1.addr = arg;
    arc1.element_type = scp_type_arc_pos_const_perm;

    MAKE_DEFAULT_NODE_ASSIGN(node1);
    MAKE_DEFAULT_OPERAND_ASSIGN(scp_procedure_node);
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &arc1))
    {
        return SC_RESULT_ERROR;
    }
    if (SCP_RESULT_TRUE != ifType(s_default_ctx, &arc1))
    {
        return SC_RESULT_OK;
    }

    if (SCP_RESULT_TRUE != searchElStr3(s_default_ctx, &node1, &arc1, &scp_procedure_node))
    {
        return SCP_RESULT_ERROR;
    }
    scp_procedure_node.param_type = SCP_FIXED;
    if (SCP_RESULT_TRUE != ifVarAssign(s_default_ctx, &scp_procedure_node))
    {
        return SC_RESULT_ERROR;
    }
    arc1.erase = SCP_TRUE;
    eraseEl(s_default_ctx, &arc1);

    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc3);

    if (SCP_RESULT_TRUE == searchElStr3(s_default_ctx, &prepared_scp_program, &arc1, &scp_procedure_node))
    {
        return SC_RESULT_OK;
    }

    //Debug info
    printf("PREPROCESSING: ");
    MAKE_DEFAULT_OPERAND_ASSIGN(node1);
    searchElStr5(s_default_ctx, &scp_procedure_node, &arc3, &node1, &arc2, &nrel_system_identifier);
    printNl(s_default_ctx, &node1);

    MAKE_DEFAULT_OPERAND_ASSIGN(scp_procedure_operators);
    searchElStr5(s_default_ctx, &scp_procedure_node, &arc1, &scp_procedure_operators, &arc2, &rrel_operators);
    scp_procedure_operators.param_type = SCP_FIXED;
    MAKE_DEFAULT_OPERAND_ASSIGN(scp_procedure_params);
    searchElStr5(s_default_ctx, &scp_procedure_node, &arc1, &scp_procedure_params, &arc2, &rrel_params);
    scp_procedure_params.param_type = SCP_FIXED;

    MAKE_DEFAULT_NODE_ASSIGN(scp_procedure_consts);
    genElStr5(s_default_ctx, &scp_procedure_node, &arc3, &scp_procedure_consts, &arc2, &nrel_scp_program_const);
    scp_procedure_consts.param_type = SCP_FIXED;
    MAKE_DEFAULT_NODE_ASSIGN(scp_procedure_copying_consts);
    genElStr5(s_default_ctx, &scp_procedure_node, &arc3, &scp_procedure_copying_consts, &arc2, &nrel_scp_program_copied_const);
    scp_procedure_copying_consts.param_type = SCP_FIXED;
    MAKE_DEFAULT_NODE_ASSIGN(scp_procedure_vars);
    genElStr5(s_default_ctx, &scp_procedure_node, &arc3, &scp_procedure_vars, &arc2, &nrel_scp_program_var);
    scp_procedure_vars.param_type = SCP_FIXED;
    MAKE_DEFAULT_NODE_ASSIGN(scp_procedure_operators_copying_pattern);
    genElStr5(s_default_ctx, &scp_procedure_node, &arc3, &scp_procedure_operators_copying_pattern, &arc2, &nrel_template_of_scp_process_creation);
    scp_procedure_operators_copying_pattern.param_type = SCP_FIXED;
    if (SCP_RESULT_TRUE == gen_system_structures(s_default_ctx, &scp_procedure_operators, &scp_procedure_params, &scp_procedure_vars, &scp_procedure_consts, &scp_procedure_copying_consts, &scp_procedure_operators_copying_pattern))
    {
        genElStr3(s_default_ctx, &prepared_scp_program, &arc1, &scp_procedure_node);
        //printf("PREPROCESSING FINISHED\n");
        return SC_RESULT_OK;
    }
    else
    {
        return SC_RESULT_ERROR;
    }
}

scp_result scp_procedure_preprocessor_init()
{
    event_procedure_preprocessing = sc_event_new(s_default_ctx, correct_scp_program.addr, SC_EVENT_ADD_OUTPUT_ARC, 0, (fEventCallback)preprocess_scp_procedure, 0);
    if (event_procedure_preprocessing == null_ptr)
        return SCP_RESULT_ERROR;
    return SCP_RESULT_TRUE;
}

scp_result scp_procedure_preprocessor_shutdown()
{
    sc_event_destroy(event_procedure_preprocessing);
    return SCP_RESULT_TRUE;
}
