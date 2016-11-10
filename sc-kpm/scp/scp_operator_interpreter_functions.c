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

#include "scp_operator_interpreter_functions.h"

#include "scp_utils.h"
#include "scp_interpreter_utils.h"
#include "scp_keynodes.h"

#include <stdio.h>

scp_uint32 check_ordinal_rrel(sc_memory_context *context, scp_operand *node, scp_uint32 count)
{
    scp_uint32 i = 0;
    for (i = 1; i <= count; i++)
    {
        if (SCP_RESULT_TRUE == ifCoin(context, node, ordinal_rrels + i))
        {
            return i;
        }
    }
    return 0;
}

scp_uint32 check_ordinal_set_rrel(sc_memory_context *context, scp_operand *node, scp_uint32 count)
{
    scp_uint32 i = 0;
    for (i = 1; i <= count; i++)
    {
        if (SCP_RESULT_TRUE == ifCoin(context, node, ordinal_set_rrels + i))
        {
            return i;
        }
    }
    return 0;
}

scp_result resolve_operands_modifiers(sc_memory_context *context, scp_operand *scp_operator_node, scp_operand *operands, scp_uint32 count)
{
    scp_operand arc1, arc2, modifier, operand_node, operand;
    scp_iterator3 *it, *it0;
    scp_uint32 rrel_number = 0;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand_node);
    MAKE_DEFAULT_NODE_ASSIGN(modifier);
    scp_operator_node->param_type = SCP_FIXED;
    it0 = scp_iterator3_new(context, scp_operator_node, &arc1, &operand_node);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it0, scp_operator_node, &arc1, &operand_node))
    {
        arc1.param_type = SCP_FIXED;
        operand_node.param_type = SCP_FIXED;
        rrel_number = 0;
        MAKE_DEFAULT_OPERAND_ASSIGN(operand);
        operand.param_type = SCP_FIXED;
        it = scp_iterator3_new(context, &modifier, &arc2, &arc1);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &modifier, &arc2, &arc1))
        {
            if (rrel_number == 0)
            {
                rrel_number = check_ordinal_rrel(context, &modifier, count);
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

            // Element type
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_node))
            {
                operand.element_type = operand.element_type | scp_type_node;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_link))
            {
                operand.element_type = operand.element_type | scp_type_link;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_arc))
            {
                operand.element_type = operand.element_type | scp_type_arc;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_edge))
            {
                operand.element_type = operand.element_type | scp_type_edge_common;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_temp))
            {
                operand.element_type = operand.element_type | scp_type_arc_temp;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_perm))
            {
                operand.element_type = operand.element_type | scp_type_arc_perm;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_access))
            {
                operand.element_type = operand.element_type | scp_type_arc_access;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_common))
            {
                operand.element_type = operand.element_type | scp_type_arc_common;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_pos_const_perm))
            {
                operand.element_type = (operand.element_type | scp_type_arc_access | scp_type_arc_pos | scp_type_arc_perm | scp_type_const);
                continue;
            }


            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_not_binary_tuple))
            {
                operand.element_type = (operand.element_type | scp_type_node_not_binary_tuple);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_struct))
            {
                operand.element_type = (operand.element_type | scp_type_node_struct);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_role_relation))
            {
                operand.element_type = (operand.element_type | scp_type_node_role_relation);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_norole_relation))
            {
                operand.element_type = (operand.element_type | scp_type_node_norole_relation);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_not_relation))
            {
                operand.element_type = (operand.element_type | scp_type_node_not_relation);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_abstract))
            {
                operand.element_type = (operand.element_type | scp_type_node_abstract);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_material))
            {
                operand.element_type = (operand.element_type | scp_type_node_material);
                continue;
            }


            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_pos))
            {
                operand.element_type = operand.element_type | scp_type_arc_pos;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_neg))
            {
                operand.element_type = operand.element_type | scp_type_arc_neg;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_fuz))
            {
                operand.element_type = operand.element_type | scp_type_arc_fuz;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_var))
            {
                operand.element_type = operand.element_type | scp_type_var;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_const))
            {
                operand.element_type = operand.element_type | scp_type_const;
                continue;
            }

            // Set
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_set))
            {
                operand.set = SCP_TRUE;
                continue;
            }

            // Erase
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_erase))
            {
                operand.erase = SCP_TRUE;
                continue;
            }
        }
        scp_iterator3_free(it);

        if (rrel_number > 0)
        {
            operands[rrel_number - 1] = operand;
            operands[rrel_number - 1].addr = operand_node.addr;
        }
        arc1.param_type = SCP_ASSIGN;
        operand_node.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it0);
    return SCP_RESULT_TRUE;
}

