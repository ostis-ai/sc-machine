#include "sc_memory.h"
#include "sc-store/sc_storage.h"

#include <glib.h>

sc_bool sc_memory_initialize(const sc_char *repo_path)
{
    return sc_storage_initialize(repo_path);
}

void sc_memory_shutdown()
{
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

void sc_memory_element_free(sc_addr addr)
{
    //! @todo make it thread-safe
    sc_storage_element_free(addr);
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
    return sc_memory_set_link_content(addr, stream);
}
