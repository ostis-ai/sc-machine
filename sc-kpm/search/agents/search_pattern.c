/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */
#include "search_pattern.h"
#include "search_pattern/search_pattern_functions.h"
#include "search_keynodes.h"
#include "search_defines.h"
#include "search_utils.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_full_pattern_search(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2;
    sc_bool sys_off = SC_TRUE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_search_full_pattern, question, sc_type_arc_pos_const_perm) == SC_FALSE)
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

        if (SC_RESULT_OK != search_full_pattern(sc_iterator3_value(it1, 2), answer, sys_off))
        {
            sc_iterator3_free(it1);
            return SC_RESULT_ERROR;
        }

        // iterate output arcs and append them into answer
        it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator3_value(it1, 2),
                                     sc_type_arc_pos_const_perm,
                                     sc_type_const);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2)))
                continue;

            appendIntoAnswer(answer, sc_iterator3_value(it2, 2));
        }

        //appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        sc_iterator3_free(it2);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
