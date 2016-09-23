/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fm_engine_private.h"
#include "sc_stream_file.h"

#include "../sc_memory.h"

#include <glib.h>
#include <glib/gstdio.h>

gchar contents_path[MAX_PATH_LENGTH + 1];
const gchar *content_dir = "contents";

GMutex fs_mutex;

#define LOCK_FS()   { g_mutex_lock(&fs_mutex); }
#define UNLOCK_FS() { g_mutex_unlock(&fs_mutex); }

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
    LOCK_FS();

    sc_uint8 *path = sc_fs_engine_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar data_path[MAX_PATH_LENGTH];

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(data_path, MAX_PATH_LENGTH, "%sdata", abs_path);

    sc_result res = SC_RESULT_ERROR_IO;

    // check if specified path exist
    if (g_file_test(data_path, G_FILE_TEST_EXISTS) == FALSE)
    {
        // file doesn't exist, so we need to save it
        if (g_mkdir_with_parents(abs_path, -1) < 0)
        {
            g_message("Error while creating '%s' directory", abs_path);
            goto clean;
        }
    }

    // write content into file
    *stream = sc_stream_file_new(data_path, flags);
    res = (*stream != 0) ? SC_RESULT_OK : SC_RESULT_ERROR_IO;

    clean:
    {
        free(path);
        UNLOCK_FS();
    }

    return res;
}

sc_result sc_fs_engine_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    LOCK_FS();

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

    sc_result res = SC_RESULT_ERROR_IO;

    // try to load existing file
    if (g_file_test(abs_path, G_FILE_TEST_IS_DIR) == FALSE)
    {
        if (g_mkdir_with_parents(abs_path, -1) < 0)
        {
            g_message("Error while creating '%s' directory", abs_path);
            goto clean;
        }
    }

    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
            goto clean;
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
        (*(sc_uint32*)content)++;

        content = g_new0(gchar, content_len + sizeof(addr));

        g_assert(content2 != 0);

        memcpy(content, content2, content_len);
        memcpy(content + content_len, &addr, sizeof(addr));
        content_len += sizeof(addr);

        // old content doesn't need
        g_free(content2);
    }

    // write content to file
    if (g_file_set_contents(addr_path, content, content_len, 0) == TRUE)
    {
        res = SC_RESULT_OK;
        goto clean;
    }

    clean:
    {
        if (content)
            g_free(content);
        free(path);
        UNLOCK_FS();
    }

    return res;
}

sc_result sc_fs_engine_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum)
{
    LOCK_FS();

    sc_uint8 *path = sc_fs_engine_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar addr_path[MAX_PATH_LENGTH];
    gchar *content = 0;
    gchar *content2 = 0;
    gsize content_len = 0;
    sc_addr * iter_next = null_ptr;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(addr_path, MAX_PATH_LENGTH, "%saddrs", abs_path);

    sc_result res = SC_RESULT_ERROR_IO;

    // try to load existing file
    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
            goto clean;
    }

    // remove addr from content
    if (content != 0)
    {
        g_assert(content_len % sizeof(sc_addr) == 0);
        sc_addr *iter = (sc_addr*)content;
        sc_addr *iter_end = (sc_addr*)(content + content_len);
        sc_bool found = SC_FALSE;

        while (iter != iter_end)
        {
            if (SC_ADDR_IS_EQUAL(*iter, addr))
            {
                if (content_len > sizeof(sc_addr))
                {
                    found = SC_TRUE;
                    content2 = g_new0(gchar, content_len - sizeof(sc_addr));
                    if ((gchar*)iter != content)
                        memcpy(content2, content, (gchar*)iter - content);

                    iter_next = iter;
                    ++iter_next;

                    if (iter_next != iter_end)
                        memcpy(content2, iter + 1, (gchar*)iter_end - (gchar*)iter_next);

                    g_free(content);
                    content = content2;
                } else
                {
                    g_free(content);
                    content = null_ptr;
                }

                break;
            }
            ++iter;
        }

        content = g_new0(gchar, content_len + sizeof(addr));
        memcpy(content, content2, content_len);
        memcpy(content + content_len, &addr, sizeof(addr));
        content_len += sizeof(addr);

        // old content doesn't need
        g_free(content2);
    } else
        res = SC_RESULT_ERROR_NOT_FOUND;

    // write content to file
    res = SC_RESULT_OK;
    if (content == null_ptr)
    {
        if (g_remove(addr_path) != 0)
            res = SC_RESULT_ERROR_IO;
    }
    else if (g_file_set_contents(addr_path, content, content_len, 0) != TRUE)
        res = SC_RESULT_ERROR_IO;

    clean:
    {
        if (content)
            g_free(content);
        free(path);
        UNLOCK_FS();
    }

    return res;
}

sc_result sc_fs_engine_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{
    LOCK_FS();

    sc_uint8 *path = sc_fs_engine_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar addr_path[MAX_PATH_LENGTH];
    gchar *content = 0, *content2 = 0;
    gsize content_len = 0;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(addr_path, MAX_PATH_LENGTH, "%saddrs", abs_path);

    sc_result res = SC_RESULT_ERROR_IO;

    // must be a null pointer
    g_assert(*result == 0);

    *result_count = 0;

    // try to load existing file
    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
            goto clean;
    }

    // store result
    if (content == 0)
    {
        *result = 0;
        *result_count = 0;
        res = SC_RESULT_ERROR_NOT_FOUND;
    }else
    {
        *result_count = *((sc_uint32*)content);
        content2 = content;
        content2 += sizeof(sc_uint32);

        *result = g_new0(sc_addr, *result_count);
        memcpy(*result, content2, sizeof(sc_addr) * (*result_count));
        res = SC_RESULT_OK;
    }


    clean:
    {
        if (content != 0)
            g_free(content);
        free(path);
        UNLOCK_FS();
    }

    return res;
}

