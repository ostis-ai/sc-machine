/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_keynodes.h"
#include "search.h"

#include <sc-core/sc_memory_headers.h>

#include <sc-common/sc_keynodes.h>

sc_addr keynode_action_all_output_const_pos_arc;
sc_addr keynode_action_all_input_const_pos_arc;
sc_addr keynode_action_all_output_const_pos_arc_with_rel;
sc_addr keynode_action_all_input_const_pos_arc_with_rel;
sc_addr keynode_action_full_semantic_neighborhood;
sc_addr keynode_action_decomposition;
sc_addr keynode_action_all_identifiers;
sc_addr keynode_action_all_identified_elements;
sc_addr keynode_action_search_all_superclasses_in_quasybinary_relation;
sc_addr keynode_action_search_all_subclasses_in_quasybinary_relation;
sc_addr keynode_action_search_links_of_relation_connected_with_element;
sc_addr keynode_action_search_full_pattern;

sc_addr keynode_action;
sc_addr keynode_action_initiated;
sc_addr keynode_nrel_result;
sc_addr keynode_action_finished;
sc_addr keynode_action_finished_successfully;
sc_addr keynode_action_finished_unsuccessfully;
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

sc_char const keynode_action_all_output_const_pos_arc_str[] = "action_search_all_output_const_pos_arcs";
sc_char const keynode_action_all_input_const_pos_arc_str[] = "action_search_all_input_const_pos_arcs";
sc_char const keynode_action_all_output_const_pos_arc_with_rel_str[] =
    "action_search_all_output_const_pos_arcs_with_rel";
sc_char const keynode_action_all_input_const_pos_arc_with_rel_str[] = "action_search_all_input_const_pos_arcs_with_rel";
sc_char const keynode_action_full_semantic_neighborhood_str[] = "action_search_full_semantic_neighborhood";
sc_char const keynode_action_decomposition_str[] = "action_search_decomposition";
sc_char const keynode_action_all_identifiers_str[] = "action_search_all_identifiers";
sc_char const keynode_action_all_identified_elements_str[] = "action_search_all_identified_elements";
sc_char const keynode_action_search_full_pattern_str[] = "action_full_pattern_search";

sc_char const keynode_action_search_all_subclasses_in_quasybinary_relation_str[] =
    "action_search_all_subclasses_in_quasybinary_relation";
sc_char const keynode_action_search_all_superclasses_in_quasybinary_relation_str[] =
    "action_search_all_superclasses_in_quasybinary_relation";
sc_char const keynode_action_search_links_of_relation_connected_with_element_str[] =
    "action_search_links_of_relation_connected_with_element";

sc_char const keynode_action_initiated_str[] = "action_initiated";
sc_char const keynode_action_str[] = "action";
sc_char const keynode_rrel_1_str[] = "rrel_1";
sc_char const keynode_rrel_2_str[] = "rrel_2";
sc_char const keynode_nrel_result_str[] = "nrel_result";
sc_char const keynode_action_finished_str[] = "action_finished";
sc_char const keynode_action_finished_successfully_str[] = "action_finished_successfully";
sc_char const keynode_action_finished_unsuccessfully_str[] = "action_finished_unsuccessfully";
sc_char const keynode_quasybinary_relation_str[] = "quasybinary_relation";
sc_char const keynode_decomposition_relation_str[] = "decomposition_relation";
sc_char const keynode_taxonomy_relation_str[] = "taxonomy_relation";
sc_char const keynode_identification_relation_str[] = "identification_relation";
sc_char const keynode_order_relation_str[] = "order_relation";
sc_char const keynode_nonbinary_relation_str[] = "nonbinary_relation";
sc_char const keynode_typical_sc_neighborhood_str[] = "typical_semantic_neighbourhood";

sc_char const keynode_nrel_identification_str[] = "nrel_identification";
sc_char const keynode_nrel_translation_str[] = "nrel_sc_text_translation";
sc_char const keynode_nrel_main_idtf_str[] = "nrel_main_idtf";
sc_char const keynode_nrel_inclusion_str[] = "nrel_inclusion";
sc_char const keynode_nrel_strict_inclusion_str[] = "nrel_strict_inclusion";
sc_char const keynode_rrel_key_sc_element_str[] = "rrel_key_sc_element";
sc_char const keynode_nrel_key_sc_element_base_order_str[] = "nrel_key_sc_element_base_order";
sc_char const keynode_system_element_str[] = "system_element";
sc_char const keynode_sc_neighborhood_str[] = "semantic_neighbourhood";
sc_char const keynode_languages_str[] = "languages";

sc_result search_keynodes_initialize(sc_memory_context * ctx, sc_addr const init_memory_generated_structure)
{
  RESOLVE_KEYNODE(ctx, keynode_action_all_output_const_pos_arc, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_all_input_const_pos_arc, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_all_output_const_pos_arc_with_rel, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_all_input_const_pos_arc_with_rel, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_full_semantic_neighborhood, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_decomposition, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nrel_result, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_all_identifiers, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_all_identified_elements, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_search_all_subclasses_in_quasybinary_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_search_all_superclasses_in_quasybinary_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_search_links_of_relation_connected_with_element, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_search_full_pattern, init_memory_generated_structure);

  RESOLVE_KEYNODE(ctx, keynode_action_initiated, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_finished, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_finished_successfully, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_action_finished_unsuccessfully, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_quasybinary_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_decomposition_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_taxonomy_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_identification_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_order_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nonbinary_relation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_typical_sc_neighborhood, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_sc_neighborhood, init_memory_generated_structure);

  RESOLVE_KEYNODE(ctx, keynode_rrel_1, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_rrel_2, init_memory_generated_structure);

  RESOLVE_KEYNODE(ctx, keynode_nrel_identification, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nrel_translation, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nrel_main_idtf, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_rrel_key_sc_element, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nrel_key_sc_element_base_order, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nrel_inclusion, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_nrel_strict_inclusion, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_system_element, init_memory_generated_structure);
  RESOLVE_KEYNODE(ctx, keynode_languages, init_memory_generated_structure);

  return SC_RESULT_OK;
}
