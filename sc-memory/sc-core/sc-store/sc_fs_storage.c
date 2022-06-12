/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fs_storage.h"

#include "sc_segment.h"
#include "sc_stream_file.h"
#include "sc_fm_engine.h"

#include "../sc_memory_version.h"

#include <stdlib.h>
#include <memory.h>
#include <glib.h>
#include <gmodule.h>
#include <glib/gstdio.h>

#define BuffSize 256 * 1024

gchar * repo_path = 0;
gchar segments_path[MAX_PATH_LENGTH];  // Path to file, where stored segments in correct state
#define SC_DIR_PERMISSIONS -1

const gchar * seg_meta = "_meta";
const gchar * addr_key_group = "addrs";

// ----------------------------------------------
void _get_segment_path(const gchar * path_dir, sc_uint id, sc_uint max_path_len, gchar * res)
{
  g_snprintf(res, max_path_len, "%s/%.10d", path_dir, id);
}

void _get_segment_checksum_path(const gchar * path, sc_uint max_path_len, gchar * res)
{
  g_snprintf(res, max_path_len, "%s.checksum", path);
}

int _checksum_type()
{
  return G_CHECKSUM_SHA512;
}

sc_uint8 _checksum_get_size()
{
  return (sc_uint8)g_checksum_type_get_length(_checksum_type());
}

guint8 * _checksum_seg(sc_segment const * seg)
{
  guint8 * result = null_ptr;
  gsize length = 0;
  GChecksum * checksum = g_checksum_new(_checksum_type());
  g_assert(seg);

  if (checksum)
  {
    g_checksum_reset(checksum);
    g_checksum_update(checksum, (guchar *)(&seg->elements[0]), SC_SEG_ELEMENTS_SIZE_BYTE);

    length = _checksum_get_size();
    result = g_new0(guint8, length);
    g_checksum_get_digest(checksum, result, &length);
    g_assert(length == _checksum_get_size());
  }

  return result;
}

void _sc_fm_remove_dir(const char * path)
{
  char tmp_path[MAX_PATH_LENGTH];
  gchar const * fname = 0;
  GDir * dir = 0;

  if (g_file_test(path, G_FILE_TEST_IS_DIR) == FALSE)
    return;

  dir = g_dir_open(path, 0, 0);
  g_assert(dir != (GDir *)0);

  // calculate files
  fname = g_dir_read_name(dir);
  while (fname != 0)
  {
    g_snprintf(tmp_path, MAX_PATH_LENGTH, "%s/%s", path, fname);

    if (g_file_test(tmp_path, G_FILE_TEST_IS_REGULAR) == TRUE)
    {
      if (g_remove(tmp_path) == -1)
        g_critical("Can't remove file: %s", tmp_path);
    }
    else if (g_file_test(tmp_path, G_FILE_TEST_IS_DIR) == TRUE)
    {
      _sc_fm_remove_dir(tmp_path);
    }

    fname = g_dir_read_name(dir);
  }

  g_dir_close(dir);
  g_rmdir(path);
}

// ----------------------------------------------
sc_bool _sc_fs_mkdirs(const char * path)
{
#if SC_PLATFORM_LINUX || SC_PLATFORM_MAC
  int const mode = 0755;
#else
  int const mode = 0;
#endif
  if (g_mkdir_with_parents(repo_path, mode) == -1)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_fs_storage_initialize(const gchar * path, sc_bool clear)
{
  g_message("Initialize sc-storage from path: %s", path);
  g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/segments.scdb", path);
  repo_path = g_strdup(path);

  sc_fm_init(repo_path);

  // clear repository if needs
  if (clear == SC_TRUE)
  {
    g_message("Clear memory");
    if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) && g_remove(segments_path) != 0)
      g_error("Can't delete segments file: %s", segments_path);

    g_message("Clear file memory");
    if (sc_fm_clear() != SC_RESULT_OK)
    {
      g_critical("Can't clear file memory");
      return SC_FALSE;
    }
  }

  return SC_TRUE;
}

sc_bool sc_fs_storage_shutdown(sc_segment ** segments, sc_bool save_segments)
{
  g_message("Shutdown sc-storage");

  if (save_segments == SC_TRUE)
  {
    g_message("Write segments");
    sc_fs_storage_write_to_path(segments);
  }

  sc_bool res = SC_FALSE;
  sc_fm_free();

  g_free(repo_path);

  return res;
}

