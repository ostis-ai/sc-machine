/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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
#include "search_structure.h"
#include "search_keynodes.h"
#include "search_utils.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_search_decomposition(sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2;
    sc_iterator5 *it5, *it_order;

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_decomposition, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        //found = SC_TRUE;
        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate decomposition
        it5 = sc_iterator5_a_a_f_a_a_new(sc_type_node | sc_type_const,
                                         sc_type_arc_common | sc_type_const,
                                         sc_iterator3_value(it1, 2),
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (SC_FALSE == sc_helper_check_arc(keynode_decomposition_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
                continue;

            appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

            // iterate decomposition set elements
            it2 = sc_iterator3_f_a_a_new(sc_iterator5_value(it5, 0), sc_type_arc_pos_const_perm, 0);
            while (sc_iterator3_next(it2) == SC_TRUE)
            {
                appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it2, 2));

                // iterate order relations between elements
                it_order = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it2, 2),
                                                      sc_type_arc_common | sc_type_const,
                                                      sc_type_node | sc_type_const,
                                                      sc_type_arc_pos_const_perm,
                                                      sc_type_node | sc_type_const);
                while (sc_iterator5_next(it_order) == SC_TRUE)
                {
                    if (SC_FALSE == sc_helper_check_arc(keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                        continue;
                    if (SC_FALSE == sc_helper_check_arc(sc_iterator5_value(it5, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
                        continue;

                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 1));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 2));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 3));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 4));

                }
                sc_iterator5_free(it_order);

            }
            sc_iterator3_free(it2);
        }
        sc_iterator5_free(it5);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
