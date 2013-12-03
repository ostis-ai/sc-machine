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

#include "sc_config.h"

#include <glib.h>

const char str_group_memory[] = "memory";
const char str_group_redis[] = "redis";
const char str_group_fm[] = "filememory";

const char str_key_max_loaded_segments[] = "max_loaded_segments";
const char str_key_redis_host[] = "host";
const char str_key_redis_port[] = "port";
const char str_key_redis_timeout[] = "timeout";
const char str_key_fm_engine[] = "engine";


// Maximum number of segments, that can be loaded into memory at one moment
sc_uint config_max_loaded_segments = G_MAXUINT16;

const char redis_hostname_default[] = "localhost";
// Redis host name
char *config_redis_host = redis_hostname_default;
// Redis port
sc_uint32 config_redis_port = 6379;
// Redis timeout
sc_uint32 config_redis_timeout = 1500;

// --- file memory ---
const char fm_default_engine[] = "filesystem";
char *config_fm_engine = fm_default_engine;

void sc_config_initialize(const sc_char *file_path)
{
    GKeyFile *key_file = 0;

    key_file = g_key_file_new();
    if ((file_path != nullptr) && (g_key_file_load_from_file(key_file, file_path, G_KEY_FILE_NONE, 0) == TRUE))
    {
        // parse settings
        if (g_key_file_has_key(key_file, str_group_memory, str_key_max_loaded_segments, 0) == TRUE)
            config_max_loaded_segments = g_key_file_get_integer(key_file, str_group_memory, str_key_max_loaded_segments, 0);

        // redis
        if (g_key_file_has_key(key_file, str_group_redis, str_key_redis_host, 0) == TRUE)
            config_redis_host = g_key_file_get_string(key_file, str_group_redis, str_key_redis_host, 0);
        if (g_key_file_has_key(key_file, str_group_redis, str_key_redis_port, 0) == TRUE)
            config_redis_port = g_key_file_get_integer(key_file, str_group_redis, str_key_redis_port, 0);
        if (g_key_file_has_key(key_file, str_group_redis, str_key_redis_timeout, 0) == TRUE)
            config_redis_timeout = g_key_file_get_integer(key_file, str_group_redis, str_key_redis_timeout, 0);

        // file memory
        if (g_key_file_has_key(key_file, str_group_fm, str_key_fm_engine, 0) == TRUE)
            config_fm_engine = g_key_file_get_string(key_file, str_group_fm, str_key_fm_engine, 0);
    }else
    {
        // setup default values
        config_max_loaded_segments = G_MAXUINT16;
    }

    g_key_file_free(key_file);
}

void sc_config_shutdown()
{

}

sc_uint sc_config_get_max_loaded_segments()
{
    return config_max_loaded_segments;
}

const sc_char* sc_config_redis_host()
{
    return config_redis_host;
}

int sc_config_redis_port()
{
    return config_redis_port;
}

sc_uint32 sc_config_redis_timeout()
{
    return config_redis_timeout;
}

const sc_char* sc_config_fm_engine()
{
    return config_fm_engine;
}
