/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiCommands.h"
#include "uiKeynodes.h"
#include "uiDefines.h"

#include <stdio.h>

// -------------------- Events ----------------------
sc_event *event_ui_start_answer_translation = 0;
sc_event *event_ui_command_generate_instance = 0;
sc_event *event_ui_remove_displayed_answer = 0;

struct sTemplateArcInfo
{
    sc_addr self_addr;
    sc_addr begin_addr;
    sc_addr end_addr;
    sc_type self_type;

	sTemplateArcInfo(sc_addr _self, sc_addr _beg, sc_addr _end, sc_type _type)
        : self_addr(_self)
        , begin_addr(_beg)
        , end_addr(_end)
        , self_type(_type)
    {

    }
};

typedef std::list < sTemplateArcInfo > tTemplArcsList;
typedef std::list < sc_addr > tElementsList;

// -------------------- Event handlers --------------
sc_result ui_command_generate_instance(const sc_event *event, sc_addr arg)
{
    sc_addr command_addr;
    sc_addr args_addr;
    sc_iterator5 *it5 = (sc_iterator5*)null_ptr;
    sc_iterator3 *it3 = (sc_iterator3*)null_ptr;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &command_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // first of all we need to find command arguments
    it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                     command_addr,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_rrel_command_arguments);

    if (sc_iterator5_next(it5) != SC_TRUE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    args_addr = sc_iterator5_value(it5, 2);
    sc_iterator5_free(it5);


    int idx = 0;
    bool found = true;
    tScAddrVector arguments;
    while (found)
    {
        found = false;
        // iterate arguments and append them into vector
        it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                         args_addr,
                                         sc_type_arc_pos_const_perm,
                                         0,
                                         sc_type_arc_pos_const_perm,
                                         ui_keynode_rrel_order[idx]);

        if (sc_iterator5_next(it5) == SC_TRUE)
        {
            arguments.push_back(sc_iterator5_value(it5, 2));
            found = true;
        }
        sc_iterator5_free(it5);
        ++idx;
    }

    // get command class
    it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                     command_addr,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_rrel_command);

    if (sc_iterator5_next(it5) != SC_TRUE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    sc_addr new_command_class_addr;
    new_command_class_addr = sc_iterator5_value(it5, 2);
    sc_iterator5_free(it5);

    // get command template
    it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                     new_command_class_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_command_template);

    if (sc_iterator5_next(it5) != SC_TRUE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    sc_addr new_command_templ_addr;
    new_command_templ_addr = sc_iterator5_value(it5, 2);
    sc_iterator5_free(it5);

    // collect all elements in command template
    sc_addr templ_item_addr;
    sc_type templ_item_type;
    tScAddrToScAddrMap templ_to_inst;

	tElementsList created_nodes;
    tTemplArcsList templ_arcs;
	bool isValid = true;
	it3 = sc_iterator3_f_a_a_new(s_default_ctx,
								 new_command_templ_addr,
								 sc_type_arc_pos_const_perm,
								 0);
    while (sc_iterator3_next(it3) == SC_TRUE && isValid)
    {
        templ_item_addr = sc_iterator3_value(it3, 2);
        sc_memory_get_element_type(s_default_ctx, templ_item_addr, &templ_item_type);

        if (templ_item_type & sc_type_var)
        {
            if (templ_item_type & sc_type_arc_mask)
            {
                // arcs will be processed later
                sc_addr beg_addr, end_addr;
                // todo checks
                sc_memory_get_arc_begin(s_default_ctx, templ_item_addr, &beg_addr);
                sc_memory_get_arc_end(s_default_ctx, templ_item_addr, &end_addr);
                templ_arcs.push_back(sTemplateArcInfo(templ_item_addr, beg_addr, end_addr, templ_item_type));
            }
            else
            {
                if (templ_item_type & sc_type_node)
                {
					sc_addr const new_addr = sc_memory_node_new(s_default_ctx, (templ_item_type & ~sc_type_var) | sc_type_const);
					templ_to_inst[templ_item_addr] = new_addr;
					created_nodes.push_back(new_addr);
                }
				else
                {
                    if (templ_item_type & sc_type_link)
                        assert("sc-links not supported yet");
                }
            }
        }
		else
        {
            // check arguments
            bool is_argument = false;
            for (sc_uint32 i = 0; i < UI_ARG_COUNT; ++i)
            {
                if (SC_ADDR_IS_EQUAL(templ_item_addr, ui_keynode_arg[i]))
                {
                    is_argument = true;
					if (i >= arguments.size())
					{
						isValid = false;
					}
					else
					{
						templ_to_inst[templ_item_addr] = arguments[i];
					}
                    break;
                }
            }

            if (!is_argument)
                templ_to_inst[templ_item_addr] = templ_item_addr;
        }
    }

	sc_iterator3_free(it3);

	if (!isValid)
	{
		tElementsList::iterator it_res, it_res_end = created_nodes.end();
		for (it_res = created_nodes.begin(); it_res != it_res_end; ++it_res)
			sc_memory_element_free(s_default_ctx, *it_res);

		// append to set of failed commands
		sc_memory_element_free(s_default_ctx, arg);
		sc_addr arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_command_failed, command_addr);
		SYSTEM_ELEMENT(arc_addr);

		return SC_RESULT_ERROR_INVALID_PARAMS;
	}

    // now process arcs
    bool created = true;
    tScAddrToScAddrMap::iterator it_arc_beg, it_arc_end;
    sc_addr arc_addr, arc_beg_addr, arc_end_addr, new_arc_addr;
    sc_type arc_type;
    while (created)
    {
        created = false;
        tTemplArcsList::iterator it, it_end = templ_arcs.end();
		it = templ_arcs.begin();

        while (it != it_end)
        {
            arc_addr = (*it).self_addr;
            arc_beg_addr = (*it).begin_addr;
            arc_end_addr = (*it).end_addr;
            arc_type = (*it).self_type;


            it_arc_beg = templ_to_inst.find(arc_beg_addr);
            it_arc_end = templ_to_inst.find(arc_end_addr);

            // check if arc can be created
			if (it_arc_beg != templ_to_inst.end() && it_arc_end != templ_to_inst.end())
			{
				created = true;
				new_arc_addr = sc_memory_arc_new(s_default_ctx, (arc_type & ~sc_type_var) | sc_type_const, (*it_arc_beg).second, (*it_arc_end).second);
				templ_to_inst[arc_addr] = new_arc_addr;

				it = templ_arcs.erase(it);
			}
			else
				++it;
        }
    }

    // need to be empty
    assert(templ_arcs.empty());

    // create contour, that contains instance of command
    sc_addr created_instance_addr = sc_memory_node_new(s_default_ctx, sc_type_node_struct | sc_type_const);
    SYSTEM_ELEMENT(created_instance_addr);
    tScAddrToScAddrMap::iterator it_res, it_res_end = templ_to_inst.end();
    for (it_res = templ_to_inst.begin(); it_res != it_res_end; ++it_res)
    {
        arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, created_instance_addr, (*it_res).second);
        SYSTEM_ELEMENT(arc_addr);
    }

    // generate result for command
    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_common | sc_type_const, command_addr, created_instance_addr);
    SYSTEM_ELEMENT(arc_addr);
    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_nrel_command_result, arc_addr);
    SYSTEM_ELEMENT(arc_addr);

    // change command state
    sc_memory_element_free(s_default_ctx, arg);
    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_command_finished, command_addr);
    SYSTEM_ELEMENT(arc_addr);

    return SC_RESULT_OK;
}