scp_result resolve_operands_modifiers_with_set(sc_memory_context *context, scp_operand *scp_operator_node, scp_operand *operands, scp_operand *sets, scp_uint32 count)
{
    scp_operand arc1, arc2, modifier, operand_node, operand;
    scp_iterator3 *it, *it0;
    scp_uint32 rrel_number = 0, set_number = 0;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand);
    MAKE_DEFAULT_OPERAND_ASSIGN(operand_node);
    MAKE_DEFAULT_NODE_ASSIGN(modifier);
    for (set_number = 0; set_number < count; set_number++)
    {
        sets[set_number].set = SCP_FALSE;
    }
    set_number = 0;
    scp_operator_node->param_type = SCP_FIXED;
    it0 = scp_iterator3_new(context, scp_operator_node, &arc1, &operand_node);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it0, scp_operator_node, &arc1, &operand_node))
    {
        arc1.param_type = SCP_FIXED;
        operand_node.param_type = SCP_FIXED;
        rrel_number = 0;
        set_number = 0;
        MAKE_DEFAULT_OPERAND_ASSIGN(operand);
        it = scp_iterator3_new(context, &modifier, &arc2, &arc1);
        while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, &modifier, &arc2, &arc1))
        {
            if (rrel_number == 0)
            {
                rrel_number = check_ordinal_rrel(context, &modifier, count);
                if (rrel_number > 0)
                {
                    continue;
                }
            }

            if (set_number == 0)
            {
                set_number = check_ordinal_set_rrel(context, &modifier, count);
                if (set_number > 0)
                {
                    operand.set = SCP_TRUE;
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

            // Element type
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_node))
            {
                operand.element_type = operand.element_type | scp_type_node;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_link))
            {
                operand.element_type = operand.element_type | scp_type_link;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_arc))
            {
                operand.element_type = operand.element_type | (~scp_type_node);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_temp))
            {
                operand.element_type = operand.element_type | scp_type_arc_temp;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_perm))
            {
                operand.element_type = operand.element_type | scp_type_arc_perm;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_pos))
            {
                operand.element_type = operand.element_type | scp_type_arc_pos;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_neg))
            {
                operand.element_type = operand.element_type | scp_type_arc_neg;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_fuz))
            {
                operand.element_type = operand.element_type | scp_type_arc_fuz;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_var))
            {
                operand.element_type = operand.element_type | scp_type_var;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_const))
            {
                operand.element_type = operand.element_type | scp_type_const;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_common))
            {
                operand.element_type = operand.element_type | scp_type_arc_common;
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_pos_const_perm))
            {
                operand.element_type = (operand.element_type | scp_type_arc_access | scp_type_arc_pos | scp_type_arc_perm | scp_type_const);
                continue;
            }

            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_not_binary_tuple))
            {
                operand.element_type = (operand.element_type | scp_type_node_not_binary_tuple);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_struct))
            {
                operand.element_type = (operand.element_type | scp_type_node_struct);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_role_relation))
            {
                operand.element_type = (operand.element_type | scp_type_node_role_relation);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_norole_relation))
            {
                operand.element_type = (operand.element_type | scp_type_node_norole_relation);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_not_relation))
            {
                operand.element_type = (operand.element_type | scp_type_node_not_relation);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_abstract))
            {
                operand.element_type = (operand.element_type | scp_type_node_abstract);
                continue;
            }
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_material))
            {
                operand.element_type = (operand.element_type | scp_type_node_material);
                continue;
            }

            // Set
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_set))
            {
                operand.set = SCP_TRUE;
                continue;
            }

            // Erase
            if (SCP_RESULT_TRUE == ifCoin(context, &modifier, &rrel_erase))
            {
                operand.erase = SCP_TRUE;
                continue;
            }
        }
        scp_iterator3_free(it);

        if (rrel_number > 0)
        {
            operands[rrel_number - 1] = operand;
            operands[rrel_number - 1].addr = operand_node.addr;
        }
        else if (set_number > 0)
        {
            sets[set_number - 1] = operand;
            sets[set_number - 1].addr = operand_node.addr;
        }
        arc1.param_type = SCP_ASSIGN;
        operand_node.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it0);
    return SCP_RESULT_TRUE;
}

