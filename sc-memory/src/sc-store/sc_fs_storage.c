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

#include "sc_fs_storage.h"
#include "sc_segment.h"
#include "sc_stream_file.h"
#include "sc_config.h"
#include "sc_fm_engine.h"

#include <stdlib.h>
#include <memory.h>
#include <glib.h>
#include <gmodule.h>

gchar *repo_path = 0;
gchar segments_path[MAX_PATH_LENGTH + 1];
sc_fm_engine *fm_engine = 0;
#define SC_DIR_PERMISSIONS -1

const gchar *seg_dir = "segments";
const gchar *addr_key_group = "addrs";

typedef sc_fm_engine* (*fFmEngineInitFunc)();

// ----------------------------------------------

sc_bool sc_fs_storage_initialize(const gchar *path, sc_bool clear)
{
    g_message("Initialize sc-storage from path: %s", path);
    g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/%s", path, seg_dir);
    repo_path = g_strdup(path);

    g_message("\tFile memory engine: %s", sc_config_fm_engine());
    // load engine extension
    gchar module_path[MAX_PATH_LENGTH + 1];
#ifdef WIN32
    g_snprintf(module_path, MAX_PATH_LENGTH, "sc-fm-%s.dll", sc_config_fm_engine());
#else
    g_snprintf(module_path, MAX_PATH_LENGTH, "libsc-fm-%s.so", sc_config_fm_engine());
#endif

    // try to load engine extension
    GModule *module = 0;
    fFmEngineInitFunc func;
    module = g_module_open(module_path, G_MODULE_BIND_LOCAL);

    // skip non module files
    if (g_str_has_suffix(module_path, G_MODULE_SUFFIX) == TRUE)
    {
        if (module == nullptr)
        {
            g_critical("Can't load module: %s. Error: %s", module_path, g_module_error());
        }else
        {
            g_message("Initialize file memory engine from: %s", module_path);
            if (g_module_symbol(module, "initialize", (gpointer*) &func) == FALSE)
            {
                g_critical("Can't find 'initialize' symbol in module: %s", module_path);
            }else
            {
                fm_engine = func(repo_path);
                if (fm_engine == 0)
                {
                    g_critical("Can't create file memory engine from: %s", module_path);
                    return SC_FALSE;
                }
            }
        }
    }

    // clear repository if needs
    if (clear == SC_TRUE)
    {
        g_message("Clear file memory");
        if (g_file_test(segments_path, G_FILE_TEST_IS_DIR))
        {
            // remove all segments
            GDir *dir = 0;
            const gchar *fname = 0;
            char path[MAX_PATH_LENGTH];

            dir = g_dir_open(segments_path, 0, 0);
            g_assert( dir != (GDir*)0 );
            // calculate files
            fname = g_dir_read_name(dir);
            while (fname != 0)
            {
                g_snprintf(path, MAX_PATH_LENGTH, "%s/%s", segments_path, fname);

                if (g_file_test(path, G_FILE_TEST_IS_REGULAR))
                {
                    if (g_remove(path) == -1)
                    {
                        g_critical("Can't remove segment: %s", path);
                        g_dir_close(dir);
                        return SC_FALSE;
                    }
                }
                fname = g_dir_read_name(dir);
            }

            g_dir_close(dir);
        }

        if (sc_fm_clear(fm_engine) != SC_RESULT_OK)
        {
            g_critical("Can't clear file memory");
            return SC_FALSE;
        }
    }


    /*g_message("Connect to redis server %s:%d", sc_config_redis_host(), sc_config_redis_port());
    sc_uint32 timeout_val = sc_config_redis_timeout();
    struct timeval timeout = {timeout_val / 1000, (timeout_val % 1000) * 1000};
    redis_context = redisConnectWithTimeout(sc_config_redis_host(), sc_config_redis_port(), timeout);

    if (redis_context == 0 || redis_context->err)
    {
        if (redis_context)
        {
            g_message("Connection error: %s", redis_context->errstr);
            redisFree(redis_context);
        } else {
            g_message("Connection error: can't allocate redis context");
        }
        return SC_FALSE;
    }

    g_message("\tSwitch redis database");
    redisReply *reply = redisCommand(redis_context, "SELECT 0");
    if (reply->type == REDIS_REPLY_ERROR)
    {
        g_message("\tCan't switch database");
        freeReplyObject(reply);
        redisFree(redis_context);
        return SC_FALSE;
    }
    freeReplyObject(reply);*/

    return SC_TRUE;
}

sc_bool sc_fs_storage_shutdown(sc_segment **segments)
{    
    g_message("Shutdown sc-storage");

//    g_message("Disconnect from redis");
//    redisFree(redis_context);

    g_message("Write storage into %s", repo_path);
    sc_fs_storage_write_to_path(segments);

    g_free(repo_path);

    return SC_TRUE;
}

void _get_segment_path(const gchar *path, 
                       sc_uint id,
                       sc_uint max_path_len,
                       gchar *res)
{
    g_snprintf(res, max_path_len, "%s/%.10d", path, id);
}

