/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_helper.h"
#include "sc_memory_headers.h"

#include <glib.h>
#include "string.h"

// sc-helper initialization flag
sc_bool sc_helper_is_initialized = SC_FALSE;

sc_char **keynodes_str = 0;
sc_addr *sc_keynodes = 0;

sc_result resolve_nrel_system_identifier(sc_memory_context const * ctx)
{
    sc_addr *results = 0;
    sc_uint32 results_count = 0;
    sc_stream *stream = sc_stream_memory_new(keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER],
                                             (sc_uint)(sizeof(sc_uchar) * strlen(keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER])),
                                             SC_STREAM_FLAG_READ, SC_FALSE);
    sc_uint32 i = 0;
    sc_iterator5 *it = 0;
    sc_bool found = SC_FALSE;
    sc_addr addr1, addr2;

    // try to find nrel_system_identifier strings
    if (sc_memory_find_links_with_content(ctx, stream, &results, &results_count) == SC_RESULT_OK)
    {
        for (i = 0; i < results_count; i++)
        {
            it = sc_iterator5_a_a_f_a_a_new(ctx,
                                            sc_type_node | sc_type_const | sc_type_node_norole,
                                            sc_type_arc_common | sc_type_const,
                                            results[i],
                                            sc_type_arc_pos_const_perm,
                                            sc_type_const | sc_type_node | sc_type_node_norole);

            while (sc_iterator5_next(it))
            {

                addr1 = sc_iterator5_value(it, 0);
                addr2 = sc_iterator5_value(it, 4);
                // comare begin sc-element and attribute, they must be equivalent
                if (SC_ADDR_IS_EQUAL(addr1, addr2))
                {
                    if (found == SC_FALSE)
                    {
                        sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER] = addr1;
                        found = SC_TRUE;
                    }else
                    {
                        sc_iterator5_free(it);
                        sc_stream_free(stream);
                        g_free(results);
                        g_error("There are more then one sc-elements with system identifier nrel_system_identifier ");
                        return SC_RESULT_ERROR;
                    }
                }
            }

            sc_iterator5_free(it);
        }

        g_free(results);
    }else
        return SC_RESULT_ERROR;

    sc_stream_free(stream);

    return found == SC_TRUE ? SC_RESULT_OK : SC_RESULT_ERROR;
}

void _init_keynodes_str()
{
    gsize bytes_read = 0, bytes_written = 0;
    sc_uint32 i = 0;

    keynodes_str = g_new0(gchar*, SC_KEYNODE_COUNT);
    keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER] = g_locale_to_utf8("nrel_system_identifier", -1, &bytes_read, &bytes_written, 0);


    // check for errors
    for (i = 0; i < (sc_uint32)SC_KEYNODE_COUNT; ++i)
    {
        if (keynodes_str[(sc_keynode)i] == null_ptr)
            g_error("Error to create string representation of keynode: %d", i);
    }

}

void _destroy_keynodes_str()
{
    sc_uint32 i = 0;

    for (i = 0; i < (sc_uint32)SC_KEYNODE_COUNT; ++i)
        g_free(keynodes_str[i]);

    g_free(keynodes_str);
}

sc_result sc_helper_init(sc_memory_context const * ctx)
{
    g_message("Initialize sc-helper");

    _init_keynodes_str();

    sc_keynodes = g_new0(sc_addr, SC_KEYNODE_COUNT);

    if (resolve_nrel_system_identifier(ctx) != SC_RESULT_OK)
    {
        g_message("Can't resovle nrel_system_identifier node. Create the last one");

        sc_addr addr = sc_memory_node_new(ctx, sc_type_const | sc_type_node_norole);
        sc_addr link = sc_memory_link_new(ctx);
        sc_stream *stream = sc_stream_memory_new(keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER],
                                                 (sc_uint)(sizeof(sc_uchar) * strlen(keynodes_str[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER])),
                                                 SC_STREAM_FLAG_READ, SC_FALSE);
        sc_memory_set_link_content(ctx, link, stream);
        sc_stream_free(stream);

        sc_addr arc = sc_memory_arc_new(ctx, sc_type_arc_common | sc_type_const, addr, link);
        sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, addr, arc);
        sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER] = addr;
    }

    sc_helper_is_initialized = SC_TRUE;

    return SC_RESULT_OK;
}

void sc_helper_shutdown()
{
    g_message("Shutdown sc-helper");

    g_free(sc_keynodes);
    _destroy_keynodes_str();
}

sc_result sc_helper_find_element_by_system_identifier(sc_memory_context const * ctx, const sc_char* data, sc_uint32 len, sc_addr *result_addr)
{
    sc_addr *results = 0;
    sc_uint32 results_count = 0;
    sc_stream *stream = 0;
    sc_uint32 i = 0;
    sc_iterator5 *it = 0;
    sc_bool found = SC_FALSE;

    g_assert(sc_helper_is_initialized == SC_TRUE);
    g_assert(sc_keynodes != 0);

    // try to find sc-links with that contains system identifier value
    stream = sc_stream_memory_new(data, sizeof(sc_char) * len, SC_STREAM_FLAG_READ, SC_FALSE);
    if (sc_memory_find_links_with_content(ctx, stream, &results, &results_count) == SC_RESULT_OK)
    {
        for (i = 0; i < results_count; i++)
        {
            it = sc_iterator5_a_a_f_a_f_new(ctx,
                                            0,
                                            sc_type_arc_common | sc_type_const,
                                            results[i],
                                            sc_type_arc_pos_const_perm,
                                            sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER]);
            if (sc_iterator5_next(it))
            {
                if (found == SC_FALSE)
                {
                    found = SC_TRUE;
                    *result_addr = sc_iterator5_value(it, 0);
                }else
                {
                    // don't foget to free allocated memory before return error
                    sc_iterator5_free(it);
                    sc_stream_free(stream);
                    g_free(results);
                    return SC_RESULT_ERROR_INVALID_STATE;
                }
            }

            sc_iterator5_free(it);
        }

        g_free(results);
    }

    sc_stream_free(stream);

    return found == SC_TRUE ? SC_RESULT_OK : SC_RESULT_ERROR;
}