scp_result find_scp_process_for_scp_operator(sc_memory_context *context, scp_operand *scp_operator_node, scp_operand *scp_process_node)
{
    scp_operand arc1;
    scp_iterator3 *it;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_NODE_ASSIGN((*scp_process_node));
    it = scp_iterator3_new(context, scp_process_node, &arc1, scp_operator_node);
    while (SCP_RESULT_TRUE == scp_iterator3_next(context, it, scp_process_node, &arc1, scp_operator_node))
    {
        scp_process_node->param_type = SCP_FIXED;
        if (SCP_RESULT_TRUE == searchElStr3(context, &scp_process, &arc1, scp_process_node))
        {
            scp_process_node->param_type = SCP_FIXED;
            scp_iterator3_free(it);
            return SCP_RESULT_TRUE;
        }
        scp_process_node->param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it);
    return SCP_RESULT_FALSE;
}

scp_result get_operand_value(sc_memory_context *context, scp_operand *operand, scp_operand *operand_value)
{
    scp_operand var_value, operand_node, arc1, arc3;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    arc3.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(var_value);
    MAKE_DEFAULT_OPERAND_FIXED(operand_node);
    var_value.param_type = SCP_ASSIGN;
    (*operand_value) = (*operand);
    operand_node.addr = operand->addr;
    if (operand->param_type == SCP_FIXED)
    {
        if (SCP_VAR == operand->operand_type)
        {
            if (SCP_RESULT_TRUE == searchElStr5(context, &operand_node, &arc3, &var_value, &arc1, &nrel_value))
            {
                operand_value->addr = var_value.addr;
            }
            else
            {
                printEl(context, operand);
                print_error("scp-operator interpreting", "Variable has FIXED modifier, but has no value");
                return SCP_RESULT_ERROR;
            }
        }
    }
    else
    {
        if (SCP_CONST == operand->operand_type)
        {
            printEl(context, operand);
            print_error("scp-operator interpreting", "Constant has ASSIGN modifier");
            return SCP_RESULT_ERROR;
        }
        else
        {
            eraseElStr5(context, &operand_node, &arc3, &var_value, &arc1, &nrel_value);
        }
    }
    return SCP_RESULT_TRUE;
}

scp_result get_operands_values(sc_memory_context *context, scp_operand *operands, scp_operand *operands_values, scp_uint32 count)
{
    scp_operand arc1, arc3, var_value, operand_node;
    scp_uint32 i;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    arc3.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(var_value);
    MAKE_DEFAULT_OPERAND_FIXED(operand_node);
    var_value.param_type = SCP_ASSIGN;
    for (i = 0; i < count; i++)
    {
        operands_values[i] = operands[i];
        operand_node.addr = operands[i].addr;
        if (operands[i].param_type == SCP_FIXED)
        {
            if (SCP_VAR == operands[i].operand_type)
            {
                if (SCP_RESULT_TRUE == searchElStr5(context, &operand_node, &arc3, &var_value, &arc1, &nrel_value))
                {
                    operands_values[i].addr = var_value.addr;
                }
                else
                {
                    printEl(context, operands + i);
                    print_error("scp-operator interpreting", "Variable has FIXED modifier, but has no value");
                    printf("wrong parameter %d\n", i + 1);
                    return SCP_RESULT_ERROR;
                }
            }
        }
        else
        {
            if (SCP_CONST == operands[i].operand_type)
            {
                printEl(context, operands + i);
                print_error("scp-operator interpreting", "Constant has ASSIGN modifier");
                printf("wrong parameter - %d\n", i + 1);
                return SCP_RESULT_ERROR;
            }
            else
            {
                eraseElStr5(context, &operand_node, &arc3, &var_value, &arc1, &nrel_value);
            }
        }
    }
    return SCP_RESULT_TRUE;
}

