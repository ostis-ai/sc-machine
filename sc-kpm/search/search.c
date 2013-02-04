/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2013 OSTIS

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

#include "search.h"
#include "sc_helper.h"
#include "sc_memory_headers.h"
#include "search_agents.h"
#include "search_keynodes.h"


sc_event* event_question_search_all_output_arcs;
sc_event* event_question_search_all_input_arcs;
sc_event* event_question_search_all_output_arcs_with_rel;
sc_event* event_question_search_all_input_arcs_with_rel;
sc_event* event_question_full_semantic_neighborhood;

// --------------------- Module ------------------------

sc_result initialize()
{
    if (search_keynodes_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    event_question_search_all_output_arcs = sc_event_new(search_keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_output_arc, 0);
    if (event_question_search_all_output_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_search_all_input_arcs = sc_event_new(search_keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_input_arc, 0);
    if (event_question_search_all_input_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_search_all_output_arcs_with_rel = sc_event_new(search_keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_output_arc_with_rel, 0);
    if (event_question_search_all_input_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_search_all_input_arcs_with_rel = sc_event_new(search_keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_all_const_pos_input_arc_with_rel, 0);
    if (event_question_search_all_input_arcs == nullptr)
        return SC_RESULT_ERROR;

    event_question_full_semantic_neighborhood = sc_event_new(search_keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, operation_search_full_semantic_neighborhood, 0);
    if (event_question_full_semantic_neighborhood == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result shutdown()
{
    sc_event_destroy(event_question_search_all_output_arcs);
    sc_event_destroy(event_question_search_all_input_arcs);
    sc_event_destroy(event_question_search_all_output_arcs_with_rel);
    sc_event_destroy(event_question_search_all_input_arcs_with_rel);
    sc_event_destroy(event_question_full_semantic_neighborhood);

    return SC_RESULT_OK;
}
