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

#include "sc_search_basic.h"
#include "sc_helper.h"
#include "sc_event.h"
#include "sc_search_basic_operations.h"
#include <stdio.h>

sc_addr class_question_all_output_const_pos_arc;
sc_addr class_question_all_input_const_pos_arc;
sc_addr class_question_all_output_const_pos_arc_with_rel;
sc_addr class_question_all_input_const_pos_arc_with_rel;
sc_addr class_question_full_semantic_neighbourhood;
sc_addr question_initiated;
sc_addr nrel_answer;
sc_addr question_finished;

const char class_question_all_output_const_pos_arc_str[] = "class_question_all_output_const_pos_arc";
const char class_question_all_input_const_pos_arc_str[] = "class_question_all_input_const_pos_arc";
const char class_question_all_output_const_pos_arc_with_rel_str[] = "class_question_all_output_const_pos_arc_with_rel";
const char class_question_all_input_const_pos_arc_with_rel_str[] = "class_question_all_input_const_pos_arc_with_rel";
const char class_question_full_semantic_neighbourhood_str[] = "class_question_full_semantic_neighbourhood";
const char question_initiated_str[] = "question_initiated";
const char nrel_answer_str[] = "question_nrel_answer";
const char question_finished_str[] = "question_finished";

sc_event* event_question_search_all_output_arcs;
sc_event* event_question_search_all_input_arcs;
sc_event* event_question_search_all_output_arcs_with_rel;
sc_event* event_question_search_all_input_arcs_with_rel;
sc_event* event_question_full_semantic_neighbourhood;

// --------------------- Module ------------------------

sc_result initialize() {
    //printf("sc-search init done...\n");

    if (sc_helper_resolve_system_identifier(class_question_all_output_const_pos_arc_str, &class_question_all_output_const_pos_arc) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(class_question_all_input_const_pos_arc_str, &class_question_all_input_const_pos_arc) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(class_question_all_output_const_pos_arc_with_rel_str, &class_question_all_output_const_pos_arc_with_rel) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(class_question_all_input_const_pos_arc_with_rel_str, &class_question_all_input_const_pos_arc_with_rel) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(nrel_answer_str, &nrel_answer) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(class_question_full_semantic_neighbourhood_str, &class_question_full_semantic_neighbourhood) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(question_initiated_str, &question_initiated) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(question_finished_str, &question_finished) == SC_FALSE)
        return SC_RESULT_ERROR;

    event_question_search_all_output_arcs = sc_event_new(question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_output_arc, 0);
    if (event_question_search_all_output_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_search_all_input_arcs = sc_event_new(question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_input_arc, 0);
    if (event_question_search_all_input_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_search_all_output_arcs_with_rel = sc_event_new(question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_output_arc_with_rel, 0);
    if (event_question_search_all_input_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_search_all_input_arcs_with_rel = sc_event_new(question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_input_arc_with_rel, 0);
    if (event_question_search_all_input_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_full_semantic_neighbourhood = sc_event_new(question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_full_semantic_neighbourhood, 0);
    if (event_question_full_semantic_neighbourhood == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result shutdown() {
    sc_event_destroy(event_question_search_all_output_arcs);
    sc_event_destroy(event_question_search_all_input_arcs);
    sc_event_destroy(event_question_search_all_output_arcs_with_rel);
    sc_event_destroy(event_question_search_all_input_arcs_with_rel);
    sc_event_destroy(event_question_full_semantic_neighbourhood);

    return SC_RESULT_OK;
}
