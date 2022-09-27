/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_file_system.h"

#include "../sc-base/sc_assert_utils.h"
#include "../sc-base/sc_message.h"

#include "glib.h"
#include "glib/gstdio.h"

void sc_fs_rmdir(const sc_char * path)
{
  if (g_file_test(path, G_FILE_TEST_IS_DIR) == SC_FALSE)
    return;

  GDir * directory = g_dir_open(path, 0, 0);
  sc_assert(directory != null_ptr);

  // calculate files
  sc_char tmp_path[MAX_PATH_LENGTH];
  sc_char const * file = g_dir_read_name(directory);
  while (file != null_ptr)
  {
    g_snprintf(tmp_path, MAX_PATH_LENGTH, "%s/%s", path, file);

    if (g_file_test(tmp_path, G_FILE_TEST_IS_REGULAR) == SC_TRUE)
    {
      if (g_remove(tmp_path) == -1)
        sc_critical("Can't remove file: %s", tmp_path);
    }
    else if (g_file_test(tmp_path, G_FILE_TEST_IS_DIR) == SC_TRUE)
      sc_fs_rmdir(tmp_path);

    file = g_dir_read_name(directory);
  }

  g_dir_close(directory);
  g_rmdir(path);
}

sc_bool sc_fs_mkdirs(const sc_char * path)
{
#if SC_PLATFORM_LINUX || SC_PLATFORM_MAC
  int const mode = 0755;
#else
  int const mode = 0;
#endif
  if (g_mkdir_with_parents(path, mode) == -1)
    return SC_FALSE;

  return SC_TRUE;
}

void * sc_fs_open_tmp_file(const sc_char * path, sc_char ** tmp_file_name, sc_char * prefix)
{
  *tmp_file_name = g_strdup_printf("%s/%s_%lu", path, prefix, (sc_ulong)g_get_real_time());

  GIOChannel * result = g_io_channel_new_file(*tmp_file_name, "w", null_ptr);
  return result;
}
