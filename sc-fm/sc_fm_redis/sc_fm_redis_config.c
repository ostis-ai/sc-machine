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

#include "sc_fm_redis_config.h"
#include <glib.h>

const char str_group_redis[] = "redis";
const char str_key_redis_host[] = "host";
const char str_key_redis_port[] = "port";
const char str_key_redis_timeout[] = "timeout";

char *config_redis_host = 0;
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
    g_free(config_redis_host);
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
