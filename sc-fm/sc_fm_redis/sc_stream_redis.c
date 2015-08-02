/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_stream_redis.h"
#include "sc_stream_private.h"
#include "sc_fm_redis.h"

#include <glib.h>

struct _sc_redis_handler
{
    redisContext *context;  // pointer to redis context
    char *key;  // key to read/write value
    sc_uint32 pos;  // current seek position
    sc_uint32 size;  // size of value in bytes
    GMutex *mutex;
};

typedef struct _sc_redis_handler sc_redis_handler;


sc_result sc_stream_redis_read(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_read)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    redisReply * reply = null_ptr;
    g_assert(handler != 0);

    if (handler->size == 0)
        return SC_RESULT_ERROR;

    reply = do_sync_redis_command(&handler->context, "GETRANGE %s %d %d", handler->key, handler->pos, handler->pos + length - 1);
    if (reply->type != REDIS_REPLY_STRING)
    {
        freeReplyObject(reply);
        return SC_RESULT_ERROR_IO;
    }

    *bytes_read = reply->len;

    memcpy(data, reply->str, reply->len);
    handler->pos += reply->len;

    freeReplyObject(reply);

    g_assert(handler->pos <= handler->size);

    return SC_RESULT_OK;
}

sc_result sc_stream_redis_write(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_written)
{
    sc_redis_handler *handler = (sc_redis_handler*)stream->handler;
    g_assert(handler != 0);

    redisReply *reply = do_sync_redis_command(&handler->context, "APPEND %s %b", handler->key, data, (size_t)length);
    if (reply->type != REDIS_REPLY_INTEGER)
    {
        freeReplyObject(reply);
        return SC_RESULT_ERROR_IO;
    }

    sc_uint32 new_size = (sc_uint32)reply->integer;
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

    g_free(handler->key);

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


sc_stream* sc_stream_redis_new(redisContext *context, const sc_char *key, sc_uint8 flags, GMutex *mutex)
{
    sc_stream *stream = 0;
    sc_redis_handler *handler = g_new0(sc_redis_handler, 1);

    handler->context = context;
    handler->key = g_strdup(key);
    handler->pos = 0;
    handler->size = 0;
    handler->mutex = mutex;

    // determine size
    if (flags & SC_STREAM_FLAG_READ)
    {
        redisReply *reply = do_sync_redis_command(&handler->context, "STRLEN %s", key);
        if (reply->type != REDIS_REPLY_INTEGER || (reply->type == REDIS_REPLY_INTEGER && reply->integer == 0))
        {
            freeReplyObject(reply);
            g_free(handler);
            return 0;
        }

        handler->size = (sc_uint32)reply->integer;
        freeReplyObject(reply);
    } else
    {
        if (flags & SC_STREAM_FLAG_WRITE)
        {
            redisReply *reply = do_sync_redis_command(&handler->context, "DEL %s", key);
            freeReplyObject(reply);
        }
    }

    stream = g_new0(sc_stream, 1);

    stream->flags = flags | SC_STREAM_FLAG_TELL;
    stream->handler = (void*)handler;

    stream->read_func = &sc_stream_redis_read;
    stream->write_func = &sc_stream_redis_write;
    stream->seek_func = &sc_stream_redis_seek;
    stream->tell_func = &sc_stream_redis_tell;
    stream->free_func = &sc_stream_redis_free_handler;
    stream->eof_func = &sc_stream_redis_eof;

    g_assert(stream->handler != 0);
    return stream;
}