scp_result get_set_operands_values(sc_memory_context *context, scp_operand *operands, scp_operand *operands_values, scp_uint32 count)
{
    scp_operand arc1, arc3, var_value, operand_node;
    scp_uint32 i;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    arc3.erase = SCP_TRUE;
    MAKE_DEFAULT_OPERAND_ASSIGN(var_value);
    MAKE_DEFAULT_OPERAND_FIXED(operand_node);
    var_value.param_type = SCP_ASSIGN;
    for (i = 0; i < count; i++)
    {
        if (operands[i].set != SCP_TRUE)
        {
            operands_values[i].set = SCP_FALSE;
            continue;
        }
        operands_values[i] = operands[i];
        operand_node.addr = operands[i].addr;
        if (operands[i].param_type == SCP_FIXED)
        {
            if (SCP_VAR == operands[i].operand_type)
            {
                if (SCP_RESULT_TRUE == searchElStr5(context, &operand_node, &arc3, &var_value, &arc1, &nrel_value))
                {
                    operands_values[i].addr = var_value.addr;
                }
                else
                {
                    printEl(context, operands + i);
                    print_error("scp-operator interpreting", "Variable has FIXED modifier, but has no value");
                    printf("wrong parameter - set_%d\n", i + 1);
                    return SCP_RESULT_ERROR;
                }
            }
        }
        else
        {
            if (SCP_CONST == operands[i].operand_type)
            {
                printEl(context, operands + i);
                print_error("scp-operator interpreting", "Constant has ASSIGN modifier");
                printf("wrong parameter - set_%d\n", i + 1);
                return SCP_RESULT_ERROR;
            }
            else
            {
                eraseElStr5(context, &operand_node, &arc3, &var_value, &arc1, &nrel_value);
            }
        }
    }
    return SCP_RESULT_TRUE;
}

scp_result set_operands_values(sc_memory_context *context, scp_operand *operands, scp_operand *operands_values, scp_uint32 count)
{
    scp_operand arc2, arc3, operand_node;
    scp_uint32 i;
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_OPERAND_FIXED(operand_node);
    for (i = 0; i < count; i++)
    {
        if (operands[i].param_type == SCP_ASSIGN)
        {
            operand_node.addr = operands[i].addr;
            operands_values[i].param_type = SCP_FIXED;
            genElStr5(context, &operand_node, &arc3, operands_values + i, &arc2, &nrel_value);
        }
    }
    return SCP_RESULT_TRUE;
}

scp_result set_set_operands_values(sc_memory_context *context, scp_operand *operands, scp_operand *operands_values, scp_uint32 count)
{
    scp_operand arc2, arc3, operand_node;
    scp_uint32 i;
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_COMMON_ARC_ASSIGN(arc3);
    MAKE_DEFAULT_OPERAND_FIXED(operand_node);
    for (i = 0; i < count; i++)
    {
        if (operands[i].set == SCP_TRUE && operands[i].param_type == SCP_ASSIGN)
        {
            operand_node.addr = operands[i].addr;
            operands_values[i].param_type = SCP_FIXED;
            genElStr5(context, &operand_node, &arc3, operands_values + i, &arc2, &nrel_value);
        }
    }
    return SCP_RESULT_TRUE;
}

scp_result search_superset_relating_to_given_class(sc_memory_context *context, scp_operand *set, scp_operand *class_node, scp_operand *superset)
{
    scp_operand arc1, arc3, curr_superset;
    scp_iterator5 *it1;

    MAKE_DEFAULT_OPERAND_ASSIGN(curr_superset);
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_COMMON_ARC_ASSIGN(arc3);

    it1 = scp_iterator5_new(context, &curr_superset, &arc3, set, &arc1, &nrel_inclusion);
    while (SCP_TRUE == scp_iterator5_next(context, it1, &curr_superset, &arc3, set, &arc1, &nrel_inclusion))
    {
        curr_superset.param_type = SCP_FIXED;
        if (SCP_TRUE == searchElStr3(context, class_node, &arc1, &curr_superset))
        {
            superset->addr = curr_superset.addr;
            scp_iterator5_free(it1);
            return SCP_TRUE;
        }
        if (SCP_TRUE == search_superset_relating_to_given_class(context, &curr_superset, class_node, superset))
        {
            scp_iterator5_free(it1);
            return SCP_TRUE;
        }
        curr_superset.param_type = SCP_ASSIGN;
    }
    scp_iterator5_free(it1);

	return SCP_TRUE;
}