sc_result ui_start_answer_translation(sc_event *event, sc_addr arg)
{
    sc_addr question_addr;
    sc_addr answer_addr;
    sc_addr author_addr;
    sc_addr output_formats_addr;
    sc_addr format_addr;
    sc_addr trans_command_addr;
    sc_addr arc_addr;
    sc_iterator5 *it5 = (sc_iterator5*)null_ptr;
    sc_iterator3 *it3 = (sc_iterator3*)null_ptr;


    if (sc_memory_get_arc_end(s_default_ctx, arg, &question_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                     question_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_FALSE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    answer_addr = sc_iterator5_value(it5, 2);

    // find author of this question
    it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                     question_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_authors);
    if (it5 == null_ptr)
        return SC_RESULT_ERROR;

    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        author_addr = sc_iterator5_value(it5, 2);
        sc_iterator5_free(it5);

        // check if author is an user
        if (sc_helper_check_arc(s_default_ctx, keynode_user, author_addr, sc_type_arc_pos_const_perm) == SC_TRUE)
        {
            // get answer output formats
            it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                             question_addr,
                                             sc_type_arc_common | sc_type_const,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             keynode_nrel_user_answer_formats);
            if (it5 == null_ptr)
                return SC_RESULT_ERROR;

            if (sc_iterator5_next(it5) == SC_TRUE)
            {
                output_formats_addr = sc_iterator5_value(it5, 2);
                sc_iterator5_free(it5);

                // list all output formats and initialize translation
                it3 = sc_iterator3_f_a_a_new(s_default_ctx,
                                             output_formats_addr,
                                             sc_type_arc_pos_const_perm,
                                             sc_type_node | sc_type_const);
                if (it3 == null_ptr)
                    return SC_RESULT_ERROR;

                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    format_addr = sc_iterator3_value(it3, 2);

                    // init translation command
                    trans_command_addr = sc_memory_node_new(s_default_ctx, sc_type_const);
                    SYSTEM_ELEMENT(trans_command_addr);

                    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, trans_command_addr, answer_addr);
                    SYSTEM_ELEMENT(arc_addr);
                    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_rrel_source_sc_construction, arc_addr);
                    SYSTEM_ELEMENT(arc_addr);

                    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, trans_command_addr, format_addr);
                    SYSTEM_ELEMENT(arc_addr);
                    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_rrel_output_format, arc_addr);
                    SYSTEM_ELEMENT(arc_addr);

                    // add into translation command set
                    arc_addr = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_command_translate_from_sc, trans_command_addr);
                    SYSTEM_ELEMENT(arc_addr);
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

