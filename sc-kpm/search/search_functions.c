/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#include "search_functions.h"
#include "sc_helper.h"
#include "sc_memory_headers.h"

#include "search_keynodes.h"

sc_bool search_all_const_pos_output_arc(sc_addr node, sc_addr answer)
{
    sc_iterator3 *it = nullptr;
    sc_addr addr2, addr3;

    it = sc_iterator3_f_a_a_new(node, sc_type_arc_pos_const_perm, 0);

    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        addr2 = sc_iterator3_value(it, 1);
        addr3 = sc_iterator3_value(it, 2);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr2);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr3);
    }
    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, node);

    sc_iterator3_free(it);
    return SC_TRUE;
}

sc_bool search_all_const_pos_input_arc(sc_addr node, sc_addr answer)
{
    sc_iterator3 *it = nullptr;
    sc_addr addr2, addr3;

    it = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, node);

    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        addr2 = sc_iterator3_value(it, 0);
        addr3 = sc_iterator3_value(it, 1);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr2);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr3);
    }
    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, node);

    sc_iterator3_free(it);
    return SC_TRUE;
}

sc_bool search_all_const_pos_output_arc_with_attr(sc_addr node, sc_addr answer)
{

    sc_iterator3 *it = nullptr;
    sc_addr addr2, addr3;

    it = sc_iterator3_f_a_a_new(node, sc_type_arc_pos_const_perm, 0);

    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        addr2 = sc_iterator3_value(it, 1);
        addr3 = sc_iterator3_value(it, 2);
        if (search_all_const_pos_input_arc(addr2, answer) == SC_FALSE)
            continue;

        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr3);
    }
    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, node);

    sc_iterator3_free(it);
    return SC_TRUE;
}

sc_bool search_all_const_pos_input_arc_with_rel(sc_addr node, sc_addr answer)
{

    sc_iterator3 *it = nullptr;
    sc_addr addr2, addr3;

    it = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, node);

    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        addr2 = sc_iterator3_value(it, 0);
        addr3 = sc_iterator3_value(it, 1);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr2);
        if (search_all_const_pos_input_arc(addr3, answer) == SC_FALSE)
            continue;
    }
    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, node);

    sc_iterator3_free(it);
    return SC_TRUE;
}

sc_bool search_all_const_pos_output_arc_with_rel(sc_addr node, sc_addr answer)
{

    sc_iterator3 *it = nullptr;
    sc_addr arc_end, arc;

    it = sc_iterator3_f_a_a_new(node, sc_type_arc_pos_const_perm, 0);

    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        arc_end = sc_iterator3_value(it, 2);
        arc = sc_iterator3_value(it, 1);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, arc_end);
        if (search_all_const_pos_input_arc(arc, answer) == SC_FALSE)
            continue;
    }
    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, node);

    sc_iterator3_free(it);
    return SC_TRUE;
}

sc_bool search_full_semantic_neighbourhood(sc_addr node, sc_addr answer)
{

    sc_iterator3 *it = nullptr;
    sc_iterator3 *it1 = nullptr;
    sc_iterator3 *it2 = nullptr;
    sc_addr addr2, addr3, addr4, addr5, quas;
    char flag = 0;

    //Input arcs iterator
    it = sc_iterator3_a_a_f_new(0, 0, node);
    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        addr3 = sc_iterator3_value(it, 0);
        addr2 = sc_iterator3_value(it, 1);
        if (SC_ADDR_IS_EQUAL(answer, addr2))
            continue;

        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr2);

        //Attribute arcs iterator
        it1 = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, addr2);
        if (it == nullptr)
            continue;

        while (sc_iterator3_next(it1))
        {
            addr4 = sc_iterator3_value(it1, 0);
            addr5 = sc_iterator3_value(it1, 1);
            if (SC_ADDR_IS_EQUAL(answer, addr4))
                continue;

            sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr4);
            sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr5);

            it2 = sc_iterator3_f_a_f_new(quas, sc_type_arc_pos_const_perm, addr4);
            if (it2 == nullptr)
                continue;

            if (sc_iterator3_next(it2))
                flag = 1;
        }
        sc_iterator3_free(it1);

        if (SC_ADDR_IS_EQUAL(answer, addr3))
            continue;

        if (flag)
            search_all_const_pos_output_arc(addr3, answer);
        else
            sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr3);
    }
    sc_iterator3_free(it);

    //Output arcs iterator
    it = sc_iterator3_f_a_a_new(node, 0, 0);
    if (it == nullptr)
        return SC_FALSE;

    while (sc_iterator3_next(it))
    {
        addr2 = sc_iterator3_value(it, 1);
        addr3 = sc_iterator3_value(it, 2);
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr2);

        //Attribute arcs iterator
        it2 = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, addr2);
        if (it == nullptr)
            continue;

        while (sc_iterator3_next(it2))
        {
            addr4 = sc_iterator3_value(it2, 0);
            addr5 = sc_iterator3_value(it2, 1);
            if (SC_ADDR_IS_EQUAL(answer, addr4))
                continue;

            sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr4);
            sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr5);

            it2 = sc_iterator3_f_a_f_new(quas, sc_type_arc_pos_const_perm, addr4);
            if (it2 == nullptr)
                continue;

            if (sc_iterator3_next(it2))
                flag = 1;
        }
        sc_iterator3_free(it2);

        if (SC_ADDR_IS_EQUAL(answer, addr3))
            continue;

        if (flag)
            search_all_const_pos_output_arc(addr3, answer);
        else
            sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr3);
    }
    sc_iterator3_free(it);

    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, node);

    return SC_TRUE;
}

sc_bool get_question_single_param(sc_addr question_class, sc_addr question_node, sc_addr *param)
{
    sc_iterator3 *it = sc_iterator3_f_a_f_new(question_class, sc_type_arc_pos_const_perm, question_node);
    if (!sc_iterator3_next(it))
        return SC_FALSE;
    sc_iterator3_free(it);

    it = sc_iterator3_f_a_a_new(question_node, sc_type_arc_pos_const_perm, 0);
    if (!sc_iterator3_next(it))
    {
        sc_iterator3_free(it);
        return SC_FALSE;
    }
    else
    {
        *param = it->results[2];
        sc_iterator3_free(it);
        return SC_TRUE;
    }
}
