/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2013 OSTIS

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
#include "search_identifiers.h"
#include "search_keynodes.h"
#include "search_utils.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_search_all_identifiers(sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1;
    sc_iterator5 *it5, *it5Check;
    sc_bool found = SC_FALSE;
    sc_uint32 i;

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_all_identifiers, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = sc_memory_node_new(sc_type_node | sc_type_const);

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        found = SC_TRUE;
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, sc_iterator3_value(it1, 2));

        // iterate all const arcs, that are no accessory, and go out from sc-element
        it5 = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it1, 2),
                                         sc_type_arc_common | sc_type_const,
                                         sc_type_link,
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            // check if this arc is an identification
            it5Check = sc_iterator5_f_a_f_a_f_new(keynode_hypermedia_nrel_identification,
                                                  sc_type_arc_common | sc_type_const,
                                                  sc_iterator5_value(it5, 4),
                                                  sc_type_arc_pos_const_perm,
                                                  keynode_set_nrel_inclusion);

            if (sc_iterator5_next(it5Check) == SC_TRUE)
            {
                // append into result
                for (i = 0;  i < 5; ++i)
                    sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, sc_iterator5_value(it5, i));
            }
            sc_iterator5_free(it5Check);
        }
        sc_iterator5_free(it5);
    }
    sc_iterator3_free(it1);

    if (found == SC_TRUE)
        sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, keynode_nrel_decomposition);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
