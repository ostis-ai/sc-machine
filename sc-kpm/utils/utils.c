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

sc_memory_context * s_default_ctx = 0;

sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);

    if (utils_keynodes_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (utils_collect_identifiers_initialize() != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result shutdown()
{
    sc_result res = SC_RESULT_OK;

    if (utils_collect_identifiers_shutdown() != SC_RESULT_OK)
        res = SC_RESULT_ERROR;

    sc_memory_context_free(s_default_ctx);

    return res;
}