sc_bool sc_fs_storage_read_from_path(sc_segment ** segments, sc_uint32 * segments_num)
{
  if (g_file_test(repo_path, G_FILE_TEST_IS_DIR) == FALSE)
  {
    g_error("%s isn't a directory.", repo_path);
    return SC_FALSE;
  }

  if (g_file_test(segments_path, G_FILE_TEST_IS_REGULAR) == FALSE)
  {
    g_message("There are no segments in %s", segments_path);
    return SC_FALSE;
  }

  // open segments
  {
    GIOChannel * in_file = g_io_channel_new_file(segments_path, "r", null_ptr);
    sc_fs_storage_segments_header header;
    gsize bytes_num = 0;
    sc_uint32 i = 0, header_size = 0;
    GChecksum * checksum = null_ptr;
    sc_segment * seg = null_ptr;
    sc_bool is_valid = SC_TRUE;
    sc_uint8 calculated_checksum[SC_STORAGE_SEG_CHECKSUM_SIZE];

    g_assert(_checksum_get_size() == SC_STORAGE_SEG_CHECKSUM_SIZE);
    if (!in_file)
    {
      g_critical("Can't open segments from: %s", segments_path);
      return SC_FALSE;
    }

    if (g_io_channel_set_encoding(in_file, null_ptr, null_ptr) != G_IO_STATUS_NORMAL)
    {
      g_critical("Can't setup encoding: %s", segments_path);
      return SC_FALSE;
    }

    if ((g_io_channel_read_chars(in_file, (gchar *)&header_size, sizeof(header_size), &bytes_num, null_ptr) !=
         G_IO_STATUS_NORMAL) ||
        (bytes_num != sizeof(header_size)))
    {
      g_critical("Can't read header size");
      return SC_FALSE;
    }

    if (header_size != sizeof(header))
    {
      g_critical("Invalid header size %d != %d", header_size, (int)sizeof(header));
      return SC_FALSE;
    }

    if ((g_io_channel_read_chars(in_file, (gchar *)&header, sizeof(header), &bytes_num, null_ptr) !=
         G_IO_STATUS_NORMAL) ||
        (bytes_num != sizeof(header)))
    {
      g_critical("Can't read header of segments: %s", segments_path);
      return SC_FALSE;
    }

    *segments_num = header.segments_num;

    /// TODO: Check version

    checksum = g_checksum_new(_checksum_type());
    g_assert(checksum);

    g_checksum_reset(checksum);

    // chek data
    for (i = 0; i < *segments_num; ++i)
    {
      seg = sc_segment_new(i);
      segments[i] = seg;

      g_io_channel_read_chars(in_file, (gchar *)seg->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes_num, null_ptr);
      sc_segment_loaded(seg);
      if (bytes_num != SC_SEG_ELEMENTS_SIZE_BYTE)
      {
        g_error("Error while read data for segment: %d", i);
        is_valid = SC_FALSE;
        break;
      }
      g_checksum_update(checksum, (guchar *)seg->elements, SC_SEG_ELEMENTS_SIZE_BYTE);
    }

    if (is_valid == SC_TRUE)
    {
      // compare checksum
      g_checksum_get_digest(checksum, calculated_checksum, &bytes_num);
      if (bytes_num != SC_STORAGE_SEG_CHECKSUM_SIZE)
        is_valid = SC_FALSE;
      else
        is_valid =
            (memcmp(calculated_checksum, header.checksum, SC_STORAGE_SEG_CHECKSUM_SIZE) == 0) ? SC_TRUE : SC_FALSE;
    }

    if (is_valid == SC_FALSE)
    {
      *segments_num = 0;
      for (i = 0; i < SC_SEGMENT_MAX; ++i)
      {
        if (segments[i])
        {
          sc_segment_free(segments[i]);
          segments[i] = null_ptr;
        }
      }
    }

    g_checksum_free(checksum);
    g_io_channel_shutdown(in_file, FALSE, null_ptr);

    if (is_valid == SC_FALSE)
      return SC_FALSE;
  }

  g_message("Segments loaded: %u", *segments_num);

  g_message("Check file memory state");
  sc_bool r = sc_fm_clean_state() == SC_RESULT_OK;

  if (r == SC_FALSE)
    g_error("File memory wasn't check properly");

  return r;
}

static GIOChannel * _open_tmp_file(gchar ** tmp_file_name)
{
  GIOChannel * result;

  *tmp_file_name = g_strdup_printf("%s/segments_%lu", repo_path, (unsigned long)g_get_real_time());

  result = g_io_channel_new_file(*tmp_file_name, "w", null_ptr);

  return result;
}

