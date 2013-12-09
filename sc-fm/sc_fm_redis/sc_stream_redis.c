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

#include "sc_stream_redis.h"
#include "sc_stream_private.h"

#include <glib.h>

struct _sc_redis_handler
{
    redisContext *context;  // pointer to redis context
    char *key;  // key to read/write value
    sc_uint32 pos;  // current seek position
    sc_uint32 size;  // size of value in bytes
};

typedef struct _sc_redis_handler sc_redis_handler;


sc_result sc_stream_redis_read(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_read)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);

    return SC_RESULT_OK;
}

sc_result sc_stream_redis_write(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_written)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);
    char *data_str = g_strndup(data, length);

    redisReply *reply = redisCommand(handler->context, "APPEND %s \"%s\"", handler->key, data_str);
    g_free(data_str);

    if (reply->type != REDIS_REPLY_INTEGER)
    {
        freeReplyObject(reply);
        return SC_RESULT_ERROR_IO;
    }

    sc_uint32 new_size = reply->integer;
    *bytes_written = new_size - handler->size;
    handler->size = new_size;
    handler->pos = handler->size;

    freeReplyObject(reply);

    return SC_RESULT_OK;
}

sc_result sc_stream_redis_seek(const sc_stream *stream, sc_stream_seek_origin origin, sc_uint32 offset)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);

    switch (origin)
    {
    case SC_STREAM_SEEK_END:
        if (offset > handler->size)
            return SC_RESULT_ERROR_INVALID_PARAMS;
        handler->pos = handler->size - offset;
        break;

    case SC_STREAM_SEEK_CUR:
        if (offset > (handler->size - handler->pos))
            return SC_RESULT_ERROR_INVALID_PARAMS;
        handler->pos += offset;
        break;

    case SC_STREAM_SEEK_SET:
        if (offset > handler->size)
            return SC_RESULT_ERROR_INVALID_PARAMS;
        handler->pos = offset;
        break;
    };

    return SC_RESULT_OK;
}

sc_result sc_stream_redis_tell(const sc_stream *stream, sc_uint32 *position)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);

    *position = handler->pos;

    return SC_RESULT_OK;
}

sc_result sc_stream_redis_free_handler(const sc_stream *stream)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);

    g_free(handler);

    return SC_RESULT_OK;
}

sc_bool sc_stream_redis_eof(const sc_stream *stream)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);

    if (handler->pos == handler->size)
        return SC_TRUE;

    return SC_FALSE;
}


sc_stream* sc_stream_redis_new(redisContext *context, const sc_char *key, sc_uint8 flags)
{
    sc_stream *stream = 0;
    sc_redis_handler *handler = g_new0(sc_redis_handler, 1);

    handler->context = context;
    handler->key = g_strdup(key);
    handler->pos = 0;
    handler->size = 0;

    // determine size
    if (flags | SC_STREAM_READ)
    {
        redisReply *reply = redisCommand(handler->context, "STRLEN %s", key);
        if (reply->type != REDIS_REPLY_INTEGER)
        {
            freeReplyObject(reply);
            g_free(handler);
            return 0;
        }

        handler->size = reply->integer;
        freeReplyObject(reply);
    }

    stream = g_new0(sc_stream, 1);

    stream->flags = flags | SC_STREAM_TELL;
    stream->handler = (void*)handler;

    stream->read_func = &sc_stream_redis_read;
    stream->write_func = &sc_stream_redis_write;
    stream->seek_func = &sc_stream_redis_seek;
    stream->tell_func = &sc_stream_redis_tell;
    stream->free_func = &sc_stream_redis_free_handler;
    stream->eof_func = &sc_stream_redis_eof;

    return stream;
}
