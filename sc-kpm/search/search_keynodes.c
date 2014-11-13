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
#include "search_keynodes.h"

#include "sc_helper.h"
#include "sc_memory_headers.h"
#include "search.h"

#include <glib.h>


sc_addr keynode_question_all_output_const_pos_arc;
sc_addr keynode_question_all_input_const_pos_arc;
sc_addr keynode_question_all_output_const_pos_arc_with_rel;
sc_addr keynode_question_all_input_const_pos_arc_with_rel;
sc_addr keynode_question_full_semantic_neighborhood;
sc_addr keynode_question_decomposition;
sc_addr keynode_question_all_identifiers;
sc_addr keynode_question_all_identified_elements;
sc_addr keynode_question_search_all_superclasses_in_quasybinary_relation;
sc_addr keynode_question_search_all_subclasses_in_quasybinary_relation;
sc_addr keynode_question_search_links_of_relation_connected_with_element;
sc_addr keynode_question_search_full_pattern;

sc_addr keynode_question;
sc_addr keynode_question_initiated;
sc_addr keynode_nrel_answer;
sc_addr keynode_question_finished;
sc_addr keynode_quasybinary_relation;
sc_addr keynode_decomposition_relation;
sc_addr keynode_taxonomy_relation;
sc_addr keynode_identification_relation;
sc_addr keynode_order_relation;
sc_addr keynode_nonbinary_relation;
sc_addr keynode_typical_sc_neighborhood;
sc_addr keynode_sc_neighborhood;

sc_addr keynode_nrel_identification;
sc_addr keynode_nrel_translation;
sc_addr keynode_nrel_main_idtf;
sc_addr keynode_rrel_key_sc_element;
sc_addr keynode_nrel_key_sc_element_base_order;
sc_addr keynode_nrel_inclusion;
sc_addr keynode_system_element;
sc_addr keynode_languages;

sc_addr keynode_rrel_1;
sc_addr keynode_rrel_2;

const char keynode_question_all_output_const_pos_arc_str[] = "question_search_all_output_const_pos_arcs";
const char keynode_question_all_input_const_pos_arc_str[] = "question_search_all_input_const_pos_arcs";
const char keynode_question_all_output_const_pos_arc_with_rel_str[] = "question_search_all_output_const_pos_arcs_with_rel";
const char keynode_question_all_input_const_pos_arc_with_rel_str[] = "question_search_all_input_const_pos_arcs_with_rel";
const char keynode_question_full_semantic_neighborhood_str[] = "question_search_full_semantic_neighborhood";
const char keynode_question_decomposition_str[] = "question_search_decomposition";
const char keynode_question_all_identifiers_str[] = "question_search_all_identifiers";
const char keynode_question_all_identified_elements_str[] = "question_search_all_identified_elements";
const char keynode_question_search_full_pattern_str[] = "question_full_pattern_search";

const char keynode_question_search_all_subclasses_in_quasybinary_relation_str[] = "question_search_all_subclasses_in_quasybinary_relation";
const char keynode_question_search_all_superclasses_in_quasybinary_relation_str[] = "question_search_all_superclasses_in_quasybinary_relation";
const char keynode_question_search_links_of_relation_connected_with_element_str[] = "question_search_links_of_relation_connected_with_element";

const char keynode_question_initiated_str[] = "question_initiated";
const char keynode_question_str[] = "question";
const char keynode_rrel_1_str[] = "rrel_1";
const char keynode_rrel_2_str[] = "rrel_2";
const char keynode_nrel_answer_str[] = "nrel_answer";
const char keynode_question_finished_str[] = "question_finished";
const char keynode_quasybinary_relation_str[] = "quasybinary_relation";
const char keynode_decomposition_relation_str[] = "decomposition_relation";
const char keynode_taxonomy_relation_str[] = "taxonomy_relation";
const char keynode_identification_relation_str[] = "identification_relation";
const char keynode_order_relation_str[] = "order_relation";
const char keynode_nonbinary_relation_str[] = "nonbinary_relation";
const char keynode_typical_sc_neighborhood_str[] = "typical_sc_neighborhood";

const char keynode_nrel_identification_str[] = "nrel_identification";
const char keynode_nrel_translation_str[] = "nrel_sc_text_translation";
const char keynode_nrel_main_idtf_str[] = "nrel_main_idtf";
const char keynode_nrel_inclusion_str[] = "nrel_inclusion";
const char keynode_rrel_key_sc_element_str[] = "rrel_key_sc_element";
const char keynode_nrel_key_sc_element_base_order_str[] = "nrel_key_sc_element_base_order";
const char keynode_system_element_str[] = "system_element";
const char keynode_sc_neighborhood_str[] = "sc_neighborhood";
const char keynode_languages_str[] = "languages";

#define resolve_keynode(keynode) \
    if (sc_helper_resolve_system_identifier(s_default_ctx, keynode##_str, &keynode) == SC_FALSE) \
    {\
        g_warning("Can't find element with system identifier: %s", keynode##_str); \
        keynode = sc_memory_node_new(s_default_ctx, 0); \
        if (sc_helper_set_system_identifier(s_default_ctx, keynode, keynode##_str, strlen(keynode##_str)) != SC_RESULT_OK) \
            return SC_RESULT_ERROR; \
        g_message("Created element with system identifier: %s", keynode##_str); \
    }


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
    resolve_keynode(keynode_question_all_identified_elements);
    resolve_keynode(keynode_question_search_all_subclasses_in_quasybinary_relation);
    resolve_keynode(keynode_question_search_all_superclasses_in_quasybinary_relation);
    resolve_keynode(keynode_question_search_links_of_relation_connected_with_element);
    resolve_keynode(keynode_question_search_full_pattern);

    resolve_keynode(keynode_question_initiated);
    resolve_keynode(keynode_question);
    resolve_keynode(keynode_question_finished);
    resolve_keynode(keynode_quasybinary_relation);
    resolve_keynode(keynode_decomposition_relation);
    resolve_keynode(keynode_taxonomy_relation);
    resolve_keynode(keynode_identification_relation);
    resolve_keynode(keynode_order_relation);
    resolve_keynode(keynode_nonbinary_relation);
    resolve_keynode(keynode_typical_sc_neighborhood);
    resolve_keynode(keynode_sc_neighborhood);

    resolve_keynode(keynode_rrel_1);
    resolve_keynode(keynode_rrel_2);

    resolve_keynode(keynode_nrel_identification);
    resolve_keynode(keynode_nrel_translation);
    resolve_keynode(keynode_nrel_main_idtf);
    resolve_keynode(keynode_rrel_key_sc_element);
    resolve_keynode(keynode_nrel_key_sc_element_base_order);
    resolve_keynode(keynode_nrel_inclusion);
    resolve_keynode(keynode_system_element);
    resolve_keynode(keynode_languages);

    return SC_RESULT_OK;
}
