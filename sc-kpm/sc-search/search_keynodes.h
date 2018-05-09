/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_keynodes_h_
#define _search_keynodes_h_

#include "sc_memory.h"

extern sc_addr keynode_question_all_output_const_pos_arc;
extern sc_addr keynode_question_all_input_const_pos_arc;
extern sc_addr keynode_question_all_output_const_pos_arc_with_rel;
extern sc_addr keynode_question_all_input_const_pos_arc_with_rel;
extern sc_addr keynode_question_full_semantic_neighborhood;
extern sc_addr keynode_question_decomposition;
extern sc_addr keynode_question_all_identifiers;
extern sc_addr keynode_question_all_identified_elements;
extern sc_addr keynode_question_search_all_superclasses_in_quasybinary_relation;
extern sc_addr keynode_question_search_all_subclasses_in_quasybinary_relation;
extern sc_addr keynode_question_search_links_of_relation_connected_with_element;
extern sc_addr keynode_question_search_full_pattern;

extern sc_addr keynode_nrel_answer;
extern sc_addr keynode_quasybinary_relation;
extern sc_addr keynode_decomposition_relation;
extern sc_addr keynode_taxonomy_relation;
extern sc_addr keynode_identification_relation;
extern sc_addr keynode_order_relation;
extern sc_addr keynode_nonbinary_relation;
extern sc_addr keynode_typical_sc_neighborhood;
extern sc_addr keynode_sc_neighborhood;
extern sc_addr keynode_question_finished;
extern sc_addr keynode_question_initiated;
extern sc_addr keynode_question;
extern sc_addr keynode_nrel_identification;
extern sc_addr keynode_nrel_translation;
extern sc_addr keynode_nrel_main_idtf;
extern sc_addr keynode_nrel_inclusion;
extern sc_addr keynode_nrel_strict_inclusion;
extern sc_addr keynode_rrel_key_sc_element;
extern sc_addr keynode_nrel_key_sc_element_base_order;
extern sc_addr keynode_system_element;
extern sc_addr keynode_languages;

extern sc_addr keynode_rrel_1;
extern sc_addr keynode_rrel_2;

//! Initialie keynodes that used by search module
sc_result search_keynodes_initialize();

#endif
