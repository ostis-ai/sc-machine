/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */
#include "search_semantic_neighborhood.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>
#include <stdio.h>

void search_translation(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator5 *it5;
    sc_iterator3 *it3, *it4;
    sc_bool found = SC_FALSE;

    // iterate translations of sc-element
    it5 = sc_iterator5_a_a_f_a_f_new(sc_type_node | sc_type_const,
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
        it3 = sc_iterator3_f_a_a_new(sc_iterator5_value(it5, 0),
                                     sc_type_arc_pos_const_perm,
                                     0);
        while (sc_iterator3_next(it3) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 2))))
                continue;

            // iterate input arcs for link
            it4 = sc_iterator3_a_a_f_new(sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it3, 2));
            while (sc_iterator3_next(it4) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 0))))
                    continue;
                if (sc_helper_check_arc(keynode_languages, sc_iterator3_value(it4, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                {
                    appendIntoAnswer(answer, sc_iterator3_value(it4, 0));
                    appendIntoAnswer(answer, sc_iterator3_value(it4, 1));
                }
            }
            sc_iterator3_free(it4);

            // iterate input arcs for arc
            it4 = sc_iterator3_a_a_f_new(sc_type_node,
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

void search_nonbinary_relation(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it1, *it2, *it3;
    sc_type el_type;

    // iterate input arcs for elem
    it1 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                 sc_type_arc_pos_const_perm,
                                 elem);
    while (sc_iterator3_next(it1) == SC_TRUE)
    {
        // if elem is a link of non-binary relation
        if (SC_TRUE == sc_helper_check_arc(keynode_nonbinary_relation, sc_iterator3_value(it1, 0), sc_type_arc_pos_const_perm))
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 1))))
                continue;

            // iterate other elements of link
            it2 = sc_iterator3_f_a_a_new(elem,
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
            while (sc_iterator3_next(it2) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it2, 2));

                // iterate attributes of link
                it3 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it2, 1));
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
                        continue;

                    sc_memory_get_element_type(sc_iterator3_value(it3, 0), &el_type);
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
    sc_iterator3 *it1;

    if (sc_helper_check_arc(keynode_typical_sc_neighborhood, elem, sc_type_arc_pos_const_perm) == SC_TRUE)
    {
        // iterate input arcs for elem
        it1 = sc_iterator3_f_a_a_new(elem,
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
    }
}

