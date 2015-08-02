/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fm_redis.h"
#include "sc_fm_engine_private.h"
#include "sc_stream_redis.h"
#include "sc_fm_redis_config.h"
#include "../sc_memory.h"

#include <glib.h>
#include <memory.h>



GMutex redis_mutex;
GMutex redis_command_mutex;

GThread *ping_thread;
gboolean ping_thread_running;

struct _redis_data
{
    redisContext *context;
};

typedef struct _redis_data redis_data;

#define LOCK_COMMAND() { g_mutex_lock(&redis_command_mutex); }
#define UNLOCK_COMMAND() { g_mutex_unlock(&redis_command_mutex); }


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
    LOCK_COMMAND();

    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    char key[128];
    char *check_sum_str = g_strndup(check_sum->data, check_sum->len);
    g_snprintf(key, 128, "link:%s:data", check_sum_str);
    g_free(check_sum_str);

    *stream = 0;
    *stream = sc_stream_redis_new(data->context, key, flags, &redis_mutex);

    UNLOCK_COMMAND();

    return *stream == 0 ? SC_RESULT_ERROR : SC_RESULT_OK;
}

sc_result sc_redis_engine_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    LOCK_COMMAND();

    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    sc_result res = SC_RESULT_OK;
    redisReply *reply = do_sync_redis_command(&data->context, "LLEN link:%b:addrs", check_sum->data, check_sum->len);
    if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
    {
        res = SC_RESULT_ERROR;
        goto clean;
    }

    reply = do_sync_redis_command(&data->context, "LPUSH link:%b:addrs %b", check_sum->data, check_sum->len, &addr, sizeof(addr));

    if (reply == 0)
    {
        res = SC_RESULT_ERROR;
        goto clean;
    }

    clean:
    {
        if (reply)
            freeReplyObject(reply);
        UNLOCK_COMMAND();
    }

    return res;
}

sc_result sc_redis_engine_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    LOCK_COMMAND();

    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "LREM link:%b:addrs 0 %b", check_sum->data, check_sum->len, &addr, sizeof(addr));
    sc_result res = SC_RESULT_ERROR;
    if (reply == 0)
        goto clean;

    if (reply->type == REDIS_REPLY_INTEGER)
    {
        res = SC_RESULT_OK;
        goto clean;
    }

    clean:
    {
        if (reply)
            freeReplyObject(reply);
        UNLOCK_COMMAND();
    }

    return res;
}

sc_result sc_redis_engine_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    LOCK_COMMAND();

    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    sc_result res = SC_RESULT_ERROR;
    redisReply *reply = do_sync_redis_command(&data->context, "LRANGE link:%b:addrs 0 -1", check_sum->data, check_sum->len);
    if (reply == 0)
        goto clean;

    if (reply->type == REDIS_REPLY_ERROR)
        goto clean;

    if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements > 0))
    {
        *result_count = (sc_uint32)reply->elements;
        *result = g_new0(sc_addr, *result_count);
        size_t i;
        for (i = 0; i < reply->elements; ++i)
            memcpy(&((*result)[i]), reply->element[i]->str, sizeof(sc_addr));

        res = SC_RESULT_OK;
        goto clean;
    } else
        res = SC_RESULT_ERROR_NOT_FOUND;

    clean:
    {
        if (reply)
            freeReplyObject(reply);
        UNLOCK_COMMAND();
    }

    return res;
}

sc_result sc_redis_engine_clear(const sc_fm_engine *engine)
{
    LOCK_COMMAND();

    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "FLUSHDB");
    sc_result res = SC_RESULT_OK;
    if (reply == 0)
    {
        res = SC_RESULT_ERROR;
        goto clean;
    }

    clean:
    {
        if (reply)
            freeReplyObject(reply);
        UNLOCK_COMMAND();
    }

    return res;
}

sc_result sc_redis_engine_save(const sc_fm_engine *engine)
{
    LOCK_COMMAND();
    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    redisReply *reply = do_sync_redis_command(&data->context, "SAVE");
    sc_result result = (reply == 0 || reply->type == REDIS_REPLY_ERROR) ? SC_RESULT_ERROR : SC_RESULT_OK;
    freeReplyObject(reply);

    UNLOCK_COMMAND();
    return result;
}

sc_result sc_redis_engine_destroy_data(const sc_fm_engine *engine)
{
    LOCK_COMMAND();

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
    UNLOCK_COMMAND();

    return SC_RESULT_OK;
}

sc_result sc_redis_engine_clean_state(const sc_fm_engine *engine)
{
    LOCK_COMMAND();

    redis_data *data = (redis_data*)engine->storage_info;
    g_assert(data);

    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make_max);

    sc_result res = SC_RESULT_OK;
    sc_uint32 cursor = 0;
    redisReply *reply = 0;
    do
    {
        reply = do_sync_redis_command(&data->context, "SCAN %d MATCH link:*:addrs COUNT 100", cursor);

        if (reply == 0 || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2)
        {
            res = SC_RESULT_ERROR;
            goto clean;
        }

        // iterate backward lists and clean them
        sc_uint32 n = (sc_uint32)reply->element[1]->elements;
        sc_uint32 i;
        for (i = 0; i < n; ++i)
        {
            const char *key = reply->element[1]->element[i]->str;
            redisReply *r = do_sync_redis_command(&data->context, "LRANGE %s 0 -1", key);

            if (r != null_ptr)
            {
                sc_uint32 j;
                for (j = 0; j < r->elements; ++j)
                {
                    sc_addr addr;
                    g_assert(sizeof(addr) == r->element[j]->len);
                    memcpy(&addr, r->element[j]->str, sizeof(addr));

                    sc_type type;
                    if (sc_memory_get_element_type(ctx, addr, &type) != SC_RESULT_OK || !(type & sc_type_link))
                    {
                        redisReply *rrem = do_sync_redis_command(&data->context, "LREM %s 0 %b", key, &addr, sizeof(addr));
                        if (rrem == null_ptr || rrem->integer != 1)
                            g_error("Error while clean %s", key);
                        else
                            freeReplyObject(rrem);
                    }
                }
                freeReplyObject(r);
            }
        }

        cursor = atoi(reply->element[0]->str);

    } while (cursor != 0);

    clean:
    {
        if (reply)
            freeReplyObject(reply);
        if (ctx)
            sc_memory_context_free(ctx);
        UNLOCK_COMMAND();
    }

    return res;
}

_SC_EXT_EXTERN sc_fm_engine* initialize(const sc_char* repo_path)
{
#if defined (SC_PLATFORM_WIN)
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    sc_redis_config_initialize();

    redis_data *data = g_new0(redis_data, 1);

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
    engine->funcCleanState = &sc_redis_engine_clean_state;

    return engine;
}

_SC_EXT_EXTERN sc_result shutdown()
{
#if defined (SC_PLATFORM_WIN)
	WSACleanup();
#endif
    return SC_RESULT_OK;
}
