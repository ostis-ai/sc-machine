
/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_keynodes.h"

#include "sc_helper.h"
#include "sc_memory_headers.h"
#include "search.h"

#include "../common/sc_keynodes.h"

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
sc_addr keynode_nrel_strict_inclusion;

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
const char keynode_typical_sc_neighborhood_str[] = "typical_semantic_neighbourhood";

const char keynode_nrel_identification_str[] = "nrel_identification";
const char keynode_nrel_translation_str[] = "nrel_sc_text_translation";
const char keynode_nrel_main_idtf_str[] = "nrel_main_idtf";
const char keynode_nrel_inclusion_str[] = "nrel_inclusion";
const char keynode_nrel_strict_inclusion_str[] = "nrel_strict_inclusion";
const char keynode_rrel_key_sc_element_str[] = "rrel_key_sc_element";
const char keynode_nrel_key_sc_element_base_order_str[] = "nrel_key_sc_element_base_order";
const char keynode_system_element_str[] = "system_element";
const char keynode_sc_neighborhood_str[] = "semantic_neighbourhood";
const char keynode_languages_str[] = "languages";

sc_result search_keynodes_initialize()
{

    RESOLVE_KEYNODE(s_default_ctx, keynode_question_all_output_const_pos_arc);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_all_input_const_pos_arc);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_all_output_const_pos_arc_with_rel);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_all_input_const_pos_arc_with_rel);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_full_semantic_neighborhood);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_decomposition);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_answer);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_all_identifiers);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_all_identified_elements);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_search_all_subclasses_in_quasybinary_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_search_all_superclasses_in_quasybinary_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_search_links_of_relation_connected_with_element);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_search_full_pattern);

    RESOLVE_KEYNODE(s_default_ctx, keynode_question_initiated);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_finished);
    RESOLVE_KEYNODE(s_default_ctx, keynode_quasybinary_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_decomposition_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_taxonomy_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_identification_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_order_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nonbinary_relation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_typical_sc_neighborhood);
    RESOLVE_KEYNODE(s_default_ctx, keynode_sc_neighborhood);

    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_1);
    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_2);

    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_identification);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_translation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_main_idtf);
    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_key_sc_element);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_key_sc_element_base_order);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_inclusion);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_strict_inclusion);
    RESOLVE_KEYNODE(s_default_ctx, keynode_system_element);
    RESOLVE_KEYNODE(s_default_ctx, keynode_languages);

    return SC_RESULT_OK;
}
