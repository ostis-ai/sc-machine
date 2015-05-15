/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_identifiers.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_search_all_identifiers(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2;
    sc_iterator5 *it5;
    sc_bool found = SC_FALSE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_all_identifiers, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        found = SC_TRUE;
        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate all const arcs, that are no accessory, and go out from sc-element
        it5 = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                         sc_iterator3_value(it1, 2),
                                         sc_type_arc_common | sc_type_const,
                                         sc_type_link,
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const | sc_type_node_norole);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            // check if this relation is an identification
            if (sc_helper_check_arc(s_default_ctx, keynode_identification_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm) == SC_TRUE)
            {

                // iterate input arcs for sc-link
                it2 = sc_iterator3_a_a_f_new(s_default_ctx,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator5_value(it5, 2));
                while (sc_iterator3_next(it2) == SC_TRUE)
                {
                    if (sc_helper_check_arc(s_default_ctx, keynode_languages, sc_iterator3_value(it2, 0), sc_type_arc_pos_const_perm) == SC_TRUE)
                    {
                        appendIntoAnswer(answer, sc_iterator3_value(it2, 0));
                        appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                    }
                }
                sc_iterator3_free(it2);

                appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 2));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 4));
            }
        }
        sc_iterator5_free(it5);
    }
    sc_iterator3_free(it1);

    if (found == SC_TRUE)
        appendIntoAnswer(answer, keynode_nrel_identification);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

sc_result agent_search_all_identified_elements(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer, begin, end;
    sc_iterator3 *it1;
    sc_bool found = SC_FALSE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_all_identified_elements, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 keynode_nrel_main_idtf,
                                 sc_type_arc_pos_const_perm,
                                 sc_type_arc_common | sc_type_const);
    while (sc_iterator3_next(it1) == SC_TRUE)
    {
        found = SC_TRUE;
        sc_memory_get_arc_begin(s_default_ctx, sc_iterator3_value(it1, 2), &begin);
        sc_memory_get_arc_end(s_default_ctx, sc_iterator3_value(it1, 2), &end);

        appendIntoAnswer(answer, sc_iterator3_value(it1, 1));
        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));
        appendIntoAnswer(answer, begin);
        appendIntoAnswer(answer, end);
    }
    sc_iterator3_free(it1);

    if (found == SC_TRUE)
        appendIntoAnswer(answer, keynode_nrel_main_idtf);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
