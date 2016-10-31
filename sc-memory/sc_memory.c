/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.h"
#include "sc_memory_version.h"
#include "sc_memory_private.h"
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
sc_uint16 s_context_id_last = 1;
sc_uint16 s_context_id_count = 0;
GHashTable *s_context_hash_table = 0;
GMutex s_concurrency_mutex;

void sc_memory_params_clear(sc_memory_params *params)
{
    params->clear = SC_FALSE;
    params->config_file = 0;
    params->ext_path = 0;
    params->repo_path = 0;
}

sc_memory_context* sc_memory_initialize(const sc_memory_params *params)
{
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);

    sc_config_initialize(params->config_file);

    s_context_hash_table = g_hash_table_new(g_direct_hash, g_direct_equal);

    char *v_str = sc_version_string_new(&SC_VERSION);
    g_message("Version: %s", v_str);
    sc_version_string_free(v_str);

    g_message("Configuration:");
    g_message("\tmax_loaded_segments: %d", sc_config_get_max_loaded_segments());
    g_message("sc-element size: %zd", sizeof(sc_element));

    if (sc_storage_initialize(params->repo_path, params->clear) != SC_TRUE)
        return 0;

    s_memory_default_ctx = sc_memory_context_new(sc_access_lvl_make(SC_ACCESS_LVL_MAX_VALUE, SC_ACCESS_LVL_MAX_VALUE));
    sc_memory_context *helper_ctx = sc_memory_context_new(sc_access_lvl_make(SC_ACCESS_LVL_MIN_VALUE, SC_ACCESS_LVL_MAX_VALUE));
    if (sc_helper_init(helper_ctx) != SC_RESULT_OK)
        goto error;
    sc_memory_context_free(helper_ctx);

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
    {
        if (helper_ctx)
            sc_memory_context_free(helper_ctx);
        sc_memory_context_free(s_memory_default_ctx);
    }
    return s_memory_default_ctx = 0;
}



void sc_memory_shutdown(sc_bool save_state)
{
    sc_events_stop_processing();

    sc_ext_shutdown();

    sc_events_shutdown();
    sc_config_shutdown();

    sc_helper_shutdown();

    sc_storage_shutdown(save_state);

    sc_memory_context_free(s_memory_default_ctx);
    s_memory_default_ctx = 0;

    /// todo: clear contexts
    g_hash_table_destroy(s_context_hash_table);
    s_context_hash_table = 0;
    s_context_id_last = 0;
    g_assert(s_context_id_count == 0);
}

sc_memory_context* sc_memory_context_new(sc_uint8 levels)
{
    sc_memory_context *ctx = g_new0(sc_memory_context, 1);
    sc_uint32 index = 0;

    ctx->access_levels = levels;

    // setup concurency id
    g_mutex_lock(&s_concurrency_mutex);
    if (s_context_id_count >= G_MAXUINT16)
        goto error;

    index = (s_context_id_last + 1) % G_MAXUINT16;
    while (index == 0 || (index != s_context_id_last && g_hash_table_lookup(s_context_hash_table, GINT_TO_POINTER(index))))
        index = (index + 1) % G_MAXUINT16;

    if (index != s_context_id_last)
    {
        ctx->id = index;
        s_context_id_last = index;
        g_hash_table_insert(s_context_hash_table, GINT_TO_POINTER(ctx->id), (gpointer)ctx);
    } else
        goto error;

    s_context_id_count++;
    goto result;

    error:
    {
        g_free(ctx);
        ctx = 0;
    }

    result:
    g_mutex_unlock(&s_concurrency_mutex);

    return ctx;
}

void sc_memory_context_free(sc_memory_context *ctx)
{
    g_assert(ctx != 0);

    g_mutex_lock(&s_concurrency_mutex);

    sc_memory_context *c = g_hash_table_lookup(s_context_hash_table, GINT_TO_POINTER(ctx->id));
    g_assert(c == ctx);
    g_hash_table_remove(s_context_hash_table, GINT_TO_POINTER(ctx->id));
    s_context_id_count--;

    g_mutex_unlock(&s_concurrency_mutex);

    g_free(ctx);
}

sc_bool sc_memory_is_initialized()
{
    return sc_storage_is_initialized();
}

sc_bool sc_memory_is_element(sc_memory_context const * ctx, sc_addr addr)
{
    return sc_storage_is_element(ctx, addr);
}

sc_result sc_memory_element_free(sc_memory_context const * ctx, sc_addr addr)
{
    return sc_storage_element_free(ctx, addr);
}

sc_addr sc_memory_node_new(const sc_memory_context * ctx, sc_type type)
{
    return sc_storage_node_new(ctx, type);
}

sc_addr sc_memory_link_new(sc_memory_context const * ctx)
{
    return sc_storage_link_new(ctx);
}

sc_addr sc_memory_arc_new(sc_memory_context const * ctx, sc_type type, sc_addr beg, sc_addr end)
{
    return sc_storage_arc_new(ctx, type, beg, end);
}

sc_result sc_memory_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type *result)
{
    return sc_storage_get_element_type(ctx, addr, result);
}

sc_result sc_memory_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
    return sc_storage_change_element_subtype(ctx, addr, type);
}

sc_result sc_memory_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr *result)
{
    return sc_storage_get_arc_begin(ctx, addr, result);
}

sc_result sc_memory_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr *result)
{
    return sc_storage_get_arc_end(ctx, addr, result);
}

sc_result sc_memory_get_arc_info(sc_memory_context const * ctx, sc_addr addr, 
                                 sc_addr * result_start_addr, sc_addr * result_end_addr)
{
    return sc_storage_get_arc_info(ctx, addr, result_start_addr, result_end_addr);
}

sc_result sc_memory_set_link_content(sc_memory_context const * ctx, sc_addr addr, const sc_stream *stream)
{
    return sc_storage_set_link_content(ctx, addr, stream);
}

sc_result sc_memory_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream **stream)
{
    return sc_storage_get_link_content(ctx, addr, stream);
}

sc_result sc_memory_find_links_with_content(sc_memory_context const * ctx, sc_stream const * stream, sc_addr **result, sc_uint32 *result_count)
{
    return sc_storage_find_links_with_content(ctx, stream, result, result_count);
}

void sc_memory_free_buff(sc_pointer buff)
{
    g_free(buff);
}

sc_result sc_memory_set_element_access_levels(sc_memory_context const * ctx, sc_addr addr, sc_access_levels access_levels, sc_access_levels * new_value)
{
    return sc_storage_set_access_levels(ctx, addr, access_levels, new_value);
}

sc_result sc_memory_get_element_access_levels(sc_memory_context const * ctx, sc_addr addr, sc_access_levels * result)
{
    return sc_storage_get_access_levels(ctx, addr, result);
}

sc_result sc_memory_stat(sc_memory_context const * ctx, sc_stat *stat)
{
    return sc_storage_get_elements_stat(ctx, stat);
}

sc_result sc_memory_save(sc_memory_context const * ctx)
{
    return sc_storage_save(ctx);
}
