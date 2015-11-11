/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiKeynodes.h"

extern "C"
{
#include "../common/sc_keynodes.h"
#include <glib.h>
}


// ------------- Keynodes ----------------------
const char keynode_user_str[] = "ui_user";
const char keynode_command_translate_from_sc_str[] = "ui_command_translate_from_sc";
const char keynode_nrel_user_answer_formats_str[] = "ui_nrel_user_answer_formats";
const char keynode_rrel_source_sc_construction_str[] = "ui_rrel_source_sc_construction";
const char keynode_rrel_output_format_str[] = "ui_rrel_output_format";


const char keynode_question_nrel_answer_str[] = "nrel_answer";
const char keynode_question_finished_str[] = "question_finished";
const char keynode_nrel_authors_str[] = "nrel_authors";
const char keynode_nrel_translation_str[] = "nrel_translation";
const char keynode_nrel_format_str[] = "nrel_format";


const char keynode_command_generate_instance_str[] = "ui_command_generate_instance";
const char keynode_command_initiated_str[] = "ui_command_initiated";
const char keynode_command_finished_str[] = "ui_command_finished";
const char keynode_command_failed_str[] = "ui_command_failed";
const char keynode_rrel_command_arguments_str[] = "ui_rrel_command_arguments";
const char keynode_rrel_command_str[] = "ui_rrel_command";
const char keynode_nrel_command_template_str[] = "ui_nrel_command_template";
const char keynode_nrel_command_result_str[] = "ui_nrel_command_result";
const char keynode_displayed_answer_str[] = "ui_displayed_answer";

const char keynode_format_scs_json_str[] = "format_scs_json";
const char keynode_format_scg_json_str[] = "format_scg_json";
const char keynode_format_scn_json_str[] = "format_scn_json";

const char keynode_system_element_str[] = "system_element";


sc_addr keynode_user;
sc_addr keynode_command_translate_from_sc;
sc_addr keynode_nrel_user_answer_formats;
sc_addr keynode_rrel_source_sc_construction;
sc_addr keynode_rrel_output_format;

sc_addr keynode_question_nrel_answer;
sc_addr keynode_question_finished;
sc_addr keynode_nrel_authors;
sc_addr keynode_nrel_translation;
sc_addr keynode_nrel_format;

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

sc_addr ui_keynode_rrel_order[RREL_ORDER_COUNT];
sc_addr ui_keynode_arg[UI_ARG_COUNT];


// -------------------------------------------------
sc_bool initialize_keynodes()
{
    RESOLVE_KEYNODE(s_default_ctx, keynode_user);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_nrel_answer);
    RESOLVE_KEYNODE(s_default_ctx, keynode_question_finished);
    RESOLVE_KEYNODE(s_default_ctx, keynode_command_translate_from_sc);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_authors);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_user_answer_formats);
    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_source_sc_construction);
    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_output_format);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_translation);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_format);

    RESOLVE_KEYNODE(s_default_ctx, keynode_command_generate_instance);
    RESOLVE_KEYNODE(s_default_ctx, keynode_command_initiated);
	RESOLVE_KEYNODE(s_default_ctx, keynode_command_failed);
    RESOLVE_KEYNODE(s_default_ctx, keynode_command_finished);
    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_command_arguments);
    RESOLVE_KEYNODE(s_default_ctx, keynode_rrel_command);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_command_template);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_command_result);
    RESOLVE_KEYNODE(s_default_ctx, keynode_displayed_answer);

    RESOLVE_KEYNODE(s_default_ctx, keynode_format_scs_json);
    RESOLVE_KEYNODE(s_default_ctx, keynode_format_scg_json);
    RESOLVE_KEYNODE(s_default_ctx, keynode_format_scn_json);

    RESOLVE_KEYNODE(s_default_ctx, keynode_system_element);

    for (sc_uint32 i = 0; i < RREL_ORDER_COUNT; ++i)
    {
        std::stringstream ss;
        ss << "rrel_" << (i + 1);
        if (sc_helper_resolve_system_identifier(s_default_ctx, ss.str().c_str(), &(ui_keynode_rrel_order[i])) == SC_FALSE)
        {
            ui_keynode_rrel_order[i] = sc_memory_node_new(s_default_ctx, 0);
            if (sc_helper_set_system_identifier(s_default_ctx, ui_keynode_rrel_order[i], ss.str().c_str(), (sc_uint32)ss.str().size()) != SC_RESULT_OK)
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
            if (sc_helper_set_system_identifier(s_default_ctx, ui_keynode_rrel_order[i], ss.str().c_str(), (sc_uint32)ss.str().size()) != SC_RESULT_OK)
                return SC_FALSE;
        }
    }

    return SC_TRUE;
}

