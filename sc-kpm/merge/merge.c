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

#include "merge.h"
#include "sc_memory_headers.h"
#include "merge_keynodes.h"
#include "merge_agents.h"

sc_memory_context * s_default_ctx = 0;

sc_event *event_question_set_cantorization;

sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);

    if (merge_keynodes_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    event_question_set_cantorization = sc_event_new(s_default_ctx, keynode_question_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_set_cantorization, 0);
    if (event_question_set_cantorization == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result shutdown()
{
    sc_event_destroy(event_question_set_cantorization);
    sc_memory_context_free(s_default_ctx);

    return SC_RESULT_OK;
}