sc_bool sc_fs_storage_write_to_path(sc_segment ** segments)
{
  sc_uint32 idx = 0, header_size = 0;
  const sc_segment * segment = 0;
  sc_fs_storage_segments_header header;
  GChecksum * checksum = null_ptr;
  GIOChannel * output = null_ptr;
  gchar * tmp_filename = null_ptr;
  gsize bytes;
  sc_bool result = SC_TRUE;

  if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
  {
    if (!_sc_fs_mkdirs(repo_path))
    {
      g_error("Can't create a directory %s", repo_path);
      return SC_FALSE;
    }
  }

  // create temporary file

  output = _open_tmp_file(&tmp_filename);

  memset(&header, 0, sizeof(sc_fs_storage_segments_header));
  header.segments_num = 0;
  header.timestamp = g_get_real_time();
  header.version = sc_version_to_int(&SC_VERSION);

  g_io_channel_set_encoding(output, null_ptr, null_ptr);

  checksum = g_checksum_new(_checksum_type());
  g_checksum_reset(checksum);

  for (idx = 0; idx < SC_ADDR_SEG_MAX; idx++)
  {
    segment = segments[idx];
    if (segment == null_ptr)
      break;  // stop save, because we allocate segment in order

    g_checksum_update(checksum, (guchar *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE);
  }

  header.segments_num = idx;
  bytes = SC_STORAGE_SEG_CHECKSUM_SIZE;
  g_checksum_get_digest(checksum, header.checksum, &bytes);

  header_size = sizeof(header);
  if (g_io_channel_write_chars(output, (gchar *)&header_size, sizeof(header_size), &bytes, null_ptr) !=
          G_IO_STATUS_NORMAL ||
      bytes != sizeof(header_size))
  {
    g_error("Can't write header size: %s", tmp_filename);
    result = SC_FALSE;
    goto clean;
  }

  if (g_io_channel_write_chars(output, (gchar *)&header, header_size, &bytes, null_ptr) != G_IO_STATUS_NORMAL ||
      bytes != header_size)
  {
    g_error("Can't write header: %s", tmp_filename);
    result = SC_FALSE;
    goto clean;
  }

  for (idx = 0; idx < header.segments_num; ++idx)
  {
    segment = segments[idx];
    g_assert(segment != null_ptr);

    if (g_io_channel_write_chars(output, (gchar *)segment->elements, SC_SEG_ELEMENTS_SIZE_BYTE, &bytes, null_ptr) !=
            G_IO_STATUS_NORMAL ||
        bytes != SC_SEG_ELEMENTS_SIZE_BYTE)
    {
      g_error("Can't write segment %d into %s", idx, tmp_filename);
      result = SC_FALSE;
      goto clean;
    }
  }

  if (result == SC_TRUE)
  {
    // rename main file
    if (g_file_test(tmp_filename, G_FILE_TEST_IS_REGULAR))
    {
      g_io_channel_shutdown(output, TRUE, NULL);
      output = null_ptr;

      if (g_rename(tmp_filename, segments_path) != 0)
      {
        g_error("Can't rename %s -> %s", tmp_filename, segments_path);
        result = SC_FALSE;
      }
    }

    // save file memory
    g_message("Save file memory state");
    if (sc_fm_save() != SC_RESULT_OK)
      g_critical("Error while saves file memory");
  }

clean:
{
  if (tmp_filename)
    g_free(tmp_filename);
  if (checksum)
    g_checksum_free(checksum);
  if (output)
    g_io_channel_shutdown(output, TRUE, null_ptr);
}

  return result;
}

sc_result sc_fs_storage_write_content(sc_addr addr, const sc_check_sum * check_sum, const sc_stream * stream)
{
  if (sc_fm_write_stream(check_sum, stream) == SC_RESULT_OK)
  {
    // seek input stream to start position
    sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);
    return sc_fs_storage_add_content_addr(addr, check_sum);
  }

  return SC_RESULT_ERROR_IO;
}

sc_result sc_fs_storage_add_content_addr(sc_addr addr, const sc_check_sum * check_sum)
{
  return sc_fm_addr_ref_append(addr, check_sum);
}

sc_result sc_fs_storage_remove_content_addr(sc_addr addr, const sc_check_sum * check_sum)
{
  return sc_fm_addr_ref_remove(addr, check_sum);
}

sc_result sc_fs_storage_find_links_with_content(
    const sc_check_sum * check_sum,
    sc_addr ** result,
    sc_uint32 * result_count)
{
  return sc_fm_find(check_sum, result, result_count);
}

sc_result sc_fs_storage_get_checksum_content(const sc_check_sum * check_sum, sc_stream ** stream)
{
  *stream = sc_fm_read_stream_new(check_sum);

  return stream ? SC_RESULT_OK : SC_RESULT_ERROR_IO;
}