sc_result sc_helper_set_system_identifier(sc_memory_context const * ctx, sc_addr addr, const sc_char* data, sc_uint32 len)
{
    sc_iterator5 *it5 = 0;
    sc_addr *results = 0;
    sc_uint32 results_count = 0;
    sc_stream *stream = 0;
    sc_uint32 i = 0;
    sc_addr idtf_addr, arc_addr;

    SC_ADDR_MAKE_EMPTY(idtf_addr)
    g_assert(sc_keynodes != 0);

    // check if specified system identifier already used
    stream = sc_stream_memory_new(data, sizeof(sc_char) * len, SC_STREAM_FLAG_READ, SC_FALSE);
    if (sc_memory_find_links_with_content(ctx, stream, &results, &results_count) == SC_RESULT_OK)
    {
        for (i = 0; i < results_count; i++)
        {
            it5 = sc_iterator5_a_a_f_a_f_new(ctx,
                                             0,
                                             sc_type_arc_common | sc_type_const,
                                             results[i],
                                             sc_type_arc_pos_const_perm,
                                             sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER]);
            if (sc_iterator5_next(it5))
            {
                // don't foget to free allocated memory before return error
                sc_iterator5_free(it5);
                sc_stream_free(stream);
                g_free(results);
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }

            sc_iterator5_free(it5);
        }

        g_free(results);
    }

    // if there are no elements with specified system identitifier, then we can use it
    idtf_addr = sc_memory_link_new(ctx);
    if (sc_memory_set_link_content(ctx, idtf_addr, stream) != SC_RESULT_OK)
    {
        sc_stream_free(stream);
        return SC_RESULT_ERROR;
    }

    // we doesn't need link data anymore
    sc_stream_free(stream);

    // setup new system identifier
    arc_addr = sc_memory_arc_new(ctx, sc_type_arc_common | sc_type_const, addr, idtf_addr);
    if (SC_ADDR_IS_EMPTY(arc_addr))
        return SC_RESULT_ERROR;

    arc_addr = sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER], arc_addr);
    if (SC_ADDR_IS_EMPTY(arc_addr))
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

sc_result sc_helper_get_system_identifier_link(sc_memory_context const * ctx, sc_addr el, sc_addr *sys_idtf_addr)
{
    sc_iterator5 *it = 0;
    sc_result res = SC_RESULT_ERROR;

    it = sc_iterator5_f_a_a_a_f_new(ctx,
                                    el,
                                    sc_type_arc_common | sc_type_const,
                                    sc_type_link,
                                    sc_type_arc_pos_const_perm,
                                    sc_keynodes[SC_KEYNODE_NREL_SYSTEM_IDENTIFIER]);
    g_assert(it != null_ptr);

    while (sc_iterator5_next(it) == SC_TRUE)
    {
        *sys_idtf_addr = sc_iterator5_value(it, 2);
        res = SC_RESULT_OK;
    }

    return res;
}

sc_result sc_helper_get_keynode(sc_memory_context const * ctx, sc_keynode keynode, sc_addr *keynode_addr)
{
    if ((sc_helper_is_initialized == SC_FALSE) || (sc_keynodes == null_ptr))
        return SC_RESULT_ERROR;

    *keynode_addr = sc_keynodes[(sc_uint32)keynode];

    return SC_RESULT_OK;
}

sc_bool sc_helper_resolve_system_identifier(sc_memory_context const * ctx, const char *system_idtf, sc_addr *result)
{
    gchar *keynode_idtf = 0;
    gsize bytes_written = 0;

    keynode_idtf = g_locale_to_utf8(system_idtf, -1, 0, &bytes_written, 0);
    if (keynode_idtf == null_ptr)
    {
        g_warning("Error while trying to convert %s to utf-8", system_idtf);
        return SC_FALSE;
    }

    if (sc_helper_find_element_by_system_identifier(ctx, keynode_idtf, (sc_uint32)bytes_written, result) != SC_RESULT_OK)
        return SC_FALSE;

    g_free(keynode_idtf);

    return SC_TRUE;
}

sc_bool sc_helper_check_arc(sc_memory_context const * ctx, sc_addr beg_el, sc_addr end_el, sc_type arc_type)
{
    sc_iterator3 *it = 0;
    sc_bool res = SC_FALSE;

    it = sc_iterator3_f_a_f_new(ctx, beg_el, arc_type, end_el);
    if (it == null_ptr)
        return SC_FALSE;

    if (sc_iterator3_next(it) == SC_TRUE)
        res = SC_TRUE;

    sc_iterator3_free(it);
    return res;
}



sc_bool sc_helper_check_version_equal(sc_uint8 major, sc_uint8 minor, sc_uint8 patch)
{
    sc_version v = {major, minor, patch, 0};
    return sc_version_compare(&SC_VERSION, &v) == 0 ? SC_TRUE : SC_FALSE;
}
