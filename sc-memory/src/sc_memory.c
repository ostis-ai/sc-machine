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

#include "sc_memory.h"
#include "sc_memory_version.h"
#include "sc_memory_private.h"
#include "sc_element.h"
#include "sc-store/sc_storage.h"
#include "sc-store/sc_element.h"
#include "sc_memory_ext.h"
#include "sc_helper.h"
#include "sc-store/sc_config.h"
#include "sc_helper_private.h"
#include "sc-store/sc_event.h"
#include "sc-store/sc_event/sc_event_private.h"

#include <glib.h>

sc_memory_context * s_memory_default_ctx = 0;

GRecMutex mutex;

#define LOCK g_rec_mutex_lock(&mutex);
#define UNLOCK g_rec_mutex_unlock(&mutex);

void sc_memory_params_clear(sc_memory_params *params)
{
    params->clear = SC_FALSE;
    params->config_file = 0;
    params->ext_path = 0;
    params->repo_path = 0;
}

sc_memory_context* sc_memory_initialize(const sc_memory_params *params)
{
    sc_config_initialize(params->config_file);

    char *v_str = sc_version_string_new(&SC_VERSION);
    g_message("Version: %s", v_str);
    sc_version_string_free(v_str);

    g_message("Configuration:");
    g_message("\tmax_loaded_segments: %d", sc_config_get_max_loaded_segments());
    g_message("sc-element size: %d", sizeof(sc_element));

    if (sc_storage_initialize(params->repo_path, params->clear) != SC_TRUE)
        return 0;

    g_rec_mutex_init(&mutex);

    s_memory_default_ctx = sc_memory_context_new(sc_access_levels_make(16, 16));

    if (sc_helper_init(s_memory_default_ctx) != SC_RESULT_OK)
        goto error;

    if (sc_events_initialize() == SC_FALSE)
    {
        g_error("Error while initialize events module");
        goto error;
    }

    sc_result ext_res;
    ext_res = sc_ext_initialize(params->ext_path);

    switch (ext_res)
    {
    case SC_RESULT_OK:
        g_message("Modules initialization finished");
        return s_memory_default_ctx;

    case SC_RESULT_ERROR_INVALID_PARAMS:
        g_warning("Extensions directory '%s' doesn't exist", params->ext_path);
        break;

    default:
        g_warning("Unknown error while initialize extensions");
        break;
    }

    error:
    sc_memory_context_free(s_memory_default_ctx);
    return s_memory_default_ctx = 0;
}



void sc_memory_shutdown()
{
    sc_events_stop_processing();

    sc_ext_shutdown();

    sc_events_shutdown();
    sc_config_shutdown();

    sc_helper_shutdown();

    g_rec_mutex_clear(&mutex);
    sc_storage_shutdown();

    sc_memory_context_free(s_memory_default_ctx);
    s_memory_default_ctx = 0;
}

sc_memory_context* sc_memory_context_new(sc_uint8 levels)
{
    sc_memory_context *ctx = g_new0(sc_memory_context, 1);
    ctx->access_levels.value = levels;
    return ctx;
}

void sc_memory_context_free(sc_memory_context *ctx)
{
    g_free(ctx);
}

sc_bool sc_memory_is_initialized()
{
    sc_bool res;
    LOCK;
    res = sc_storage_is_initialized();
    UNLOCK;
    return res;
}

sc_bool sc_memory_is_element(sc_memory_context const * ctx, sc_addr addr)
{
    sc_bool res;
    LOCK;
    res = sc_storage_is_element(addr);
    UNLOCK;
    return res;
}

sc_result sc_memory_element_free(sc_memory_context const * ctx, sc_addr addr)
{
    sc_result res;
    LOCK;
    res = sc_storage_element_free(addr);
    UNLOCK;
    return res;
}

sc_addr sc_memory_node_new(sc_memory_context const * ctx, sc_type type)
{
    sc_addr res;
    LOCK;
    res = sc_storage_node_new(type);
    UNLOCK;
    return res;
}

sc_addr sc_memory_link_new(sc_memory_context const * ctx)
{
    sc_addr res;
    LOCK;
    res = sc_storage_link_new();
    UNLOCK;
    return res;
}

sc_addr sc_memory_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end)
{
    sc_addr res;
    LOCK;
    res = sc_storage_arc_new(type, beg, end);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type *result)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_element_type(addr, result);
    UNLOCK;
    return res;
}

sc_result sc_memory_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
    sc_result res;
    LOCK;
    res = sc_storage_change_element_subtype(addr, type);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr *result)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_arc_begin(addr, result);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr *result)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_arc_end(addr, result);
    UNLOCK;
    return res;
}

sc_result sc_memory_set_link_content(sc_memory_context const * ctx, sc_addr addr, const sc_stream *stream)
{
    sc_result res;
    LOCK;
    res = sc_storage_set_link_content(addr, stream);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream **stream)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_link_content(addr, stream);
    UNLOCK;
    return res;
}

sc_result sc_memory_find_links_with_content(sc_memory_context const * ctx, sc_stream const * stream, sc_addr **result, sc_uint32 *result_count)
{
    sc_result res;
    LOCK;
    res = sc_storage_find_links_with_content(stream, result, result_count);
    UNLOCK;
    return res;
}

sc_result sc_memory_stat(sc_memory_context const * ctx, sc_stat *stat)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_elements_stat(stat);
    UNLOCK;
    return res;
}