sc_result agent_search_full_semantic_neighborhood(sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2, *it3, *it4, *it6;
    sc_iterator5 *it5, *it_order, *it_order2;
    sc_type el_type;
    sc_bool sys_off = SC_TRUE;
    sc_bool key_order_found = SC_FALSE;

    printf("test\n");

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_full_semantic_neighborhood, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get question argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate input arcs
        it2 = sc_iterator3_a_a_f_new(0,
                                     0,
                                     sc_iterator3_value(it1, 2));
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it2, 0));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));

            // iterate input arcs into found arc, to find relations
            it3 = sc_iterator3_a_a_f_new(sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it3) == SC_TRUE)
            {
                sc_memory_get_element_type(sc_iterator3_value(it3, 0), &el_type);
                if (!(el_type & (sc_type_node_norole | sc_type_node_role)))
                    continue;

                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));

                // search typical sc-neighborhood if necessary
                if (SC_ADDR_IS_EQUAL(keynode_rrel_key_sc_element, sc_iterator3_value(it3, 0)))
                {
                    search_typical_sc_neighborhood(sc_iterator3_value(it2, 0), answer, sys_off);
                }

                // check if it's a quasy binary relation
                if (sc_helper_check_arc(keynode_quasybinary_relation, sc_iterator3_value(it3, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                {
                    // iterate elements of relation
                    it4 = sc_iterator3_f_a_a_new(sc_iterator3_value(it2, 0), sc_type_arc_pos_const_perm, 0);
                    while (sc_iterator3_next(it4) == SC_TRUE)
                    {
                        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it4, 2))))
                            continue;

                        appendIntoAnswer(answer, sc_iterator3_value(it4, 1));
                        appendIntoAnswer(answer, sc_iterator3_value(it4, 2));

                        // iterate order relations between elements
                        it_order = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it4, 2),
                                                              sc_type_arc_common | sc_type_const,
                                                              sc_type_node | sc_type_const,
                                                              sc_type_arc_pos_const_perm,
                                                              sc_type_node | sc_type_const);
                        while (sc_iterator5_next(it_order) == SC_TRUE)
                        {
                            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 1)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 2))
                                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 3)) || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 4))))
                                continue;

                            if (SC_FALSE == sc_helper_check_arc(keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                                continue;
                            if (SC_FALSE == sc_helper_check_arc(sc_iterator3_value(it2, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
                                continue;

                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 1));
                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 2));
                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 3));
                            appendIntoAnswer(answer, sc_iterator5_value(it_order, 4));

                        }
                        sc_iterator5_free(it_order);

                        // iterate roles of element in link
                        it6 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                                     sc_type_arc_pos_const_perm,
                                                     sc_iterator3_value(it4, 1));
                        while (sc_iterator3_next(it6) == SC_TRUE)
                        {
                            sc_memory_get_element_type(sc_iterator3_value(it6, 0), &el_type);
                            if (!(el_type & sc_type_node_role))
                                continue;

                            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it6, 0))
                                                       || IS_SYSTEM_ELEMENT(sc_iterator3_value(it6, 1))))
                                continue;

                            appendIntoAnswer(answer, sc_iterator3_value(it6, 0));
                            appendIntoAnswer(answer, sc_iterator3_value(it6, 1));
                        }
                        sc_iterator3_free(it6);

                    }
                    sc_iterator3_free(it4);
                }
            }
            sc_iterator3_free(it3);

            // search all parents in quasybinary relation
            it5 = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it2, 0),
                                             sc_type_arc_common | sc_type_const,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_type_node | sc_type_const);
            while (sc_iterator5_next(it5) == SC_TRUE)
            {
                // check if it's a quasy binary relation
                if (sc_helper_check_arc(keynode_quasybinary_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm) == SC_TRUE)
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
                }
            }
            sc_iterator5_free(it5);

            // search translation for element
            search_translation(sc_iterator3_value(it2, 0), answer, sys_off);

            // search non-binary relation link
            search_nonbinary_relation(sc_iterator3_value(it2, 0), answer, sys_off);
        }
        sc_iterator3_free(it2);

        // iterate output arcs
        it2 = sc_iterator3_f_a_a_new(sc_iterator3_value(it1, 2),
                                     0,
                                     0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 2));

            // iterate input arcs into found arc, to find relations
            it3 = sc_iterator3_a_a_f_new(sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it3) == SC_TRUE)
            {
                sc_memory_get_element_type(sc_iterator3_value(it3, 0), &el_type);
                if (!(el_type & (sc_type_node_norole | sc_type_node_role)))
                    continue;

                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));

                // search of key sc-elements order
                if (SC_ADDR_IS_EQUAL(sc_iterator3_value(it3, 0), keynode_rrel_key_sc_element))
                {
                    it_order2 = sc_iterator5_f_a_a_a_f_new(sc_iterator3_value(it2, 1),
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

                // search translation for element
                search_translation(sc_iterator3_value(it3, 0), answer, sys_off);
            }
            sc_iterator3_free(it3);

            // search translation for element
            search_translation(sc_iterator3_value(it2, 2), answer, sys_off);

            // check if element is an sc-link
            if (SC_RESULT_OK == sc_memory_get_element_type(sc_iterator3_value(it2, 2), &el_type) &&
                (el_type | sc_type_link))
            {
                // iterate input arcs for link
                it3 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it2, 2));
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    if (sc_helper_check_arc(keynode_languages, sc_iterator3_value(it3, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                    {
                        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))))
                            continue;

                        appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                        appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
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
