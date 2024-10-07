/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_keynodes_h_
#define _ui_keynodes_h_

extern "C"
{
#include "sc-core/sc_memory_headers.h"
}

extern sc_addr keynode_user;
extern sc_addr keynode_command_translate_from_sc;
extern sc_addr keynode_nrel_user_result_formats;
extern sc_addr keynode_rrel_source_sc_construction;
extern sc_addr keynode_rrel_user_lang;
extern sc_addr keynode_rrel_output_format;
extern sc_addr keynode_rrel_filter_list;

extern sc_addr keynode_action_nrel_result;
extern sc_addr keynode_nrel_authors;
extern sc_addr keynode_nrel_translation;
extern sc_addr keynode_nrel_format;
extern sc_addr keynode_nrel_system_identifier;
extern sc_addr keynode_nrel_main_idtf;
extern sc_addr keynode_nrel_idtf;
extern sc_addr keynode_rrel_key_sc_element;
extern sc_addr keynode_nrel_sc_text_translation;

extern sc_addr keynode_command_generate_instance;
extern sc_addr keynode_command_initiated;
extern sc_addr keynode_command_failed;
extern sc_addr keynode_command_finished;
extern sc_addr keynode_rrel_command_arguments;
extern sc_addr keynode_rrel_command;
extern sc_addr keynode_nrel_command_template;
extern sc_addr keynode_nrel_command_result;
extern sc_addr keynode_displayed_result;

extern sc_addr keynode_format_scs_json;
extern sc_addr keynode_format_scg_json;
extern sc_addr keynode_format_scn_json;

extern sc_addr keynode_concept_scn_json_elements_order_set;
extern sc_addr keynode_concept_scn_json_elements_filter_set;
extern sc_addr keynode_nrel_scn_json_elements_base_order;

#define RREL_ORDER_COUNT 10
extern sc_addr ui_keynode_rrel_order[RREL_ORDER_COUNT];
#define UI_ARG_COUNT 10
extern sc_addr ui_keynode_arg[UI_ARG_COUNT];

//! Initialize all keynodes, that will be used in extension
sc_bool initialize_keynodes(sc_addr const init_memory_generated_structure);

#endif
