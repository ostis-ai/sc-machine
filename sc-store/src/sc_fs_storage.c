#include "sc_fs_storage.h"
#include "sc_segment.h"

#include <stdlib.h>
#include <memory.h>

gchar *repo_path = 0;

const gchar *seg_dir = "segments";
const gchar *content_dir = "contents";
gchar segments_path[MAX_PATH_LENGTH + 1];
gchar contents_path[MAX_PATH_LENGTH + 1];

#define SC_DIR_PERMISSIONS -1

sc_bool sc_fs_storage_initialize(const gchar *path)
{
    g_message("Initialize sc-storage from path: %s\n", path);

    g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/%s", path, seg_dir);
    g_snprintf(contents_path, MAX_PATH_LENGTH, "%s/%s", path, content_dir);

    if (!g_file_test(contents_path, G_FILE_TEST_IS_DIR))
    {
        if (g_mkdir_with_parents(contents_path, SC_DIR_PERMISSIONS) < 0)
            g_error("Can't create '%s' directory.", contents_path);
        return SC_FALSE;
    }

    repo_path = g_malloc0(sizeof(gchar) * (strlen(path) + 1));
    g_stpcpy(repo_path, path);

    return SC_TRUE;
}

sc_bool sc_fs_storage_shutdown(GPtrArray *segments)
{
    g_message("Shutdown sc-storage\n");
    g_message("Write storage into %s", repo_path);
    sc_fs_storage_write_to_path(segments);

    g_free( repo_path );

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
    sc_uint length;

    _get_segment_path(segments_path, id, MAX_PATH_LENGTH, file_name);
    g_assert( g_file_get_contents(file_name, (gchar**)(&segment), &length, 0) );
    g_assert( length == sizeof(sc_segment) );

    return segment;
}

sc_bool sc_fs_storage_read_from_path(GPtrArray *segments)
{
    const gchar *fname = 0;
    gchar file_name[MAX_PATH_LENGTH + 1];
    gsize length = 0;
    sc_uint files_count = 0, idx;
    sc_segment *segment = 0;
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

    g_message("Segments founded: %u", files_count);

    // load segments
    for (idx = 0; idx < files_count; idx++)
        g_ptr_array_add( segments, (gpointer)sc_fs_storage_load_segment(idx) );

    g_message("Segments loaded: %u", segments->len);

    g_dir_close(dir);
    return SC_TRUE;
}

sc_bool sc_fs_storage_write_to_path(GPtrArray *segments)
{
    sc_uint idx = 0;
    sc_segment *segment = 0;
    GMappedFile *file = 0;
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

    for (idx = 0; idx < segments->len; idx++)
    {
        segment = (sc_segment*)g_ptr_array_index(segments, idx);
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


sc_result sc_fs_storage_write_content(sc_addr addr, const sc_check_sum *check_sum, const sc_uint8 *data, sc_uint32 data_len)
{
    sc_uint8 *path = sc_fs_storage_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar data_path[MAX_PATH_LENGTH];

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(data_path, MAX_PATH_LENGTH, "%sdata", abs_path);

    // check if specified path exist
    if (g_file_test(data_path, G_FILE_TEST_EXISTS))
    {
        g_message("Content data file '%s' already exist", data_path);
        free(path);
        return SC_OK; // do nothing, file saved
    }

    // file doesn't exist, so we need to save it
    if (g_mkdir_with_parents(abs_path, SC_DIR_PERMISSIONS) < 0)
    {
        g_message("Eorror while creating '%s' directory", abs_path);
        free(path);
        return SC_IO_ERROR;
    }

    // write content into file
    if (g_file_set_contents(data_path, data, data_len, 0) == TRUE)
    {

        //! @todo add storage of sc-addr, to find sc-links by content

        free(path);
        return SC_OK;
    }

    return SC_IO_ERROR;
}

sc_uint8* sc_fs_storage_make_checksum_path(const sc_check_sum *check_sum)
{
    // calculate output string length and create it
    sc_uint div = (check_sum->len % 8 == 0) ? 8 : 4;
    sc_uint len = check_sum->len + check_sum->len / div + 1;
    sc_uint8 *result = malloc(sizeof(sc_uint8) * len);
    sc_uint idx = 0;
    sc_uint j = 0;

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
