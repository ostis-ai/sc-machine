/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiKeynodes.h"

extern "C"
{
#include "../sc-common/sc_keynodes.h"
#include <glib.h>
}

// ------------- Keynodes ----------------------
char const keynode_user_str[] = "ui_user";
char const keynode_command_translate_from_sc_str[] = "ui_command_translate_from_sc";
char const keynode_nrel_user_answer_formats_str[] = "ui_nrel_user_answer_formats";
char const keynode_rrel_source_sc_construction_str[] = "ui_rrel_source_sc_construction";
char const keynode_rrel_output_format_str[] = "ui_rrel_output_format";
char const keynode_rrel_user_lang_str[] = "ui_rrel_user_lang";
char const keynode_rrel_filter_list_str[] = "ui_rrel_filter_list";

char const keynode_action_nrel_answer_str[] = "nrel_answer";
char const keynode_action_finished_str[] = "action_finished";
char const keynode_nrel_authors_str[] = "nrel_authors";
char const keynode_nrel_translation_str[] = "nrel_translation";
char const keynode_nrel_format_str[] = "nrel_format";
char const keynode_nrel_system_identifier_str[] = "nrel_system_identifier";
char const keynode_nrel_main_idtf_str[] = "nrel_main_idtf";
char const keynode_nrel_idtf_str[] = "nrel_idtf";
char const keynode_rrel_key_sc_element_str[] = "rrel_key_sc_element";
char const keynode_nrel_sc_text_translation_str[] = "nrel_sc_text_translation";

char const keynode_command_generate_instance_str[] = "ui_command_generate_instance";
char const keynode_command_initiated_str[] = "ui_command_initiated";
char const keynode_command_finished_str[] = "ui_command_finished";
char const keynode_command_failed_str[] = "ui_command_failed";
char const keynode_rrel_command_arguments_str[] = "ui_rrel_command_arguments";
char const keynode_rrel_command_str[] = "ui_rrel_command";
char const keynode_nrel_command_template_str[] = "ui_nrel_command_template";
char const keynode_nrel_command_result_str[] = "ui_nrel_command_result";
char const keynode_displayed_answer_str[] = "ui_displayed_answer";

char const keynode_format_scs_json_str[] = "format_scs_json";
char const keynode_format_scg_json_str[] = "format_scg_json";
char const keynode_format_scn_json_str[] = "format_scn_json";

char const keynode_system_element_str[] = "system_element";

char const keynode_concept_scn_json_elements_order_set_str[] = "concept_scn_json_elements_order_set";
char const keynode_concept_scn_json_elements_filter_set_str[] = "concept_scn_json_elements_filter_set";
char const keynode_nrel_scn_json_elements_base_order_str[] = "nrel_scn_json_elements_base_order";

sc_addr keynode_user;
sc_addr keynode_command_translate_from_sc;
sc_addr keynode_nrel_user_answer_formats;
sc_addr keynode_rrel_source_sc_construction;
sc_addr keynode_rrel_output_format;
sc_addr keynode_rrel_user_lang;
sc_addr keynode_rrel_filter_list;

sc_addr keynode_action_nrel_answer;
sc_addr keynode_action_finished;
sc_addr keynode_nrel_authors;
sc_addr keynode_nrel_translation;
sc_addr keynode_nrel_format;
sc_addr keynode_nrel_system_identifier;
sc_addr keynode_nrel_main_idtf;
sc_addr keynode_nrel_idtf;
sc_addr keynode_rrel_key_sc_element;
sc_addr keynode_nrel_sc_text_translation;

sc_addr keynode_command_generate_instance;
sc_addr keynode_command_initiated;
sc_addr keynode_command_finished;
sc_addr keynode_command_failed;
sc_addr keynode_rrel_command_arguments;
sc_addr keynode_rrel_command;
sc_addr keynode_nrel_command_template;
sc_addr keynode_nrel_command_result;
sc_addr keynode_displayed_answer;

sc_addr keynode_format_scs_json;
sc_addr keynode_format_scg_json;
sc_addr keynode_format_scn_json;

sc_addr keynode_system_element;

sc_addr keynode_concept_scn_json_elements_order_set;
sc_addr keynode_concept_scn_json_elements_filter_set;
sc_addr keynode_nrel_scn_json_elements_base_order;

sc_addr ui_keynode_rrel_order[RREL_ORDER_COUNT];
sc_addr ui_keynode_arg[UI_ARG_COUNT];

// -------------------------------------------------
sc_bool initialize_keynodes(sc_addr const init_memory_generated_structure)
{
  RESOLVE_KEYNODE(s_default_ctx, keynode_user, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_action_nrel_answer, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_action_finished, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_command_translate_from_sc, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_authors, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_user_answer_formats, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_source_sc_construction, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_output_format, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_user_lang, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_filter_list, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_translation, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_format, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_system_identifier, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_main_idtf, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_idtf, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_key_sc_element, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_sc_text_translation, init_memory_generated_structure);

  RESOLVE_KEYNODE(s_default_ctx, keynode_command_generate_instance, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_command_initiated, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_command_failed, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_command_finished, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_command_arguments, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_command, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_command_template, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_command_result, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_displayed_answer, init_memory_generated_structure);

  RESOLVE_KEYNODE(s_default_ctx, keynode_format_scs_json, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_format_scg_json, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_format_scn_json, init_memory_generated_structure);

  RESOLVE_KEYNODE(s_default_ctx, keynode_system_element, init_memory_generated_structure);

  RESOLVE_KEYNODE(s_default_ctx, keynode_concept_scn_json_elements_order_set, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_concept_scn_json_elements_filter_set, init_memory_generated_structure);
  RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_scn_json_elements_base_order, init_memory_generated_structure);

  for (sc_uint32 i = 0; i < RREL_ORDER_COUNT; ++i)
  {
    std::stringstream ss;
    ss << "rrel_" << (i + 1);
    if (sc_helper_resolve_system_identifier(s_default_ctx, ss.str().c_str(), &(ui_keynode_rrel_order[i])) == SC_FALSE)
    {
      ui_keynode_rrel_order[i] = sc_memory_node_new(s_default_ctx, 0);
      if (sc_helper_set_system_identifier(
              s_default_ctx, ui_keynode_rrel_order[i], ss.str().c_str(), (sc_uint32)ss.str().size())
          != SC_RESULT_OK)
        return SC_FALSE;
    }
  }

  for (sc_uint32 i = 0; i < UI_ARG_COUNT; ++i)
  {
    std::stringstream ss;
    ss << "ui_arg_" << (i + 1);
    if (sc_helper_resolve_system_identifier(s_default_ctx, ss.str().c_str(), &(ui_keynode_arg[i])) == SC_FALSE)
    {
      ui_keynode_rrel_order[i] = sc_memory_node_new(s_default_ctx, 0);
      if (sc_helper_set_system_identifier(
              s_default_ctx, ui_keynode_rrel_order[i], ss.str().c_str(), (sc_uint32)ss.str().size())
          != SC_RESULT_OK)
        return SC_FALSE;
    }
  }

  return SC_TRUE;
}
