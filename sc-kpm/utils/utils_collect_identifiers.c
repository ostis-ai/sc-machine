/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils_collect_identifiers.h"
#include "utils_keynodes.h"
#include "utils.h"

#include "sc_helper.h"

#include <glib.h>

const char str_group_redis[] = "redis";
const char str_key_redis_host[] = "host";
const char str_key_redis_port[] = "port";
const char str_key_redis_timeout[] = "timeout";


const char str_sys_idtf_postfix[] = "sys";
const char str_main_idtf_postfix[] = "main";
const char str_idtf_postfix[] = "common";

const char *utils_redis_host = 0;
sc_uint32 utils_redis_port = 6379;
sc_uint32 utils_redis_timeout = 1500;

GMutex ci_redis_mutex;
redisContext *redisCtx = 0;
GThread *ping_thread;
gboolean ping_thread_running;

// ---------------------
sc_event *event_add_idtf = 0;
sc_event *event_add_main_idtf = 0;
sc_event *event_add_sys_idtf = 0;



// --- ping thread ---
gpointer ping_thread_loop(gpointer context)
{
    g_mutex_lock(&ci_redis_mutex);
    ping_thread_running = TRUE;
    g_mutex_unlock(&ci_redis_mutex);

    gboolean running = TRUE;

    while (ping_thread_running)
    {

        g_mutex_lock(&ci_redis_mutex);
        redisReply *reply = redisCommand((redisContext*)context, "PING");
        freeReplyObject(reply);

        running = ping_thread_running;
        g_mutex_unlock(&ci_redis_mutex);

        // wait on second
        g_usleep(1000000);
    }

    return 0;
}


redisContext* connectToRedis()
{
    g_message("Connect to redis server %s:%d", utils_redis_host, utils_redis_port);

    sc_uint32 timeout_val = utils_redis_timeout;
    struct timeval timeout = {timeout_val / 1000, (timeout_val % 1000) * 1000};

    redisContext *c = redisConnectWithTimeout(utils_redis_host, utils_redis_port, timeout);

    if (c == 0)
    {
        g_warning("redis: Couldn't connect to server");
        return 0;
    }

    if (c != 0 && c->err)
    {
        g_warning("redis: %s", c->errstr);
        redisFree(c);
        return 0;
    }

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

redisReply* do_sync_redis_command(redisContext *context, const char *format, ...)
{
    va_list ap;
    void *reply = 0;
    int tries = 0;
    va_start(ap,format);

    g_mutex_lock(&ci_redis_mutex);
    while (reply == 0 && tries < 5)
    {
        reply = redisvCommand(context, format, ap);
        if (reply == 0)
            g_error("redis: %s", context->errstr);
        tries++;
    }
    g_mutex_unlock(&ci_redis_mutex);

    va_end(ap);
    return reply;
}

// --------------------------------------------------
sc_result agent_append_idtf(const sc_event *event, sc_addr arg)
{
    sc_addr arc, el, link, n;
    sc_stream *content = 0;
    sc_uint8 *data = 0;
    sc_uint32 data_len = 0, read_bytes = 0;
    redisReply * reply = null_ptr;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &arc) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_memory_get_arc_begin(s_default_ctx, arg, &n) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // get element
    if (sc_memory_get_arc_begin(s_default_ctx, arc, &el) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (sc_helper_check_arc(s_default_ctx, keynode_system_element, el, sc_type_arc_pos_const_perm) == SC_TRUE)
        return SC_RESULT_OK;

    // get sc-link
    if (sc_memory_get_arc_end(s_default_ctx, arc, &link) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // get content of sc-link
    if (sc_memory_get_link_content(s_default_ctx, link, &content) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // get length of data
    if (sc_stream_get_length(content, &data_len) != SC_RESULT_OK)
    {
        sc_stream_free(content);
        return SC_RESULT_ERROR;
    }

    data = g_malloc0(data_len + 1);
    if (sc_stream_read_data(content, data, data_len, &read_bytes) != SC_RESULT_OK || read_bytes != data_len)
    {
        sc_stream_free(content);
        g_free(data);
        return SC_RESULT_ERROR;

    }
    sc_result res = SC_RESULT_OK;
    sc_addr el_addr = el;

    if (SC_ADDR_IS_EQUAL(n, keynode_nrel_idtf))
        el_addr = link;

    reply = do_sync_redis_command(redisCtx, "SET idtf:%s:%s %b",
                                              SC_ADDR_IS_EQUAL(n, keynode_nrel_main_idtf) ? str_main_idtf_postfix : (SC_ADDR_IS_EQUAL(n, keynode_nrel_idtf) ? str_idtf_postfix : str_sys_idtf_postfix),
                                              data, &el_addr, sizeof(el_addr));
    if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
        res = SC_RESULT_ERROR;

    if (reply)
        freeReplyObject(reply);

    sc_stream_free(content);
    g_free(data);

    return res;
}


sc_result utils_collect_identifiers_initialize()
{
    utils_redis_host = sc_config_get_value_string(str_group_redis, str_key_redis_host);
    if (utils_redis_host == 0)
        utils_redis_host = "127.0.0.1";

    utils_redis_port = sc_config_get_value_int(str_group_redis, str_key_redis_port);
    if (utils_redis_port == 0)
        utils_redis_port = 6379;

    utils_redis_timeout = sc_config_get_value_int(str_group_redis, str_key_redis_timeout);
    if (utils_redis_timeout == 0)
        utils_redis_timeout = 1500;

    // connect to redis
    redisCtx = connectToRedis();

    if (!redisCtx)
        return SC_RESULT_ERROR;

    // initialize agents
    event_add_idtf = sc_event_new(s_default_ctx, keynode_nrel_idtf, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_append_idtf, (fDeleteCallback)0);
    if (event_add_idtf == 0)
        return SC_RESULT_ERROR;

    event_add_main_idtf = sc_event_new(s_default_ctx, keynode_nrel_main_idtf, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_append_idtf, (fDeleteCallback)0);
    if (event_add_main_idtf == 0)
        return SC_RESULT_ERROR;

    event_add_sys_idtf = sc_event_new(s_default_ctx, keynode_nrel_system_identifier, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_append_idtf, (fDeleteCallback)0);
    if (event_add_sys_idtf == 0)
        return SC_RESULT_ERROR;


    return SC_RESULT_OK;
}

sc_result utils_collect_identifiers_shutdown()
{
    if (event_add_idtf)
    {
        sc_event_destroy(event_add_idtf);
        event_add_idtf = 0;
    }

    if (event_add_main_idtf)
    {
        sc_event_destroy(event_add_main_idtf);
        event_add_main_idtf = 0;
    }

    if (event_add_sys_idtf)
    {
        sc_event_destroy(event_add_sys_idtf);
        event_add_sys_idtf = 0;
    }

    g_mutex_lock(&ci_redis_mutex);
    ping_thread_running = FALSE;
    g_mutex_unlock(&ci_redis_mutex);
    if (ping_thread)
    {
        g_thread_join(ping_thread);
        ping_thread = 0;
    }

    if (redisCtx)
        redisFree(redisCtx);

    return SC_RESULT_OK;
}