sc_result _sc_fs_clear_delete_files(const char *root_path)
{
    // remove all segments
    GDir *dir = 0;
    const gchar *fname = 0;
    char path[MAX_PATH_LENGTH];

    dir = g_dir_open(root_path, 0, 0);
	if (dir != (GDir*)0)
	{
		// calculate files
		fname = g_dir_read_name(dir);
		while (fname != 0)
		{
			g_snprintf(path, MAX_PATH_LENGTH, "%s/%s", root_path, fname);

			if (g_file_test(path, G_FILE_TEST_IS_REGULAR))
			{
				if (g_remove(path) == -1)
				{
					g_critical("Can't remove file: %s", path);
					g_dir_close(dir);
					return SC_RESULT_ERROR;
				}
			}
			else
			{
				if (g_file_test(path, G_FILE_TEST_IS_DIR))
				{
					sc_result res = _sc_fs_clear_delete_files(path);
					if (res != SC_RESULT_OK)
					{
						g_dir_close(dir);
						return SC_RESULT_ERROR;
					}
				}
			}
			fname = g_dir_read_name(dir);
		}

		g_dir_close(dir);
		if (g_rmdir(root_path))
			return SC_RESULT_ERROR;
	}

    return SC_RESULT_OK;
}

sc_result sc_fs_engine_clear(const sc_fm_engine *engine)
{
    LOCK_FS();
    sc_result res = _sc_fs_clear_delete_files(contents_path);
    UNLOCK_FS();
    return res;
}

sc_result sc_fs_save(const sc_fm_engine *engine)
{
    return SC_RESULT_OK;
}

sc_result sc_fs_engine_destroy_data(const sc_fm_engine *engine)
{
    return SC_RESULT_OK;
}

sc_result _sc_fs_engine_clean_state_dir(const sc_memory_context *ctx, const gchar *path)
{
    sc_result res = SC_RESULT_OK;
    GDir *dir = g_dir_open(path, 0, 0);
    const gchar *name = null_ptr;
    gchar buff[MAX_PATH_LENGTH];

    while ((name = g_dir_read_name(dir)) != null_ptr && res == SC_RESULT_OK)
    {
        g_snprintf(buff, MAX_PATH_LENGTH, "%s/%s", path, name);
        if (g_file_test(buff, G_FILE_TEST_IS_DIR) == TRUE)
        {
            res = _sc_fs_engine_clean_state_dir(ctx, buff);
        }
        else if (g_file_test(buff, G_FILE_TEST_IS_REGULAR) == TRUE)
        {
            if (g_str_has_suffix(buff, "addrs") == TRUE)
            {
                gchar *content = null_ptr;
                gsize len = 0;
                if (g_file_get_contents(buff, &content, &len, 0) == TRUE)
                {
                    gchar *content2 = g_new0(gchar, len);

                    sc_uint32 count = *(sc_uint32*)content;
                    sc_addr *addrs = (sc_addr*)(content + sizeof(sc_uint32));
                    sc_addr *addrs2 = (sc_addr*)(content2 + sizeof(sc_uint32));
                    sc_uint32 i = 0, copied = 0;
                    for (; i < count; ++i)
                    {
                        if (sc_memory_is_element(ctx, addrs[i]) == SC_TRUE)
                        {
                            addrs2[copied] = addrs[i];
                            copied++;
                        }
                    }

                    *(sc_uint32*)content2 = copied;

                    if (copied != count)
                    {
                        if (g_file_set_contents(buff, content2, sizeof(sc_uint32) + sizeof(sc_addr) * copied, 0) == FALSE)
                            res = SC_RESULT_ERROR_IO;
                    }

                    g_free(content);
                    g_free(content2);
                } else
                    res = SC_RESULT_ERROR_IO;
            }
        }
    }

    g_dir_close(dir);
    return res;
}

sc_result sc_fs_engine_clean_state(const sc_fm_engine *engine)
{
    sc_memory_context *ctx = sc_memory_context_new(sc_access_lvl_make_max);
    sc_bool res = _sc_fs_engine_clean_state_dir(ctx, contents_path);
    sc_memory_context_free(ctx);
    return res;
}


// --- extension interface ---
_SC_EXT_EXTERN sc_fm_engine * initialize(const sc_char* repo_path)
{
    // initialize file system storage
    g_snprintf(contents_path, MAX_PATH_LENGTH, "%s/%s", repo_path, content_dir);

    if (!g_file_test(contents_path, G_FILE_TEST_IS_DIR))
    {
        if (g_mkdir_with_parents(contents_path, -1) < 0)
            g_error("Can't create '%s' directory.", contents_path);
    }

    g_mutex_init(&fs_mutex);

    sc_fm_engine *engine = g_new0(sc_fm_engine, 1);

    engine->storage_info = 0;
    engine->funcStreamCreate = &sc_fs_engine_create_stream;
    engine->funcAddrRefAppend = &sc_fs_engine_addr_ref_append;
    engine->funcAddrRefRemove = &sc_fs_engine_addr_ref_remove;
    engine->funcFind = &sc_fs_engine_find;
    engine->funcClear = &sc_fs_engine_clear;
    engine->funcSave = &sc_fs_save;
    engine->funcDestroyData = &sc_fs_engine_destroy_data;
    engine->funcCleanState = &sc_fs_engine_clean_state;

    return engine;
}

_SC_EXT_EXTERN sc_result shutdown()
{
    g_mutex_clear(&fs_mutex);
    return SC_RESULT_OK;
}