scp_result register_scp_based_sc_agent(sc_memory_context *context, scp_operand *agent, scp_operand *program, scp_operand *event_type, scp_operand *event_elem)
{
    scp_operand abstract_agent1, abstract_agent2, arc1, arc2, arc3, program_set, init_cond;
    scp_iterator3 *it1;

    MAKE_DEFAULT_NODE_ASSIGN(abstract_agent1);
    MAKE_DEFAULT_NODE_ASSIGN(abstract_agent2);
    MAKE_DEFAULT_NODE_ASSIGN(program_set);
    MAKE_DEFAULT_NODE_ASSIGN((*program));
    MAKE_DEFAULT_OPERAND_ASSIGN(init_cond);
    MAKE_DEFAULT_OPERAND_ASSIGN((*event_elem));
    MAKE_DEFAULT_OPERAND_ASSIGN((*event_type));
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    MAKE_DEFAULT_ARC_ASSIGN(arc2);
    MAKE_COMMON_ARC_ASSIGN(arc3);

    it1 = scp_iterator3_new(context, &abstract_agent1, &arc1, agent);
    while (SCP_TRUE == scp_iterator3_next(context, it1, &abstract_agent1, &arc1, agent))
    {
        abstract_agent1.param_type = SCP_FIXED;
        if (SCP_TRUE == searchElStr3(context, &platform_independent_abstract_sc_agent, &arc1, &abstract_agent1))
            break;
        abstract_agent1.param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it1);
    if (abstract_agent1.param_type == SCP_ASSIGN)
        return SCP_FALSE;

    // Search agent program for given agent
    if (SCP_TRUE != searchElStr5(context, &program_set, &arc3, &abstract_agent1, &arc2, &nrel_sc_agent_program))
        return SCP_FALSE;
    program_set.param_type = SCP_FIXED;
    it1 = scp_iterator3_new(context, &program_set, &arc1, program);
    while (SCP_TRUE == scp_iterator3_next(context, it1, &program_set, &arc1, program))
    {
        program->param_type = SCP_FIXED;
        if (SCP_TRUE == searchElStr3(context, &agent_scp_program, &arc1, program))
            break;
        program->param_type = SCP_ASSIGN;
    }
    scp_iterator3_free(it1);
    if (program->param_type == SCP_ASSIGN)
        return SCP_FALSE;

    // Search primary init condition for given agent
    if (SCP_TRUE != search_superset_relating_to_given_class(context, &abstract_agent1, &abstract_sc_agent, &abstract_agent2))
        return SCP_FALSE;
    abstract_agent2.param_type = SCP_FIXED;
    if (SCP_TRUE != searchElStr5(context, &abstract_agent2, &arc3, &init_cond, &arc2, &nrel_primary_initiation_condition))
        return SCP_FALSE;

    init_cond.param_type = SCP_FIXED;
    if (SCP_TRUE != searchElStr3(context, event_type, &init_cond, event_elem))
        return SCP_FALSE;
    event_elem->param_type = SCP_FIXED;
    event_type->param_type = SCP_FIXED;

    return SCP_TRUE;
}

/* Goto */
scp_result goto_conditional_success(sc_memory_context *context, scp_operand *scp_operator_node)
{
    scp_operand arc1;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    genElStr3(context, &successfully_executed_scp_operator, &arc1, scp_operator_node);
    return SCP_RESULT_TRUE;
}

scp_result goto_conditional_unsuccess(sc_memory_context *context, scp_operand *scp_operator_node)
{
    scp_operand arc1;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    genElStr3(context, &unsuccessfully_executed_scp_operator, &arc1, scp_operator_node);
    return SCP_RESULT_TRUE;
}

scp_result goto_conditional_error(sc_memory_context *context, scp_operand *scp_operator_node)
{
    //!TODO Add error processing
    return SCP_RESULT_TRUE;
}

scp_result goto_unconditional(sc_memory_context *context, scp_operand *scp_operator_node)
{
    scp_operand arc1;
    MAKE_DEFAULT_ARC_ASSIGN(arc1);
    genElStr3(context, &executed_scp_operator, &arc1, scp_operator_node);

	return SCP_TRUE;
}

void operator_interpreting_crash(sc_memory_context *context, scp_operand *operator_node)
{
    scp_operand scp_process_node;
    find_scp_process_for_scp_operator(context, operator_node, &scp_process_node);
    mark_scp_process_as_useless(context, &scp_process_node);
}
