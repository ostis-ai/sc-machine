/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_link_helpers.h"
#include "sc_element.h"
#include "sc_stream_memory.h"

#include <stdlib.h>
#include <memory.h>
#include <glib.h>

#define SC_DEFAULT_CHECKSUM G_CHECKSUM_SHA256

sc_bool sc_link_calculate_checksum(const sc_stream *stream, sc_check_sum *check_sum)
{
    sc_char buffer[1024];
    sc_uint32 data_read;
    const gchar *result = 0;
    GChecksum *checksum = g_checksum_new(SC_DEFAULT_CHECKSUM);

    g_assert(stream != 0);
    g_assert(check_sum != 0);
    g_checksum_reset(checksum);

    sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

    while (sc_stream_eof(stream) == SC_FALSE)
    {
        if (sc_stream_read_data(stream, buffer, 1024, &data_read) == SC_RESULT_ERROR)
        {
            g_checksum_free(checksum);
            return SC_FALSE;
        }

        g_checksum_update(checksum, (guchar*)buffer, data_read);
    }

    // store results
    check_sum->len = (sc_uint8)g_checksum_type_get_length(SC_DEFAULT_CHECKSUM);
    g_assert(check_sum->len == SC_CHECKSUM_LEN);

    result = g_checksum_get_string(checksum);
    memcpy(&(check_sum->data[0]), result, check_sum->len);

    g_checksum_free(checksum);

    sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

    return SC_TRUE;
}

sc_bool sc_link_self_container_calculate_checksum(sc_element *el, sc_check_sum *sum)
{
    sc_stream *stream = sc_stream_memory_new(&el->content.data[1], el->content.data[0], SC_STREAM_FLAG_READ, SC_FALSE);
    sc_bool r = sc_link_calculate_checksum(stream, sum);
    sc_stream_free(stream);
    return r;
}



