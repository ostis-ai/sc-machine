/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#include <glib.h>

sc_bool sc_memory_initialize(const sc_char *repo_path, const sc_char *ext_path)
{
    sc_bool res = sc_storage_initialize(repo_path);

    sc_result ext_res = sc_ext_initialize(ext_path);

    switch (ext_res)
    {
    case SC_RESULT_OK:
        g_message("Modules initializetion finished");
        break;
    case SC_RESULT_ERROR_INVALID_PARAMS:
        g_warning("Extensions directory '%s'' doesn't exist", ext_path);
        break;

    default:
        g_warning("Unknown error while initialize extensions");
    }

    return res;
}

void sc_memory_shutdown()
{
    sc_ext_shutdown();
    sc_storage_shutdown();
}

sc_bool sc_memory_is_initialized()
{
    //! @todo make it thread-safe
    return sc_storage_is_initialized();
}

sc_bool sc_memory_is_element(sc_addr addr)
{
    //! @todo make it thread-safe
    return sc_storage_is_element(addr);
}

sc_result sc_memory_element_free(sc_addr addr)
{
    //! @todo make it thread-safe
    return sc_storage_element_free(addr);
}

sc_addr sc_memory_node_new(sc_type type)
{
    //! @todo make it thread-safe
    return sc_storage_node_new(type);
}

sc_addr sc_memory_link_new()
{
    //! @todo make it thread-safe
    return sc_storage_link_new();
}

sc_addr sc_memory_arc_new(sc_type type, sc_addr beg, sc_addr end)
{
    //! @todo make it thread-safe
    return sc_storage_arc_new(type, beg, end);
}

sc_result sc_memory_get_element_type(sc_addr addr, sc_type *result)
{
    //! @todo make it thread-safe
    return sc_storage_get_element_type(addr, result);
}

sc_result sc_memory_get_arc_begin(sc_addr addr, sc_addr *result)
{
    //! @todo make it thread-safe
    return sc_storage_get_arc_begin(addr, result);
}

sc_result sc_memory_get_arc_end(sc_addr addr, sc_addr *result)
{
    //! @todo make it thread-safe
    return sc_storage_get_arc_end(addr, result);
}

sc_result sc_memory_set_link_content(sc_addr addr, const sc_stream *stream)
{
    //! @todo make it thread-safe
    return sc_storage_set_link_content(addr, stream);
}

sc_result sc_memory_get_link_content(sc_addr addr, sc_stream **stream)
{
    //! @todo make it thread-safe
    return sc_storage_get_link_content(addr, stream);
}

sc_result sc_memory_find_links_with_content(const sc_stream *stream, sc_addr **result, sc_uint32 *result_count)
{
    //! @todo make it thread-safe
    return sc_storage_find_links_with_content(stream, result, result_count);
}
