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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "uiPrecompiled.h"
#include "uiKeynodes.h"


// ------------- Keynodes ----------------------
const char keynode_ui_user_str[] = "ui_user";
const char keynode_ui_command_translate_from_sc_str[] = "ui_command_translate_from_sc";
const char keynode_ui_nrel_user_answer_formats_str[] = "ui_nrel_user_answer_formats";
const char keynode_ui_rrel_source_sc_construction_str[] = "ui_rrel_source_sc_construction";
const char keynode_ui_rrel_output_format_str[] = "ui_rrel_output_format";


const char keynode_question_nrel_answer_str[] = "question_nrel_answer";
const char keynode_question_finished_str[] = "question_finished";
const char keynode_nrel_author_str[] = "nrel_author";
const char keynode_nrel_translation_str[] = "nrel_translation";

const char keynode_ui_command_generate_instance_str[] = "ui_command_generate_instance";
const char keynode_ui_command_initiated_str[] = "ui_command_initiated";
const char keynode_ui_command_finished_str[] = "ui_command_finished";
const char keynode_ui_rrel_command_arguments_str[] = "ui_rrel_command_arguments";
const char keynode_ui_rrel_command_str[] = "ui_rrel_command";
const char keynode_ui_nrel_command_template_str[] = "ui_nrel_command_template";
const char keynode_ui_nrel_command_result_str[] = "ui_nrel_command_result";

const char keynode_format_scs_json_str[] = "format_scs_json";
const char keynode_format_scg_json_str[] = "format_scg_json";
const char keynode_format_scn_json_str[] = "format_scn_json";


sc_addr ui_keynode_user;
sc_addr ui_keynode_command_translate_from_sc;
sc_addr ui_keynode_nrel_user_answer_formats;
sc_addr ui_keynode_rrel_source_sc_construction;
sc_addr ui_keynode_rrel_output_format;

sc_addr ui_keynode_question_nrel_answer;
sc_addr ui_keynode_question_finished;
sc_addr ui_keynode_nrel_author;
sc_addr ui_keynode_nrel_translation;

sc_addr ui_keynode_command_generate_instance;
sc_addr ui_keynode_command_initiated;
sc_addr ui_keynode_command_finished;
sc_addr ui_keynode_rrel_command_arguments;
sc_addr ui_keynode_rrel_command;
sc_addr ui_keynode_nrel_command_template;
sc_addr ui_keynode_nrel_command_result;

sc_addr ui_keynode_format_scs_json;
sc_addr ui_keynode_format_scg_json;
sc_addr ui_keynode_format_scn_json;

sc_addr ui_keynode_rrel_order[RREL_ORDER_COUNT];
sc_addr ui_keynode_arg[UI_ARG_COUNT];


// -------------------------------------------------
sc_bool initialize_keynodes()
{
    if (sc_helper_resolve_system_identifier(keynode_ui_user_str, &ui_keynode_user) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_question_nrel_answer_str, &ui_keynode_question_nrel_answer) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_question_finished_str, &ui_keynode_question_finished) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_command_translate_from_sc_str, &ui_keynode_command_translate_from_sc) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_nrel_author_str, &ui_keynode_nrel_author) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_nrel_user_answer_formats_str, &ui_keynode_nrel_user_answer_formats) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_rrel_source_sc_construction_str, &ui_keynode_rrel_source_sc_construction) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_rrel_output_format_str, &ui_keynode_rrel_output_format) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_nrel_translation_str, &ui_keynode_nrel_translation) == SC_FALSE)
        return SC_FALSE;


    if (sc_helper_resolve_system_identifier(keynode_ui_command_generate_instance_str, &ui_keynode_command_generate_instance) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_command_initiated_str, &ui_keynode_command_initiated) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_command_finished_str, &ui_keynode_command_finished) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_rrel_command_arguments_str, &ui_keynode_rrel_command_arguments) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_rrel_command_str, &ui_keynode_rrel_command) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_nrel_command_template_str, &ui_keynode_nrel_command_template) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_ui_nrel_command_result_str, &ui_keynode_nrel_command_result) == SC_FALSE)
        return SC_FALSE;


    if (sc_helper_resolve_system_identifier(keynode_format_scs_json_str, &ui_keynode_format_scs_json) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_format_scg_json_str, &ui_keynode_format_scg_json) == SC_FALSE)
        return SC_FALSE;

    if (sc_helper_resolve_system_identifier(keynode_format_scn_json_str, &ui_keynode_format_scn_json) == SC_FALSE)
        return SC_FALSE;

    for (sc_uint32 i = 0; i < RREL_ORDER_COUNT; ++i)
    {
        std::stringstream ss;
        ss << "rrel_" << (i + 1);
        if (sc_helper_resolve_system_identifier(ss.str().c_str(), &(ui_keynode_rrel_order[i])) == SC_FALSE)
            return SC_FALSE;
    }

    for (sc_uint32 i = 0; i < UI_ARG_COUNT; ++i)
    {
        std::stringstream ss;
        ss << "ui_arg_" << (i + 1);
        if (sc_helper_resolve_system_identifier(ss.str().c_str(), &(ui_keynode_arg[i])) == SC_FALSE)
            return SC_FALSE;
    }

    return SC_TRUE;
}

