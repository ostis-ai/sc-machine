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

#include "search_utils.h"
#include "search_keynodes.h"
#include "search_defines.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>
#include <glib.h>

sc_addr create_answer_node()
{
    sc_addr res = sc_memory_node_new(sc_type_const);
    SYSTEM_ELEMENT(res);
    return res;
}

void connect_answer_to_question(sc_addr question, sc_addr answer)
{
    sc_addr arc;

    arc = sc_memory_arc_new(sc_type_arc_common | sc_type_const, question, answer);
    SYSTEM_ELEMENT(arc);
    arc = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_nrel_answer, arc);
    SYSTEM_ELEMENT(arc);
}

void appendIntoAnswer(sc_addr answer, sc_addr el)
{
    sc_addr arc;
    if (sc_helper_check_arc(answer, el, sc_type_arc_pos_const_perm) == SC_TRUE)
        return;

    arc = sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, el);
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

void log_agent_behavior(sc_char *format, sc_addr agent_keynode)
{
    sc_addr idtf;
    if (SC_RESULT_OK == sc_helper_get_system_identifier(agent_keynode, &idtf))
    {
        sc_stream *stream;
        sc_uint32 length = 0, read_length = 0;
        sc_char *data;
        sc_memory_get_link_content(idtf, &stream);
        sc_stream_get_length(stream, &length);
        data = calloc(length + 1, sizeof(sc_char));
        sc_stream_read_data(stream, data, length, &read_length);
        data[length] = '\0';
        g_message(format, data);
        sc_stream_free(stream);
        free(data);
    }
}

void log_agent_started(sc_addr agent_keynode)
{
    log_agent_behavior("sc-agent started: %s\n", agent_keynode);
}

void log_agent_finished_successfully(sc_addr agent_keynode)
{
    log_agent_behavior("sc-agent finished successfully: %s\n", agent_keynode);
}

void log_agent_finished_unsuccessfully(sc_addr agent_keynode)
{
    log_agent_behavior("sc-agent finished unsuccessfully: %s\n", agent_keynode);
}
