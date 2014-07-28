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

#include "merge_utils.h"
#include "merge_defines.h"
#include "merge_keynodes.h"
#include "merge.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

void finish_question(sc_addr question)
{
    sc_iterator3 *it = nullptr;
    sc_addr arc;

    it = sc_iterator3_f_a_f_new(s_default_ctx,
                                keynode_question_initiated,
                                sc_type_arc_pos_const_perm, question);
    while (sc_iterator3_next(it))
        sc_memory_element_free(s_default_ctx, sc_iterator3_value(it, 1));
    sc_iterator3_free(it);

    arc = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, keynode_question_finished, question);
    SYSTEM_ELEMENT(arc);
}

void copy_incident_connectors(sc_addr dest, sc_addr source)
{
    sc_addr new_connector;
    sc_type conn_type;
    sc_iterator3 *it;

    // Output connectors loop
    it = sc_iterator3_f_a_a_new(s_default_ctx,
                                source,
                                0,
                                0);
    while (sc_iterator3_next(it))
    {
        if (SC_RESULT_OK != sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it, 1), &conn_type))
            continue;
        new_connector = sc_memory_arc_new(s_default_ctx, conn_type, dest, sc_iterator3_value(it, 2));
        copy_incident_connectors(new_connector, sc_iterator3_value(it, 1));
    }
    sc_iterator3_free(it);

    // Input connectors loop
    it = sc_iterator3_a_a_f_new(s_default_ctx,
                                0,
                                0,
                                source);
    while (sc_iterator3_next(it))
    {
        if (SC_RESULT_OK != sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it, 1), &conn_type))
            continue;
        new_connector = sc_memory_arc_new(s_default_ctx, conn_type, sc_iterator3_value(it, 0), dest);
        copy_incident_connectors(new_connector, sc_iterator3_value(it, 1));
    }
    sc_iterator3_free(it);
}

sc_result merge_sc_elements(sc_addr first, sc_addr second)
{
    copy_incident_connectors(first, second);

    return sc_memory_element_free(s_default_ctx, second);
}

sc_addr resolve_sc_addr_from_pointer(gpointer data)
{
    sc_addr elem;
    elem.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(GPOINTER_TO_INT(data));
    elem.seg = SC_ADDR_LOCAL_SEG_FROM_INT(GPOINTER_TO_INT(data));
    return elem;
}
