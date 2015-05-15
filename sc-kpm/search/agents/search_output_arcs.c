/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_output_arcs.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_search_all_const_pos_output_arc(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2;
    sc_bool sys_off = SC_TRUE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_all_output_const_pos_arc, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        // iterate output arcs and append them into answer
        it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator3_value(it1, 2),
                                     sc_type_arc_pos_const_perm,
                                     0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 2));
        }

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        sc_iterator3_free(it2);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

// ---------------------------------------------
sc_result agent_search_all_const_pos_output_arc_with_rel(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2, *it3;
    sc_bool sys_off = SC_TRUE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_all_output_const_pos_arc_with_rel, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        // iterate output arcs and append them into answer
        it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator3_value(it1, 2),
                                     sc_type_arc_pos_const_perm,
                                     0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                continue;

            // iterate relations
            it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                         sc_type_node,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it3) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
            }
            sc_iterator3_free(it3);

            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 2));
        }
        sc_iterator3_free(it2);

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));
    }
    sc_iterator3_free(it1);


    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
