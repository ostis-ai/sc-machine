/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_stream.h"
#include "sc_stream_private.h"

#include <glib.h>


sc_result sc_stream_free(sc_stream *stream)
{
    g_assert(stream != 0);

    if (stream->free_func == 0)
        return SC_RESULT_ERROR;

    stream->free_func(stream);
    g_free(stream);

    return SC_RESULT_OK;
}

sc_result sc_stream_read_data(const sc_stream *stream, sc_char *data, sc_uint32 data_len, sc_uint32 *read_bytes)
{
    g_assert(stream != 0);

    if (sc_stream_check_flag(stream, SC_STREAM_FLAG_READ) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (stream->read_func == 0)
        return SC_RESULT_ERROR;

    return stream->read_func(stream, data, data_len, read_bytes);
}

sc_result sc_stream_write_data(const sc_stream *stream, sc_char *data, sc_uint32 data_len, sc_uint32 *written_bytes)
{
    g_assert(stream != 0);

    if (sc_stream_check_flag(stream, SC_STREAM_FLAG_WRITE) == SC_FALSE)
        return SC_RESULT_ERROR;

    if (stream->write_func == 0)
        return SC_RESULT_ERROR;

    return stream->write_func(stream, data, data_len, written_bytes);
}

sc_result sc_stream_seek(const sc_stream *stream, sc_stream_seek_origin seek_origin, sc_uint32 offset)
{
    g_assert(stream != 0);

    if (sc_stream_check_flag(stream, SC_STREAM_FLAG_SEEK) == FALSE)
        return SC_RESULT_ERROR;


    if (stream->seek_func == 0)
        return SC_RESULT_ERROR;

    return stream->seek_func(stream, seek_origin, offset);
}


sc_bool sc_stream_eof(const sc_stream *stream)
{
    g_assert(stream != 0);

    if (stream->eof_func == 0)
        return SC_TRUE;

    return stream->eof_func(stream);
}

sc_result sc_stream_get_length(const sc_stream *stream, sc_uint32 *length)
{
    sc_uint32 old_pos = 0;
    sc_result res = SC_RESULT_ERROR;

    g_assert(stream != 0);

    if (stream->tell_func == 0 || stream->seek_func == 0)
        return SC_RESULT_ERROR;

    if (sc_stream_check_flag(stream, SC_STREAM_FLAG_SEEK | SC_STREAM_FLAG_TELL) == FALSE)
        return SC_RESULT_ERROR;

    if (stream->tell_func(stream, &old_pos) == SC_RESULT_ERROR)
        return SC_RESULT_ERROR;

    // seek to the end of stream
    if (stream->seek_func(stream, SC_STREAM_SEEK_END, 0) == SC_RESULT_ERROR)
    {
        // try to return stream position back
        stream->seek_func(stream, SC_STREAM_SEEK_SET, old_pos);
        return SC_RESULT_ERROR;
    }

    // get end stream position (stream length)
    res = stream->tell_func(stream, length);
    // return stream position back
    stream->seek_func(stream, SC_STREAM_SEEK_SET, old_pos);

    return res;
}

sc_result sc_stream_get_position(const sc_stream *stream, sc_uint32 *position)
{
    g_assert(stream != 0);

    if (stream->tell_func == 0)
    {
        g_message("Tell function is null");
        return SC_RESULT_ERROR;
    }

    return stream->tell_func(stream, position);
}


sc_bool sc_stream_check_flag(const sc_stream *stream, sc_uint8 flag)
{
    g_assert(stream != 0);
    return (stream->flags & flag) ? SC_TRUE : SC_FALSE;
}
