/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fm_redis_config.h"
#include <glib.h>

const char str_group_redis[] = "redis";
const char str_key_redis_host[] = "host";
const char str_key_redis_port[] = "port";
const char str_key_redis_timeout[] = "timeout";

const char *config_redis_host = 0;
sc_uint32 config_redis_port = 6379;
sc_uint32 config_redis_timeout = 1500;


void sc_redis_config_initialize()
{
    config_redis_host = sc_config_get_value_string(str_group_redis, str_key_redis_host);
    if (config_redis_host == 0)
        config_redis_host = g_strdup_printf("127.0.0.1");
    else
        config_redis_host = g_strdup(config_redis_host);

    config_redis_port = sc_config_get_value_int(str_group_redis, str_key_redis_port);
    if (config_redis_port == 0)
        config_redis_port = 6379;

    config_redis_timeout = sc_config_get_value_int(str_group_redis, str_key_redis_timeout);
    if (config_redis_timeout == 0)
        config_redis_timeout = 1500;
}

void sc_redis_config_shutdown()
{

}

const sc_uint8* sc_redis_config_host()
{
    return config_redis_host;
}

sc_uint32 sc_redis_config_port()
{
    return config_redis_port;
}

sc_uint32 sc_redis_config_timeout()
{
    return config_redis_timeout;
}