sc_segment* sc_fs_storage_load_segment(sc_uint id)
{
    sc_segment *segment = sc_segment_new(id);
    gchar file_name[MAX_PATH_LENGTH + 1];
    gboolean res;
    sc_uint length;

    _get_segment_path(segments_path, id, MAX_PATH_LENGTH, file_name);
    res = g_file_get_contents(file_name, (gchar**)(&segment), &length, 0);
    g_assert( res );
    g_assert( length == sizeof(sc_segment) );

    return segment;
}

sc_bool sc_fs_storage_read_from_path(sc_segment **segments, sc_uint16 *segments_num)
{
    const gchar *fname = 0;
    sc_uint files_count = 0, idx, to_load;
    GDir *dir = 0;

    if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
    {
        g_error("%s isn't a directory.", repo_path);
        return SC_FALSE;
    }

    if (!g_file_test(segments_path, G_FILE_TEST_IS_DIR))
    {
        g_message("There are no segments directory in %s", repo_path);
        return SC_FALSE;
    }

    dir = g_dir_open(segments_path, 0, 0);
    g_assert( dir != (GDir*)0 );

    // calculate files
    fname = g_dir_read_name(dir);
    while (fname != 0)
    {
        if (!g_file_test(fname, G_FILE_TEST_IS_DIR))
            files_count++;
        fname = g_dir_read_name(dir);
    }

    g_message("Segments found: %u", files_count);
    *segments_num = files_count;

    to_load = MIN(files_count, sc_config_get_max_loaded_segments());
    // load segments
    for (idx = 0; idx < to_load; idx++)
        segments[idx] = (gpointer)sc_fs_storage_load_segment(idx);

    g_message("Segments loaded: %u", idx);

    g_dir_close(dir);
    return SC_TRUE;
}

sc_bool sc_fs_storage_write_to_path(sc_segment **segments)
{
    sc_uint idx = 0;
    const sc_segment *segment = 0;
    gchar file_name[MAX_PATH_LENGTH + 1];
    gchar segments_path[MAX_PATH_LENGTH + 1];

    if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
    {
        g_error("%s isn't a directory.", repo_path);
        return SC_FALSE;
    }

    // check if segments directory exists, if it doesn't, then create one
    g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/%s", repo_path, seg_dir);
    if (!g_file_test(segments_path, G_FILE_TEST_IS_DIR))
    {
        if (g_mkdir_with_parents(segments_path, SC_DIR_PERMISSIONS) < 0)
            return SC_FALSE;
    }

    for (idx = 0; idx < SC_ADDR_SEG_MAX; idx++)
    {
        segment = segments[idx];
        if (segment == nullptr) continue; // skip null segments

        _get_segment_path(segments_path, idx, MAX_PATH_LENGTH, file_name);
        g_file_set_contents(file_name, (gchar*)segment, sizeof(sc_segment), 0);
    }

    _get_segment_path(segments_path, idx, MAX_PATH_LENGTH, file_name);
    while (g_file_test(file_name, G_FILE_TEST_EXISTS))
    {
        g_remove(file_name);
        _get_segment_path(segments_path, ++idx, MAX_PATH_LENGTH, file_name);
    }


    return SC_TRUE;
}


sc_result sc_fs_storage_write_content(sc_addr addr, const sc_check_sum *check_sum, const sc_stream *stream)
{
    // write content into file
    sc_char buffer[1024];
    sc_uint32 data_read, data_write;
    sc_stream *out_stream = 0;

    if (sc_fm_stream_new(fm_engine, check_sum, SC_STREAM_WRITE, &out_stream) == SC_RESULT_OK)
    {
        g_assert(out_stream != 0);
        // reset input stream positon to begin
        sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

        while (sc_stream_eof(stream) == SC_FALSE)
        {
            if (sc_stream_read_data(stream, buffer, 1024, &data_read) == SC_RESULT_ERROR)
            {
                sc_stream_free(out_stream);
                return SC_RESULT_ERROR;
            }

            if (sc_stream_write_data(out_stream, buffer, data_read, &data_write) == SC_RESULT_ERROR)
            {
                sc_stream_free(out_stream);
                return SC_RESULT_ERROR;
            }

            if (data_read != data_write)
            {
                sc_stream_free(out_stream);
                return SC_RESULT_ERROR;
            }
        }
        sc_stream_free(out_stream);

        return sc_fs_storage_add_content_addr(addr, check_sum);
    }

    return SC_RESULT_ERROR_IO;
}

sc_result sc_fs_storage_add_content_addr(sc_addr addr, const sc_check_sum *check_sum)
{
    g_assert(fm_engine != 0);
    return sc_fm_addr_ref_append(fm_engine, addr, check_sum);
}

sc_result sc_fs_storage_find_links_with_content(const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    g_assert(fm_engine != 0);
    return sc_fm_find(fm_engine, check_sum, result, result_count);
}

sc_result sc_fs_storage_get_checksum_content(const sc_check_sum *check_sum, sc_stream **stream)
{
    g_assert(fm_engine != 0);
    return sc_fm_stream_new(fm_engine, check_sum, SC_STREAM_READ, stream);
}




