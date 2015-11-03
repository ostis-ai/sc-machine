/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_semantic_neighborhood.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>
#include <stdio.h>

void search_translation(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator5 *it5;
    sc_iterator3 *it3, *it4;
    sc_bool found = SC_FALSE;

    // iterate translations of sc-element
    it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     elem,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_translation);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        found = SC_TRUE;

        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))))
            continue;

        appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 3));

        // iterate translation sc-links
        it3 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator5_value(it5, 0),
                                     sc_type_arc_pos_const_perm,
                                     0);
        while (sc_iterator3_next(it3) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 2))))
                continue;

            // iterate input arcs for link
            it4 = sc_iterator3_a_a_f_new(s_default_ctx,
                                         sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it3, 2));
            while (sc_iterator3_next(it4) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 0))))
                    continue;
                if (sc_helper_check_arc(s_default_ctx, keynode_languages, sc_iterator3_value(it4, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                {
                    appendIntoAnswer(answer, sc_iterator3_value(it4, 0));
                    appendIntoAnswer(answer, sc_iterator3_value(it4, 1));
                }
            }
            sc_iterator3_free(it4);

            // iterate input arcs for arc
            it4 = sc_iterator3_a_a_f_new(s_default_ctx,
                                         sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it3, 1));
            while (sc_iterator3_next(it4) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 1))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it4, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it4, 1));
            }
            sc_iterator3_free(it4);

            appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it3, 2));
        }
        sc_iterator3_free(it3);

    }
    sc_iterator5_free(it5);

    if (found == SC_TRUE)
    {
        appendIntoAnswer(answer, keynode_nrel_translation);
    }
}

void search_arc_components(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_type type;
    sc_addr begin, end;

    if (SC_RESULT_OK != sc_memory_get_element_type(s_default_ctx, elem, &type))
        return;
    if (!(type & ~sc_type_node))
        return;
    if (SC_RESULT_OK != sc_memory_get_arc_begin(s_default_ctx, elem, &begin))
        return;
    if (SC_RESULT_OK != sc_memory_get_arc_end(s_default_ctx, elem, &end))
        return;

    appendIntoAnswer(answer, begin);
    appendIntoAnswer(answer, end);
}

void search_nonbinary_relation(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it1, *it2, *it3;
    sc_type el_type;

    // iterate input arcs for elem
    it1 = sc_iterator3_a_a_f_new(s_default_ctx,
                                 sc_type_node | sc_type_const,
                                 sc_type_arc_pos_const_perm,
                                 elem);
    while (sc_iterator3_next(it1) == SC_TRUE)
    {
        // if elem is a link of non-binary relation
        if (SC_TRUE == sc_helper_check_arc(s_default_ctx, keynode_nonbinary_relation, sc_iterator3_value(it1, 0), sc_type_arc_pos_const_perm))
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 1))))
                continue;

            // iterate other elements of link
            it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                         elem,
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
            while (sc_iterator3_next(it2) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it2, 2));

                search_arc_components(sc_iterator3_value(it2, 2), answer, sys_off);

                // iterate attributes of link
                it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it2, 1));
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
                        continue;

                    sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
                    if (!(el_type & (sc_type_node_norole | sc_type_node_role)))
                        continue;

                    appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                    appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
                }
                sc_iterator3_free(it3);
            }
            sc_iterator3_free(it2);

            appendIntoAnswer(answer, sc_iterator3_value(it1, 0));
            appendIntoAnswer(answer, sc_iterator3_value(it1, 1));
        }
    }
    sc_iterator3_free(it1);
}

