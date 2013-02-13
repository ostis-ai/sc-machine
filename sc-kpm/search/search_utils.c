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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */

#include "search_utils.h"
#include "search_keynodes.h"
#include "search_defines.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

void connect_answer_to_question(sc_addr question, sc_addr answer)
{
    sc_addr arc;

    arc = sc_memory_arc_new(sc_type_arc_common | sc_type_const, question, answer);
    SYSTEM_ELEMENT(arc);
    arc = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_nrel_answer, arc);
    SYSTEM_ELEMENT(arc);
}

void finish_question(sc_addr question)
{
    sc_iterator3 *it = nullptr;
    sc_addr arc;

    it = sc_iterator3_f_a_f_new(keynode_question_initiated, sc_type_arc_pos_const_perm, question);
    while (sc_iterator3_next(it))
        sc_memory_element_free(sc_iterator3_value(it, 1));
    sc_iterator3_free(it);

    arc = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_question_finished, question);
    SYSTEM_ELEMENT(arc);
}
