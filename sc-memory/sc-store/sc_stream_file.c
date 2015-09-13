/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_stream_file.h"
#include "sc_stream_private.h"

#include <stdio.h>
#include <glib.h>

#define SC_STREAM_FILE_FD_CHECK \
    FILE *fd = (FILE*)stream->handler; \
    g_assert(stream != 0); \
    if (fd == 0) \
    { \
        g_message("File handler is null"); \
        return SC_RESULT_ERROR; \
    }

sc_result sc_stream_file_read(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_read)
{
    SC_STREAM_FILE_FD_CHECK;

    *bytes_read = (sc_uint32)fread(data, 1, length, fd);
    return SC_RESULT_OK;
}

sc_result sc_stream_file_write(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_written)
{
    SC_STREAM_FILE_FD_CHECK;

    if (bytes_written)
        *bytes_written = (sc_uint32)fwrite(data, 1, length, fd);
    else
        fwrite(data, 1, length, fd);

    return SC_RESULT_OK;
}

sc_result sc_stream_file_seek(const sc_stream *stream, sc_stream_seek_origin origin, sc_uint32 offset)
{
    int whence = 0;
    SC_STREAM_FILE_FD_CHECK;

    switch (origin)
    {
    case SC_STREAM_SEEK_SET:
        whence = SEEK_SET;
        break;

    case SC_STREAM_SEEK_CUR:
        whence = SEEK_CUR;
        break;

    case SC_STREAM_SEEK_END:
        whence = SEEK_END;
        break;
    };

    if (fseek(fd, offset, whence) == 0)
        return SC_RESULT_OK;

    return SC_RESULT_ERROR;
}

sc_result sc_stream_file_tell(const sc_stream *stream, sc_uint32 *position)
{
    SC_STREAM_FILE_FD_CHECK;
    *position = ftell(fd);
    return SC_RESULT_OK;
}

sc_result sc_stream_file_free_handler(const sc_stream *stream)
{
    SC_STREAM_FILE_FD_CHECK;
    if (fclose(fd) == 0)
        return SC_RESULT_OK;

    return SC_RESULT_ERROR;
}

sc_bool sc_stream_file_eof(const sc_stream *stream)
{
    SC_STREAM_FILE_FD_CHECK;

    if (feof(fd) == 0)
        return SC_FALSE;

    return SC_TRUE;
}


sc_stream* sc_stream_file_new(const sc_char *file_name, sc_uint8 flags)
{
    sc_stream *stream = 0;
    FILE *fd = 0;

    if (flags & SC_STREAM_FLAG_READ)
    {
        g_assert(!(flags & SC_STREAM_FLAG_APPEND)); // couldn't support append in read mode
        fd = fopen(file_name, "rb");
    }
    else
    {
        if (flags & SC_STREAM_FLAG_WRITE)
        {
            if (flags & SC_STREAM_FLAG_APPEND)
                fd = fopen(file_name, "wb+");
            else
                fd = fopen(file_name, "wb");
        }
    }

    // if file not opened, then return null
    if (fd == 0)
        return 0;

    stream = g_new0(sc_stream, 1);

    stream->flags = flags | SC_STREAM_FLAG_TELL | SC_STREAM_FLAG_SEEK;
    stream->handler = (void*)fd;

    stream->read_func = &sc_stream_file_read;
    stream->write_func = &sc_stream_file_write;
    stream->seek_func = &sc_stream_file_seek;
    stream->tell_func = &sc_stream_file_tell;
    stream->free_func = &sc_stream_file_free_handler;
    stream->eof_func = &sc_stream_file_eof;

    return stream;
}