void search_typical_sc_neighborhood(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it1, *it0;
    sc_iterator5 *it5;
    sc_bool found = SC_FALSE;

    // search for keynode_typical_sc_neighborhood
    it0 = sc_iterator3_a_a_f_new(s_default_ctx,
                                 sc_type_node | sc_type_const,
                                 sc_type_arc_pos_const_perm,
                                 elem);
    while (sc_iterator3_next(it0) == SC_TRUE)
    {
        if (SC_ADDR_IS_EQUAL(sc_iterator3_value(it0, 0), keynode_typical_sc_neighborhood))
        {
            found = SC_TRUE;
            // iterate input arcs for elem
            it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                         elem,
                                         sc_type_arc_pos_const_perm,
                                         0);
            while (sc_iterator3_next(it1) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it1, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it1, 2));
            }
            sc_iterator3_free(it1);

            appendIntoAnswer(answer, sc_iterator3_value(it0, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it0, 0));
            continue;
        }

        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it0, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it0, 1))))
            continue;

        it5 = sc_iterator5_f_a_f_a_f_new(s_default_ctx,
                                         keynode_sc_neighborhood,
                                         sc_type_arc_common | sc_type_const,
                                         sc_iterator3_value(it0, 0),
                                         sc_type_arc_pos_const_perm,
                                         keynode_nrel_strict_inclusion);
        if (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it0, 0));
            appendIntoAnswer(answer, sc_iterator3_value(it0, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
        }
        sc_iterator5_free(it5);

    }
    sc_iterator3_free(it0);
    if (found == SC_TRUE)
    {
        appendIntoAnswer(answer, keynode_typical_sc_neighborhood);
    }
}

