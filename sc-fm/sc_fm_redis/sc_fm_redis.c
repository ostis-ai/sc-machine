/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

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

#include "sc_fm_redis.h"
#include "sc_fm_engine_private.h"
#include "sc_stream_redis.h"
#include "sc_fm_redis_config.h"
#include <glib.h>
#include <memory.h>

GMutex redis_mutex;

GThread *ping_thread;
gboolean ping_thread_running;

struct _redis_data
{
    redisContext *context;
};

typedef struct _redis_data redis_data;


// --- ping thread ---
gpointer ping_thread_loop(gpointer context)
{
    g_mutex_lock(&redis_mutex);
    ping_thread_running = TRUE;
    g_mutex_unlock(&redis_mutex);

    gboolean running = TRUE;

    while (ping_thread_running)
    {

        g_mutex_lock(&redis_mutex);
        redisReply *reply = redisCommand((redisContext*)context, "PING");
        freeReplyObject(reply);

        running = ping_thread_running;
        g_mutex_unlock(&redis_mutex);

        // wait on second
        g_usleep(1000000);
    }

    return 0;
}


redisContext* connectToRedis()
{
    g_message("Connect to redis server %s:%d", sc_redis_config_host(), sc_redis_config_port());

    sc_uint32 timeout_val = sc_redis_config_timeout();
    struct timeval timeout = {timeout_val / 1000, (timeout_val % 1000) * 1000};

    redisContext *c = redisConnectWithTimeout(sc_redis_config_host(), sc_redis_config_port(), timeout);

    if (c == 0)
    {
        g_error("redis: Couldn't connect to server");
        return 0;
    }

    if (c != 0 && c->err)
    {
        g_error("redis: %s", c->errstr);
        redisFree(c);
        return 0;
    }

    g_message("\tSwitch redis database");

    redisReply *reply = redisCommand(c, "SELECT 0");
    if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
    {
        g_error("\tCan't switch database");
        freeReplyObject(reply);
        redisFree(c);
        return 0;
    }
    freeReplyObject(reply);

    // start ping thread

    ping_thread = g_thread_new("redis_ping_thread", ping_thread_loop, c);



    return c;
}


redisReply* do_sync_redis_command(redisContext **context, const char *format, ...)
{
    va_list ap;
    void *reply = 0;
    int tries = 0, context_tries = 0;
    va_start(ap,format);

    g_mutex_lock(&redis_mutex);
    while (reply == 0 && tries < 5)
    {
        reply = redisvCommand(*context, format, ap);
        if (reply == 0)
            g_error("redis: %s", (*context)->errstr);
        tries++;
    }
    g_mutex_unlock(&redis_mutex);

    va_end(ap);
    return reply;
}

sc_result sc_redis_engine_create_stream(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_uint8 flags, sc_stream **stream)
{
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    char key[128];
    char *check_sum_str = g_strndup(check_sum->data, check_sum->len);
    g_snprintf(key, 128, "link:%s:data", check_sum_str);
    g_free(check_sum_str);

    *stream = 0;
    *stream = sc_stream_redis_new(data->context, key, flags, &redis_mutex);

    return *stream == 0 ? SC_RESULT_ERROR : SC_RESULT_OK;
}

sc_result sc_redis_engine_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "LLEN link:%b:addrs", check_sum->data, check_sum->len);
    if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
    {
        freeReplyObject(reply);
        return SC_RESULT_ERROR;
    }

    int old_size = reply->integer;
    reply = do_sync_redis_command(&data->context, "LPUSH link:%b:addrs %b", check_sum->data, check_sum->len, &addr, sizeof(addr));

    if (reply == 0)
        return SC_RESULT_ERROR;

    if ((reply->type == REDIS_REPLY_INTEGER) && (old_size < reply->integer))
    {
        freeReplyObject(reply);
        return SC_RESULT_OK;
    }
    freeReplyObject(reply);


    return SC_RESULT_ERROR;
}

sc_result sc_redis_engine_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "LREM link:%b:addrs 0 %b", check_sum->data, check_sum->len, &addr, sizeof(addr));

    if (reply == 0)
        return SC_RESULT_ERROR;

    if (reply->type == REDIS_REPLY_INTEGER)
    {
        freeReplyObject(reply);
        return SC_RESULT_OK;
    }
    freeReplyObject(reply);

    return (reply->type != REDIS_REPLY_INTEGER) ? SC_RESULT_ERROR : SC_RESULT_OK;
}

sc_result sc_redis_engine_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "LRANGE link:%b:addrs 0 -1", check_sum->data, check_sum->len);
    if (reply == 0)
        return SC_RESULT_ERROR;

    if (reply->type == REDIS_REPLY_ERROR)
    {
        freeReplyObject(reply);
        return SC_RESULT_ERROR;
    }

    if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements > 0))
    {
        *result_count = reply->elements;
        *result = g_new0(sc_addr, *result_count);
        size_t i;
        for (i = 0; i < reply->elements; ++i)
            memcpy(&((*result)[i]), reply->element[i]->str, sizeof(sc_addr));

        freeReplyObject(reply);
        return SC_RESULT_OK;
    }
    freeReplyObject(reply);

    return SC_RESULT_ERROR;
}

sc_result sc_redis_engine_clear(const sc_fm_engine *engine)
{
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "FLUSHDB");

    if (reply == 0)
        return SC_RESULT_ERROR;

    freeReplyObject(reply);

    return SC_RESULT_OK;
}

sc_result sc_redis_engine_save(const sc_fm_engine *engine)
{
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "SAVE");

    sc_result result = (reply == 0 || reply->type == REDIS_REPLY_ERROR) ? SC_RESULT_ERROR : SC_RESULT_OK;
    freeReplyObject(reply);

    return result;
}

sc_result sc_redis_engine_destroy_data(const sc_fm_engine *engine)
{
    redis_data *data = (redis_data*)engine->storage_info;

    g_assert(data);

    g_mutex_lock(&redis_mutex);
    ping_thread_running = FALSE;
    g_mutex_unlock(&redis_mutex);
    g_thread_join(ping_thread);
    ping_thread = 0;

    redisFree(data->context);
    g_free(data);

    sc_redis_config_shutdown();

    return SC_RESULT_OK;
}

sc_fm_engine* initialize(const sc_char* repo_path)
{
    sc_redis_config_initialize();

    redis_data *data = g_new0(redis_data, 1);

    g_mutex_init(&redis_mutex);
    data->context = connectToRedis();

    if (data->context == 0 || data->context->err)
    {
        if (data->context)
        {
            g_critical("Connection error: %s", data->context->errstr);
            redisFree(data->context);
        } else
        {
            g_critical("Connection error: can't allocate redis context");
        }

        g_free(data);

        return 0;
    }

    sc_fm_engine *engine = g_new0(sc_fm_engine, 1);

    engine->storage_info = data;
    engine->funcStreamCreate = &sc_redis_engine_create_stream;
    engine->funcAddrRefAppend = &sc_redis_engine_addr_ref_append;
    engine->funcAddrRefRemove = &sc_redis_engine_addr_ref_remove;
    engine->funcFind = &sc_redis_engine_find;
    engine->funcClear = &sc_redis_engine_clear;
    engine->funcSave = &sc_redis_engine_save;
    engine->funcDestroyData = &sc_redis_engine_destroy_data;

    return engine;
}