sc_result ui_remove_displayed_answer(sc_event *event, sc_addr arg)
{
    sc_addr answer_addr;
    sc_iterator5 *it5 = 0;
    sc_iterator5 *it5Res = 0;
    sc_iterator5 *it5Args = 0;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &answer_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // first of all delete translation command
    it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     answer_addr,
                                     sc_type_arc_pos_const_perm,
                                     keynode_rrel_source_sc_construction);
    if (sc_iterator5_next(it5) == SC_TRUE)
        sc_memory_element_free(s_default_ctx, sc_iterator5_value(it5, 0));
    sc_iterator5_free(it5);

    // remove translation result
    it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                     answer_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_link,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_translation);
    if (sc_iterator5_next(it5) == SC_TRUE)
        sc_memory_element_free(s_default_ctx, sc_iterator5_value(it5, 2));
    sc_iterator5_free(it5);

    // find question, and remove all connected information
    it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     answer_addr,
                                     sc_type_arc_pos_const_perm,
                                     keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        it5Res = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                            0,
                                            0,
                                            sc_iterator5_value(it5, 0),
                                            0,
                                            keynode_nrel_command_result);

        if (sc_iterator5_next(it5Res) == SC_TRUE)
        {
            it5Args = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                                 sc_iterator5_value(it5Res, 0),
                                                 sc_type_arc_pos_const_perm,
                                                 sc_type_node | sc_type_const,
                                                 sc_type_arc_pos_const_perm,
                                                 keynode_rrel_command_arguments);
            if (sc_iterator5_next(it5Args) == SC_TRUE)
                sc_memory_element_free(s_default_ctx, sc_iterator5_value(it5Args, 2));
            sc_iterator5_free(it5Args);

            sc_memory_element_free(s_default_ctx, sc_iterator5_value(it5Res, 0));
        }
        sc_iterator5_free(it5Res);

        sc_memory_element_free(s_default_ctx, sc_iterator5_value(it5, 0));
    }
    sc_iterator5_free(it5);

    // now we can remove answer node
    sc_memory_element_free(s_default_ctx, answer_addr);

    return SC_RESULT_OK;
}

// -------------------- Module ----------------------
sc_result ui_initialize_commands()
{
    /*event_ui_start_answer_translation = sc_event_new(keynode_question_finished, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_start_answer_translation, 0);
    if (event_ui_start_answer_translation == null)
        return SC_RESULT_ERROR;*/

    event_ui_command_generate_instance = sc_event_new(s_default_ctx, keynode_command_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_command_generate_instance, 0);
    if (event_ui_command_generate_instance == null_ptr)
        return SC_RESULT_ERROR;

    /*event_ui_remove_displayed_answer = sc_event_new(keynode_displayed_answer, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_remove_displayed_answer, 0);
    if (event_ui_remove_displayed_answer == null)
        return SC_RESULT_ERROR;*/

    return SC_RESULT_OK;
}

void ui_shutdown_commands()
{
    if (event_ui_start_answer_translation)
        sc_event_destroy(event_ui_start_answer_translation);
    event_ui_start_answer_translation = (sc_event*)null_ptr;

    if (event_ui_command_generate_instance)
        sc_event_destroy(event_ui_command_generate_instance);
    event_ui_command_generate_instance = (sc_event*)null_ptr;

    if (event_ui_remove_displayed_answer)
        sc_event_destroy(event_ui_remove_displayed_answer);
    event_ui_remove_displayed_answer = (sc_event*)null_ptr;
}