sc_result agent_search_full_semantic_neighborhood(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2, *it3, *it4, *it6;
    sc_iterator5 *it5, *it_order, *it_order2;
    sc_type el_type;
    sc_bool sys_off = SC_TRUE;
    sc_bool key_order_found = SC_FALSE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_full_semantic_neighborhood, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get question argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        search_translation(sc_iterator3_value(it1, 2), answer, sys_off);
        search_arc_components(sc_iterator3_value(it1, 2), answer, sys_off);

        // iterate input arcs
        it2 = sc_iterator3_a_a_f_new(s_default_ctx,
                                     0,
                                     0,
                                     sc_iterator3_value(it1, 2));
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it2, 0));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));

            search_arc_components(sc_iterator3_value(it2, 0), answer, sys_off);

            // iterate input arcs into found arc, to find relations
            it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                         sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it3) == SC_TRUE)
            {
                sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
                if (!(el_type & (sc_type_node_norole | sc_type_node_role)))
                    continue;

                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));

                search_arc_components(sc_iterator3_value(it3, 0), answer, sys_off);

                // search typical sc-neighborhood if necessary
                if (SC_ADDR_IS_EQUAL(keynode_rrel_key_sc_element, sc_iterator3_value(it3, 0)))
                {
                    search_typical_sc_neighborhood(sc_iterator3_value(it2, 0), answer, sys_off);
                    search_translation(sc_iterator3_value(it2, 0), answer, sys_off);
                }

                // check if it's a quasy binary relation
                if (sc_helper_check_arc(s_default_ctx, keynode_quasybinary_relation, sc_iterator3_value(it3, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                {
                    // iterate elements of relation
                    it4 = sc_iterator3_f_a_a_new(s_default_ctx,
                                                 sc_iterator3_value(it2, 0),
                                                 sc_type_arc_pos_const_perm,
                                                 0);
                    while (sc_iterator3_next(it4) == SC_TRUE)
                    {
                        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 2))))
                            continue;

                        appendIntoAnswer(answer, sc_iterator3_value(it4, 1));
                        appendIntoAnswer(answer, sc_iterator3_value(it4, 2));

                        search_arc_components(sc_iterator3_value(it4, 2), answer, sys_off);

                        // iterate order relations between elements
                        it_order = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                                              sc_iterator3_value(it4, 2),
                                                              sc_type_arc_common | sc_type_const,
                                                              sc_type_node | sc_type_const,
                                                              sc_type_arc_pos_const_perm,
                                                              sc_type_node | sc_type_const);
                        while (sc_iterator5_next(it_order) == SC_TRUE)
                        {
                            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 1)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 2))
                                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 3)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 4))))
                                continue;

                            if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                                continue;
                            if (SC_FALSE == sc_helper_check_arc(s_default_ctx, sc_iterator3_value(it2, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
                                continue;

                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 1));
                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 2));
                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 3));
                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 4));

                        }
                        sc_iterator5_free(it_order);

                        // iterate roles of element in link
                        it6 = sc_iterator3_a_a_f_new(s_default_ctx,
                                                     sc_type_node | sc_type_const,
                                                     sc_type_arc_pos_const_perm,
                                                     sc_iterator3_value(it4, 1));
                        while (sc_iterator3_next(it6) == SC_TRUE)
                        {
                            sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it6, 0), &el_type);
                            if (!(el_type & sc_type_node_role))
                                continue;

                            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it6, 0))
                                                       || IS_SYSTEM_ELEMENT(sc_iterator3_value(it6, 1))))
                                continue;

                            appendIntoAnswer(answer, sc_iterator3_value(it6, 0));
                            appendIntoAnswer(answer, sc_iterator3_value(it6, 1));

                            search_arc_components(sc_iterator3_value(it6, 0), answer, sys_off);
                        }
                        sc_iterator3_free(it6);

                    }
                    sc_iterator3_free(it4);
                }
            }
            sc_iterator3_free(it3);

            // search all parents in quasybinary relation
            it5 = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                             sc_iterator3_value(it2, 0),
                                             sc_type_arc_common | sc_type_const,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_type_node | sc_type_const);
            while (sc_iterator5_next(it5) == SC_TRUE)
            {
                // check if it's a quasy binary relation
                if (sc_helper_check_arc(s_default_ctx, keynode_quasybinary_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm) == SC_TRUE)
                {
                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 2))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
                        continue;

                    appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
                    appendIntoAnswer(answer, sc_iterator5_value(it5, 2));
                    appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
                    appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

                    search_arc_components(sc_iterator5_value(it5, 2), answer, sys_off);
                }
            }
            sc_iterator5_free(it5);

            // search non-binary relation link
            search_nonbinary_relation(sc_iterator3_value(it2, 0), answer, sys_off);
        }
        sc_iterator3_free(it2);

        // iterate output arcs
        it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator3_value(it1, 2),
                                     0,
                                     0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 2));

            search_arc_components(sc_iterator3_value(it2, 2), answer, sys_off);

            // iterate input arcs into found arc, to find relations
            it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                         sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it3) == SC_TRUE)
            {
                sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
                if (!(el_type & (sc_type_node_norole | sc_type_node_role)))
                    continue;

                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));

                // search of key sc-elements order
                if (SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 0), keynode_rrel_key_sc_element))
                {
                    it_order2 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                                           sc_iterator3_value(it2, 1),
                                                           sc_type_arc_common | sc_type_const,
                                                           sc_type_arc_pos_const_perm,
                                                           sc_type_arc_pos_const_perm,
                                                           keynode_nrel_key_sc_element_base_order);
                    while (sc_iterator5_next(it_order2) == SC_TRUE)
                    {
                        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order2, 3))))
                            continue;

                        appendIntoAnswer(answer, sc_iterator5_value(it_order2, 1));
                        appendIntoAnswer(answer, sc_iterator5_value(it_order2, 3));
                        if (SC_FALSE == key_order_found)
                        {
                            key_order_found = SC_TRUE;
                            appendIntoAnswer(answer, keynode_nrel_key_sc_element_base_order);
                        }
                    }
                    sc_iterator5_free(it_order2);
                }
            }
            sc_iterator3_free(it3);

            // check if element is an sc-link
            if (SC_RESULT_OK == sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it2, 2), &el_type) &&
                (el_type | sc_type_link))
            {
                // iterate input arcs for link
                it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it2, 2));
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    if (sc_helper_check_arc(s_default_ctx, keynode_languages, sc_iterator3_value(it3, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                    {
                        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))))
                            continue;

                        appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                        appendIntoAnswer(answer, sc_iterator3_value(it3, 1));

                        search_arc_components(sc_iterator3_value(it3, 0), answer, sys_off);
                    }
                }
                sc_iterator3_free(it3);
            }
        }
        sc_iterator3_free(it2);

    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

