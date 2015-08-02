/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
    g_assert(engine->funcStreamCreate != null_ptr);
    return engine->funcStreamCreate(engine, check_sum, flags, stream);
}

sc_result sc_fm_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    g_assert(engine->funcAddrRefAppend != null_ptr);
    return engine->funcAddrRefAppend(engine, addr, check_sum);
}

sc_result sc_fm_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    g_assert(engine->funcAddrRefRemove != null_ptr);
    return engine->funcAddrRefRemove(engine, addr, check_sum);
}

sc_result sc_fm_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    g_assert(engine->funcFind != null_ptr);
    return engine->funcFind(engine, check_sum, result, result_count);
}

sc_result sc_fm_clear(const sc_fm_engine *engine)
{
    g_assert(engine->funcClear != null_ptr);
    return engine->funcClear(engine);
}

sc_result sc_fm_save(const sc_fm_engine *engine)
{
    g_assert(engine->funcSave != null_ptr);
    return engine->funcSave(engine);
}

sc_result sc_fm_clean_state(const sc_fm_engine *engine)
{
    g_assert(engine->funcCleanState != null_ptr);
    return engine->funcCleanState(engine);
}
