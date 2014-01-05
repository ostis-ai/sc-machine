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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */
#include "search_keynodes.h"

#include "sc_helper.h"
#include "sc_memory_headers.h"


sc_addr keynode_question_all_output_const_pos_arc;
sc_addr keynode_question_all_input_const_pos_arc;
sc_addr keynode_question_all_output_const_pos_arc_with_rel;
sc_addr keynode_question_all_input_const_pos_arc_with_rel;
sc_addr keynode_question_full_semantic_neighborhood;
sc_addr keynode_question_decomposition;
sc_addr keynode_question_all_identifiers;
sc_addr keynode_question_search_all_superclasses_in_quasybinary_relation;
sc_addr keynode_question_search_all_subclasses_in_quasybinary_relation;

sc_addr keynode_question_initiated;
sc_addr keynode_nrel_answer;
sc_addr keynode_question_finished;
sc_addr keynode_quasybinary_relation;
sc_addr keynode_decomposition_relation;
sc_addr keynode_identification_relation;
sc_addr keynode_order_relation;

sc_addr keynode_nrel_identification;
sc_addr keynode_set_nrel_inclusion;
sc_addr keynode_system_element;

const char keynode_question_all_output_const_pos_arc_str[] = "question_search_all_output_const_pos_arcs";
const char keynode_question_all_input_const_pos_arc_str[] = "question_search_all_input_const_pos_arcs";
const char keynode_question_all_output_const_pos_arc_with_rel_str[] = "question_search_all_output_const_pos_arcs_with_rel";
const char keynode_question_all_input_const_pos_arc_with_rel_str[] = "question_search_all_input_const_pos_arcs_with_rel";
const char keynode_question_full_semantic_neighborhood_str[] = "question_search_full_semantic_neighborhood";
const char keynode_question_decomposition_str[] = "question_search_decomposition";
const char keynode_question_all_identifiers_str[] = "question_search_all_identifiers";

const char keynode_question_search_all_subclasses_in_quasybinary_relation_str[] = "question_search_all_subclasses_in_quasybinary_relation";
const char keynode_question_search_all_superclasses_in_quasybinary_relation_str[] = "question_search_all_superclasses_in_quasybinary_relation";


const char keynode_question_initiated_str[] = "question_initiated";
const char keynode_nrel_answer_str[] = "nrel_answer";
const char keynode_question_finished_str[] = "question_finished";
const char keynode_quasybinary_relation_str[] = "quasybinary_relation";
const char keynode_decomposition_relation_str[] = "decomposition_relation";
const char keynode_identification_relation_str[] = "identification_relation";
const char keynode_order_relation_str[] = "order_relation";

const char keynode_nrel_identification_str[] = "nrel_identification";
const char keynode_set_nrel_inclusion_str[] = "nrel_inclusion";
const char keynode_system_element_str[] = "system_element";

#define resolve_keynode(keynode) if (sc_helper_resolve_system_identifier(keynode##_str, &keynode) == SC_FALSE) return SC_RESULT_ERROR;

sc_result search_keynodes_initialize()
{

    resolve_keynode(keynode_question_all_output_const_pos_arc);
    resolve_keynode(keynode_question_all_input_const_pos_arc);
    resolve_keynode(keynode_question_all_output_const_pos_arc_with_rel);
    resolve_keynode(keynode_question_all_input_const_pos_arc_with_rel);
    resolve_keynode(keynode_question_full_semantic_neighborhood);
    resolve_keynode(keynode_question_decomposition);
    resolve_keynode(keynode_nrel_answer);
    resolve_keynode(keynode_question_all_identifiers);
    resolve_keynode(keynode_question_search_all_subclasses_in_quasybinary_relation);
    resolve_keynode(keynode_question_search_all_superclasses_in_quasybinary_relation);

    resolve_keynode(keynode_question_initiated);
    resolve_keynode(keynode_question_finished);
    resolve_keynode(keynode_quasybinary_relation);
    resolve_keynode(keynode_decomposition_relation);
    resolve_keynode(keynode_identification_relation);
    resolve_keynode(keynode_order_relation);

    resolve_keynode(keynode_nrel_identification);
    resolve_keynode(keynode_set_nrel_inclusion);
    resolve_keynode(keynode_system_element);

    return SC_RESULT_OK;
}
