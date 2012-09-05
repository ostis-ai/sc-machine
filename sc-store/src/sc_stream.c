#include "sc_stream.h"
#include "sc_stream_private.h"

#include <glib.h>


sc_result sc_stream_read_data(sc_stream *stream, sc_char *data, sc_uint32 data_len, sc_uint32 *read_bytes)
{
    g_assert(stream != 0);

    if (stream->read_func == 0)
    {
        g_message("Null pointer to read function in stream");
        return SC_ERROR;
    }

    if (sc_stream_check_flag(stream, SC_STREAM_READ) == SC_FALSE)
    {
        g_message("Stream doesn't support SC_STREAM_READ flag");
        return SC_ERROR;
    }

    return stream->read_func(stream, data, data_len, read_bytes);
}



sc_result sc_stream_get_length(sc_stream *stream, sc_uint32 *length)
{
    sc_uint32 old_pos = 0;
    sc_result res = SC_ERROR;

    g_assert(stream != 0);

    if (stream->tell_func == 0 || stream->seek_func == 0)
    {
        g_message("Can't calculate stream size. There are no tell or seek function");
        return SC_ERROR;
    }

    if (sc_stream_check_flag(stream, SC_STREAM_SEEK | SC_STREAM_TELL) == FALSE)
    {
        g_message("Stream doesn't support SC_STREAM_SEEK or SC_STREAM_TELL");
        return SC_ERROR;
    }

    if (stream->tell_func(stream, &old_pos) == SC_ERROR)
        return SC_ERROR;

    // seek to the end of stream
    if (stream->seek_func(stream, SC_STREAM_SEEK_END, 0) == SC_ERROR)
    {
        // try to return stream position back
        stream->seek_func(stream, SC_STREAM_SEEK_SET, old_pos);
        return SC_ERROR;
    }

    // get end stream position (stream length)
    res = stream->tell_func(stream, length);
    // return stream position back
    stream->seek_func(stream, SC_STREAM_SEEK_SET, old_pos);

    return res;
}

sc_result sc_stream_get_position(sc_stream *stream, sc_uint32 *position)
{
    g_assert(stream != 0);

    if (stream->tell_func == 0)
    {
        g_message("Tell function is null");
        return SC_ERROR;
    }

    return stream->tell_func(stream, position);
}


sc_bool sc_stream_check_flag(sc_stream *stream, sc_uint8 flag)
{
    g_assert(stream != 0);
    return (stream->flags & flag) ? SC_TRUE : SC_FALSE;
}
