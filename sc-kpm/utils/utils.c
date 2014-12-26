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
#include "utils.h"
#include "utils_keynodes.h"
#include "utils_collect_identifiers.h"
#include "utils_garbage_deletion.h"

sc_memory_context * s_default_ctx = 0;
sc_memory_context * s_garbage_ctx = 0;

sc_event *event_garbage_deletion;

sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);
    s_garbage_ctx = sc_memory_context_new(sc_access_lvl_make_max);

    if (utils_collect_keynodes_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (utils_collect_identifiers_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (utils_keynodes_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    event_garbage_deletion = sc_event_new(s_default_ctx, keynode_sc_garbage, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_garbage_delete, 0);
    if (event_garbage_deletion == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_uint32 load_priority()
{
    return 0;
}

sc_result shutdown()
{
    sc_result res = SC_RESULT_OK;

    if (utils_collect_identifiers_shutdown() != SC_RESULT_OK)
        res = SC_RESULT_ERROR;

    if (event_garbage_deletion)
        sc_event_destroy(event_garbage_deletion);

    sc_memory_context_free(s_garbage_ctx);
    sc_memory_context_free(s_default_ctx);

    return res;
}
