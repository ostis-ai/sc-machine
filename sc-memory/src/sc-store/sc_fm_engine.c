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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "sc_fm_engine.h"
#include "sc_fm_engine_private.h"
#include <glib.h>

void sc_fm_free(sc_fm_engine *engine)
{
    g_assert(engine->funcDestroyData);
    engine->funcDestroyData(engine);

    g_free(engine);
}

sc_result sc_fm_stream_new(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_uint8 flags, sc_stream **stream)
{
    g_assert(engine->funcStreamCreate != nullptr);
    return engine->funcStreamCreate(engine, check_sum, flags, stream);
}

sc_result sc_fm_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    g_assert(engine->funcAddrRefAppend != nullptr);
    return engine->funcAddrRefAppend(engine, addr, check_sum);
}

sc_result sc_fm_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    g_assert(engine->funcAddrRefRemove != nullptr);
    return engine->funcAddrRefRemove(engine, addr, check_sum);
}

sc_result sc_fm_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    g_assert(engine->funcFind != nullptr);
    return engine->funcFind(engine, check_sum, result, result_count);
}

sc_result sc_fm_clear(const sc_fm_engine *engine)
{
    g_assert(engine->funcClear != nullptr);
    return engine->funcClear(engine);
}

sc_result sc_fm_save(const sc_fm_engine *engine)
{
    g_assert(engine->funcSave != nullptr);
    return engine->funcSave(engine);
}

sc_result sc_fm_clean_state(const sc_fm_engine *engine)
{
    g_assert(engine->funcCleanState != nullptr);
    return engine->funcCleanState(engine);
}
