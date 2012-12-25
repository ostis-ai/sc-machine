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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/
#include "questions.h"

#include "sc_helper.h"

const char keynode_question_initiated_str[] = "question_initiated";
const char keynode_question_nrel_answer_str[] = "question_nrel_answer";

sc_addr keynode_question_initiated;
sc_addr ui_keynode_question_nrel_answer;


// --------------------- Events -----------------------
sc_event *event_question_search_all_output_arcs = 0;

sc_result question_search_all_output_arcs(sc_event *event, sc_addr arg)
{
    sc_addr question_node;
    sc_addr question_argument;
    sc_addr answer_node;
    sc_addr value;
    sc_bool arg_exist = SC_FALSE;
    sc_iterator3 *it = 0;
    sc_uint32 i = 0;

    if (sc_memory_get_arc_end(arg, &question_node) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // get question argument
    it = sc_iterator3_f_a_a_new(question_node, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it))
    {
        question_argument = sc_iterator3_value(it, 2);
        arg_exist = SC_TRUE;
    }

    sc_iterator3_free(it);

    // make answer
    answer_node = sc_memory_node_new(sc_type_node | sc_type_const);

    if (arg_exist == SC_TRUE)
    {
        // iterate all output arcs and append them into result
        it = sc_iterator3_f_a_a_new(question_argument, 0, 0);
        while (sc_iterator3_next(it))
        {
            for (i = 0; i < 3; ++i)
            {
                value = sc_iterator3_value(it, i);
                sc_memory_arc_new(sc_type_arc_pos_const_perm, answer_node, value);
            }
        }
        sc_iterator3_free(it);
    }

    // connect question with answer
    value = sc_memory_arc_new(sc_type_arc_common | sc_type_const, question_node, answer_node);
    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_question_nrel_answer, value);

    return SC_RESULT_OK;
}


// --------------------- Module ------------------------
sc_result initialize()
{

    if (sc_helper_resolve_system_identifier(keynode_question_initiated_str, &keynode_question_initiated) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(keynode_question_nrel_answer_str, &ui_keynode_question_nrel_answer) == SC_FALSE)
        return SC_RESULT_ERROR;


    event_question_search_all_output_arcs = sc_event_new(keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, question_search_all_output_arcs, 0);
    if (event_question_search_all_output_arcs == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result shutdown()
{
    sc_event_destroy(event_question_search_all_output_arcs);

    return SC_RESULT_OK;
}