sc_result agent_search_links_of_relation_connected_with_element(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer, param_elem, param_rel;
    sc_iterator3 *it1, *it2, *it3, *it4;
    sc_iterator5 *it5, *it_order;
    sc_type el_type;
    sc_bool sys_off = SC_TRUE;
    sc_bool param_elem_found = SC_FALSE, param_rel_found = SC_FALSE, found = SC_FALSE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_search_links_of_relation_connected_with_element, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get question arguments
    it5 = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                     question,
                                     sc_type_arc_pos_const_perm,
                                     0,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        if (SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 4), keynode_rrel_1))
        {
            param_elem = sc_iterator5_value(it5, 2);
            param_elem_found = SC_TRUE;
            continue;
        }
        if (SC_ADDR_IS_EQUAL(sc_iterator5_value(it5, 4), keynode_rrel_2))
        {
            param_rel = sc_iterator5_value(it5, 2);
            param_rel_found = SC_TRUE;
            continue;
        }
    }
    sc_iterator5_free(it5);
    if (param_elem_found == SC_FALSE || param_rel_found == SC_FALSE)
    {
        return SC_RESULT_ERROR;
    }

    appendIntoAnswer(answer, param_elem);

    if (IS_SYSTEM_ELEMENT(param_elem) || IS_SYSTEM_ELEMENT(param_rel))
        sys_off = SC_FALSE;

    search_translation(param_elem, answer, sys_off);

    if (SC_TRUE == sc_helper_check_arc(s_default_ctx, keynode_quasybinary_relation, param_rel, sc_type_arc_pos_const_perm))
    {
        // Search subclasses in quasybinary relation
        // Iterate input arcs of quasybinary relation
        it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                         sc_type_const,
                                         sc_type_const | sc_type_arc_common,
                                         param_elem,
                                         sc_type_arc_pos_const_perm,
                                         param_rel);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))))
                continue;

            found = SC_TRUE;

            appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));

            search_translation(sc_iterator5_value(it5, 0), answer, sys_off);

            search_arc_components(sc_iterator5_value(it5, 0), answer, sys_off);

            // Iterate subclasses in quasybinary relation
            it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                         sc_iterator5_value(it5, 0),
                                         sc_type_arc_pos_const_perm,
                                         0);
            while (sc_iterator3_next(it1) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it1, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

                search_translation(sc_iterator3_value(it1, 2), answer, sys_off);

                search_arc_components(sc_iterator3_value(it1, 2), answer, sys_off);

                // iterate order relations between elements
                it_order = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                                      sc_iterator3_value(it1, 2),
                                                      sc_type_arc_common | sc_type_const,
                                                      sc_type_node | sc_type_const,
                                                      sc_type_arc_pos_const_perm,
                                                      sc_type_node | sc_type_const);
                while (sc_iterator5_next(it_order) == SC_TRUE)
                {
                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 1)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 2))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 3)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 4))))
                        continue;

                    if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                        continue;
                    if (SC_FALSE == sc_helper_check_arc(s_default_ctx, sc_iterator5_value(it5, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
                        continue;

                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 1));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 2));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 3));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 4));
                }
                sc_iterator5_free(it_order);

                // iterate roles of element in link
                it2 = sc_iterator3_a_a_f_new(s_default_ctx,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it1, 1));
                while (sc_iterator3_next(it2) == SC_TRUE)
                {
                    sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it2, 0), &el_type);
                    if (!(el_type & sc_type_node_role))
                        continue;

                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 0))
                                               || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))))
                        continue;

                    appendIntoAnswer(answer, sc_iterator3_value(it2, 0));
                    appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                }
                sc_iterator3_free(it2);
            }
            sc_iterator3_free(it1);
        }
        sc_iterator5_free(it5);

        // Iterate input arcs of quasybinary relation
        it1 = sc_iterator3_a_a_f_new(s_default_ctx,
                                     sc_type_const | sc_type_node,
                                     sc_type_arc_pos_const_perm,
                                     param_elem);
        while (sc_iterator3_next(it1) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 0))
                                       || IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 1))))
                continue;

            // search all parents in quasybinary relation
            it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                             sc_iterator3_value(it1, 0),
                                             sc_type_arc_common | sc_type_const,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             param_rel);
            if (sc_iterator5_next(it5) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 2))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))))
                    continue;

                found = SC_TRUE;

                appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 2));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 3));

                search_translation(sc_iterator5_value(it5, 2), answer, sys_off);
                search_arc_components(sc_iterator5_value(it5, 2), answer, sys_off);

                appendIntoAnswer(answer, sc_iterator3_value(it1, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it1, 1));

                search_arc_components(sc_iterator3_value(it1, 0), answer, sys_off);
            }
            sc_iterator5_free(it5);
        }
        sc_iterator3_free(it1);
    }
    else
    {
        // Iterate output arcs of given relation
        it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                         param_elem,
                                         sc_type_const,
                                         sc_type_const,
                                         sc_type_arc_pos_const_perm,
                                         param_rel);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 2))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))))
                continue;

            found = SC_TRUE;

            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 2));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));

            search_translation(sc_iterator5_value(it5, 2), answer, sys_off);
            search_arc_components(sc_iterator5_value(it5, 2), answer, sys_off);
        }
        sc_iterator5_free(it5);

        // Iterate input arcs of given relation
        it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                         sc_type_const,
                                         sc_type_const,
                                         param_elem,
                                         sc_type_arc_pos_const_perm,
                                         param_rel);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))))
                continue;

            found = SC_TRUE;

            appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));

            search_translation(sc_iterator5_value(it5, 0), answer, sys_off);
            search_arc_components(sc_iterator5_value(it5, 0), answer, sys_off);
        }
        sc_iterator5_free(it5);

        // Iterate input arcs for input element
        it1 = sc_iterator3_a_a_f_new(s_default_ctx,
                                     sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     param_elem);
        while (sc_iterator3_next(it1) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 0))
                                       || IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 1))))
                continue;

            // Iterate input arcs for input element
            it2 = sc_iterator3_f_a_f_new(s_default_ctx,
                                         param_rel,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it1, 0));
            if (sc_iterator3_next(it2) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))))
                    continue;

                found = SC_TRUE;

                appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it1, 0));

                // Iterate elements of found link of given relation
                it3 = sc_iterator3_f_a_a_new(s_default_ctx,
                                             sc_iterator3_value(it1, 0),
                                             sc_type_arc_pos_const_perm,
                                             sc_type_const);
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))
                                               || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 2))))
                        continue;

                    appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
                    appendIntoAnswer(answer, sc_iterator3_value(it3, 2));

                    search_translation(sc_iterator3_value(it3, 2), answer, sys_off);
                    search_arc_components(sc_iterator3_value(it3, 2), answer, sys_off);

                    // Iterate role relations
                    it4 = sc_iterator3_a_a_f_new(s_default_ctx,
                                                 sc_type_const | sc_type_node,
                                                 sc_type_arc_pos_const_perm,
                                                 sc_iterator3_value(it3, 1));
                    while (sc_iterator3_next(it4) == SC_TRUE)
                    {
                        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 0))
                                                   || IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 1))))
                            continue;

                        appendIntoAnswer(answer, sc_iterator3_value(it4, 0));
                        appendIntoAnswer(answer, sc_iterator3_value(it4, 1));

                        search_arc_components(sc_iterator3_value(it4, 0), answer, sys_off);
                    }
                    sc_iterator3_free(it4);
                }
                sc_iterator3_free(it3);
            }
            sc_iterator3_free(it2);
        }
        sc_iterator3_free(it1);
    }

    if (found == SC_TRUE)
    {
        appendIntoAnswer(answer, param_rel);
    }

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
