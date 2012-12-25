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
#include "uiCommands.h"
#include "uiKeynodes.h"


// -------------------- Events ----------------------
sc_event *event_ui_start_answer_translation = 0;

// -------------------- Event handlers --------------
sc_result ui_start_answer_translation(sc_event *event, sc_addr arg)
{
    sc_addr answer_arc_addr;
    sc_addr question_addr;
    sc_addr answer_addr;
    sc_addr author_addr;
    sc_addr output_formats_addr;
    sc_addr format_addr;
    sc_addr trans_command_addr;
    sc_addr arc_addr;
    sc_iterator5 *it5 = (sc_iterator5*)nullptr;
    sc_iterator3 *it3 = (sc_iterator3*)nullptr;


    if (sc_memory_get_arc_end(arg, &answer_arc_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // get question node
    if (sc_memory_get_arc_begin(answer_arc_addr, &question_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // get answer node
    if (sc_memory_get_arc_end(answer_arc_addr, &answer_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // find author of this question
    it5 = sc_iterator5_f_a_a_a_f_new(question_addr,
                                    sc_type_arc_common | sc_type_const,
                                    sc_type_node | sc_type_const,
                                    sc_type_arc_pos_const_perm,
                                    ui_keynode_nrel_author);
    if (it5 == nullptr)
        return SC_RESULT_ERROR;

    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        author_addr = sc_iterator5_value(it5, 2);
        sc_iterator5_free(it5);

        // check if author is an user
        if (sc_helper_check_arc(ui_keynode_ui_user, author_addr, sc_type_arc_pos_const_perm) == SC_TRUE)
        {
            // get answer output formats
            it5 = sc_iterator5_f_a_a_a_f_new(question_addr,
                                            sc_type_arc_common | sc_type_const,
                                            sc_type_node | sc_type_const,
                                            sc_type_arc_pos_const_perm,
                                            ui_keynode_ui_nrel_user_answer_formats);
            if (it5 == nullptr)
                return SC_RESULT_ERROR;

            if (sc_iterator5_next(it5) == SC_TRUE)
            {
                output_formats_addr = sc_iterator5_value(it5, 2);
                sc_iterator5_free(it5);

                // list all output formats and initialize translation
                it3 = sc_iterator3_f_a_a_new(output_formats_addr,
                                             sc_type_arc_pos_const_perm,
                                             sc_type_node | sc_type_const);
                if (it3 == nullptr)
                    return SC_RESULT_ERROR;

                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    format_addr = sc_iterator3_value(it3, 2);

                    // init translation command
                    trans_command_addr = sc_memory_node_new(sc_type_const);

                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, trans_command_addr, answer_addr);
                    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_ui_rrel_source_sc_construction, arc_addr);

                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, trans_command_addr, format_addr);
                    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_ui_rrel_output_format, arc_addr);

                    // add into translation command set
                    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_ui_command_translate_from_sc, trans_command_addr);
                }
                sc_iterator3_free(it3);

            }else
                sc_iterator5_free(it5);

        }
    }else
        sc_iterator5_free(it5);


    // get answer node

    return SC_RESULT_OK;
}

// -------------------- Module ----------------------
sc_result ui_initialize_commands()
{
    event_ui_start_answer_translation = sc_event_new(ui_keynode_question_nrel_answer, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_start_answer_translation, 0);
    if (event_ui_start_answer_translation == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

void ui_shutdown_commands()
{

}
