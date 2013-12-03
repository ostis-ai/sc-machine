/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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
#include "sc-store/sc_storage.h"
#include "sc_memory_ext.h"
#include "sc_helper.h"
#include "sc-store/sc_config.h"

#include <glib.h>

GRecMutex mutex;

#define LOCK g_rec_mutex_lock(&mutex);
#define UNLOCK g_rec_mutex_unlock(&mutex);

sc_bool sc_memory_initialize(const sc_char *repo_path, const sc_char *config_file)
{
    sc_bool res = SC_FALSE;

    sc_config_initialize(config_file);

    g_message("Run with configuration");
    g_message("\tmax_loaded_segments: %d", sc_config_get_max_loaded_segments());

    res = sc_storage_initialize(repo_path);
    g_rec_mutex_init(&mutex);

    return res;
}

sc_bool sc_memory_initialize_ext(const sc_char *path)
{
    sc_result ext_res;

    ext_res = sc_ext_initialize(path);

    switch (ext_res)
    {
    case SC_RESULT_OK:
        g_message("Modules initialization finished");
        return SC_TRUE;

    case SC_RESULT_ERROR_INVALID_PARAMS:
        g_warning("Extensions directory '%s'' doesn't exist",path);
        break;

    default:
        g_warning("Unknown error while initialize extensions");
        break;
    }

    return SC_FALSE;
}

void sc_memory_shutdown_ext()
{
    sc_ext_shutdown();
    sc_config_shutdown();
}

void sc_memory_shutdown()
{
    g_rec_mutex_clear(&mutex);
    sc_storage_shutdown();
}

sc_bool sc_memory_is_initialized()
{
    sc_bool res;
    LOCK;
    res = sc_storage_is_initialized();
    UNLOCK;
    return res;
}

sc_bool sc_memory_is_element(sc_addr addr)
{
    sc_bool res;
    LOCK;
    res = sc_storage_is_element(addr);
    UNLOCK;
    return res;
}

sc_result sc_memory_element_free(sc_addr addr)
{
    sc_result res;
    LOCK;
    res = sc_storage_element_free(addr);
    UNLOCK;
    return res;
}

sc_addr sc_memory_node_new(sc_type type)
{
    sc_addr res;
    LOCK;
    res = sc_storage_node_new(type);
    UNLOCK;
    return res;
}

sc_addr sc_memory_link_new()
{
    sc_addr res;
    LOCK;
    res = sc_storage_link_new();
    UNLOCK;
    return res;
}

sc_addr sc_memory_arc_new(sc_type type, sc_addr beg, sc_addr end)
{
    sc_addr res;
    LOCK;
    res = sc_storage_arc_new(type, beg, end);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_element_type(sc_addr addr, sc_type *result)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_element_type(addr, result);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_arc_begin(sc_addr addr, sc_addr *result)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_arc_begin(addr, result);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_arc_end(sc_addr addr, sc_addr *result)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_arc_end(addr, result);
    UNLOCK;
    return res;
}

sc_result sc_memory_set_link_content(sc_addr addr, const sc_stream *stream)
{
    sc_result res;
    LOCK;
    res = sc_storage_set_link_content(addr, stream);
    UNLOCK;
    return res;
}

sc_result sc_memory_get_link_content(sc_addr addr, sc_stream **stream)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_link_content(addr, stream);
    UNLOCK;
    return res;
}

sc_result sc_memory_find_links_with_content(const sc_stream *stream, sc_addr **result, sc_uint32 *result_count)
{
    sc_result res;
    LOCK;
    res = sc_storage_find_links_with_content(stream, result, result_count);
    UNLOCK;
    return res;
}

sc_result sc_memory_stat(sc_stat *stat)
{
    sc_result res;
    LOCK;
    res = sc_storage_get_elements_stat(stat);
    UNLOCK;
    return res;
}
