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

#include "sc_fm_engine_private.h"
#include "sc_stream_file.h"
#include <glib.h>

gchar contents_path[MAX_PATH_LENGTH + 1];
const gchar *content_dir = "contents";


sc_uint8* sc_fs_engine_make_checksum_path(const sc_check_sum *check_sum)
{
    // calculate output string length and create it
    sc_uint div = (check_sum->len % 8 == 0) ? 8 : 4;
    sc_uint len = check_sum->len + check_sum->len / div + 1;
    sc_uint8 *result = malloc(sizeof(sc_uint8) * len);
    sc_uint idx = 0;
    sc_uint j = 0;

    g_assert(check_sum->len == 32);

    g_assert(check_sum != 0);
    g_assert(check_sum->len != 0);
    g_assert(check_sum->len % 4 == 0);

    for (idx = 0; idx < check_sum->len; idx++)
    {
        result[j++] = check_sum->data[idx];
        if ((idx + 1 ) % div == 0)
            result[j++] = '/';
    }

    g_assert(j == (len - 1));

    result[len - 1] = 0;

    return result;
}

// --- implemntation of interface ---
sc_result sc_fs_engine_create_stream(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_uint8 flags, sc_stream **stream)
{
    sc_uint8 *path = sc_fs_engine_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar data_path[MAX_PATH_LENGTH];

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(data_path, MAX_PATH_LENGTH, "%sdata", abs_path);

    // check if specified path exist
    if (g_file_test(data_path, G_FILE_TEST_EXISTS) == FALSE)
    {
        // file doesn't exist, so we need to save it
        if (g_mkdir_with_parents(abs_path, -1) < 0)
        {
            g_message("Error while creating '%s' directory", abs_path);
            free(path);
            return SC_RESULT_ERROR_IO;
        }
    }

    // write content into file
    *stream = sc_stream_file_new(data_path, flags);

    return stream != 0 ? SC_RESULT_OK : SC_RESULT_ERROR_IO;
}

sc_result sc_fs_engine_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    sc_uint8 *path = sc_fs_engine_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar addr_path[MAX_PATH_LENGTH];
    gchar *content = 0;
    gchar *content2 = 0;
    gsize content_len = 0;
    sc_uint32 addrs_num = 0;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(addr_path, MAX_PATH_LENGTH, "%saddrs", abs_path);

    // try to load existing file
    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
        {
            if (content != 0)
                free(content);
            free(path);
            return SC_RESULT_ERROR_IO;
        }
    }

    // append addr into content
    if (content == 0)
    {
        content_len = sizeof(sc_addr) + sizeof(sc_uint32);
        content = g_new0(gchar, content_len);

        addrs_num = 1;
        memcpy(content, &addrs_num, sizeof(addrs_num));
        memcpy(content + sizeof(addrs_num), &addr, sizeof(addr));
    }else
    {
        content2 = content;
        (*(sc_uint32*)content2)++;

        content = g_new0(gchar, content_len + sizeof(addr));
        memcpy(content, content2, content_len);
        memcpy(content + content_len, &addr, sizeof(addr));
        content_len += sizeof(addr);

        // old content doesn't need
        g_free(content2);
    }

    // write content to file
    if (g_file_set_contents(addr_path, content, content_len, 0) == TRUE)
    {
        g_free(content);
        free(path);

        return SC_RESULT_OK;
    }

    g_free(content);
    free(path);

    return SC_RESULT_ERROR;
}

sc_result sc_fs_engine_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    return SC_RESULT_ERROR_IO;
}

sc_result sc_fs_engine_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    sc_uint8 *path = sc_fs_engine_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar addr_path[MAX_PATH_LENGTH];
    gchar *content = 0, *content2 = 0;
    gsize content_len = 0;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(addr_path, MAX_PATH_LENGTH, "%saddrs", abs_path);

    // must be a null pointer
    g_assert(*result == 0);

    *result_count = 0;

    // try to load existing file
    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
        {
            if (content != 0)
                free(content);
            free(path);
            return SC_RESULT_ERROR_IO;
        }

    }

    // store result
    if (content == 0)
    {
        *result = 0;
        *result_count = 0;
    }else
    {
        *result_count = *((sc_uint32*)content);
        content2 = content;
        content2 += sizeof(sc_uint32);

        *result = g_new0(sc_addr, *result_count);
        memcpy(*result, content2, sizeof(sc_addr) * (*result_count));
    }

    if (content != 0)
        g_free(content);
    free(path);

    return SC_RESULT_OK;
}

sc_result sc_fs_engine_destroy_data(const sc_fm_engine *engine)
{
    return SC_RESULT_OK;
}


sc_fm_engine* initialize(const sc_char* repo_path)
{
    // initialize file system storage
    g_snprintf(contents_path, MAX_PATH_LENGTH, "%s/%s", repo_path, content_dir);

    if (!g_file_test(contents_path, G_FILE_TEST_IS_DIR))
    {
        if (g_mkdir_with_parents(contents_path, -1) < 0)
            g_error("Can't create '%s' directory.", contents_path);
    }


    sc_fm_engine *engine = g_new0(sc_fm_engine, 1);

    engine->storage_info = 0;
    engine->funcStreamCreate = &sc_fs_engine_create_stream;
    engine->funcAddrRefAppend = &sc_fs_engine_addr_ref_append;
    engine->funcAddrRefRemove = &sc_fs_engine_addr_ref_remove;
    engine->funcFind = &sc_fs_engine_find;
    engine->funcDestroyData = &sc_fs_engine_destroy_data;

    return engine;
}
