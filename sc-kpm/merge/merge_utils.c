/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "merge_utils.h"
#include "merge_defines.h"
#include "merge_keynodes.h"
#include "merge.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

void finish_question(sc_addr question)
{
    sc_iterator3 *it = null_ptr;
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
