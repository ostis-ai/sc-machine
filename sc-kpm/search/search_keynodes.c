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
#include "search_keynodes.h"

#include "sc_helper.h"
#include "sc_memory_headers.h"


sc_addr search_keynode_question_all_output_const_pos_arc;
sc_addr search_keynode_question_all_input_const_pos_arc;
sc_addr search_keynode_question_all_output_const_pos_arc_with_rel;
sc_addr search_keynode_question_all_input_const_pos_arc_with_rel;
sc_addr search_keynode_question_full_semantic_neighborhood;
sc_addr search_keynode_question_initiated;
sc_addr search_keynode_nrel_answer;
sc_addr search_keynode_question_finished;
sc_addr search_keynode_quasybinary_relation;

const char search_keynode_question_all_output_const_pos_arc_str[] = "question_search_all_output_const_pos_arcs";
const char search_keynode_question_all_input_const_pos_arc_str[] = "question_search_all_input_const_pos_arcs";
const char search_keynode_question_all_output_const_pos_arc_with_rel_str[] = "question_search_all_output_const_pos_arcs_with_rel";
const char search_keynode_question_all_input_const_pos_arc_with_rel_str[] = "question_search_all_input_const_pos_arcs_with_rel";
const char search_keynode_question_full_semantic_neighborhood_str[] = "question_search_full_semantic_neighborhood";
const char search_keynode_question_initiated_str[] = "question_initiated";
const char search_keynode_nrel_answer_str[] = "question_nrel_answer";
const char search_keynode_question_finished_str[] = "question_finished";
const char search_keynode_quasybinary_relation_str[] = "quasybinary_relation";


sc_result search_keynodes_initialize()
{

    if (sc_helper_resolve_system_identifier(search_keynode_question_all_output_const_pos_arc_str, &search_keynode_question_all_output_const_pos_arc) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_question_all_input_const_pos_arc_str, &search_keynode_question_all_input_const_pos_arc) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_question_all_output_const_pos_arc_with_rel_str, &search_keynode_question_all_output_const_pos_arc_with_rel) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_question_all_input_const_pos_arc_with_rel_str, &search_keynode_question_all_input_const_pos_arc_with_rel) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_nrel_answer_str, &search_keynode_nrel_answer) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_question_full_semantic_neighborhood_str, &search_keynode_question_full_semantic_neighborhood) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_question_initiated_str, &search_keynode_question_initiated) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_question_finished_str, &search_keynode_question_finished) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (sc_helper_resolve_system_identifier(search_keynode_quasybinary_relation_str, &search_keynode_quasybinary_relation) == SC_FALSE)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}
