/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_ext.h"

#include <gmodule.h>

#include "sc-store/sc-base/sc_allocator.h"
#include "sc-store/sc-base/sc_message.h"

GList * modules_priority_list = null_ptr;

//! Type of module function
typedef sc_result (*fModuleFunc)();
typedef sc_uint32 (*fModulePriorityFunc)();

typedef struct _sc_module_info
{
  GModule * ptr;
  gchar * path;
  sc_uint32 priority;
  fModuleFunc init_func;
  fModuleFunc shut_func;
} sc_module_info;

void sc_module_info_free(gpointer mi)
{
  sc_module_info * info = (sc_module_info *)mi;

  if (info->path)
    sc_mem_free(info->path);
  if (info->ptr)
    g_module_close(info->ptr);
  sc_mem_free(info);
}

gint sc_priority_less(gconstpointer a, gconstpointer b)
{
  sc_module_info * ma = (sc_module_info *)a;
  sc_module_info * mb = (sc_module_info *)b;

  if (ma->priority < mb->priority)
    return -1;
  if (ma->priority > mb->priority)
    return 1;

  return strcmp(ma->path, mb->path);
}

gint sc_priority_great(gconstpointer a, gconstpointer b)
{
  return sc_priority_less(b, a);
}

sc_result sc_ext_initialize(const sc_char * ext_dir_path, const sc_char ** enabled_list)
{
  GDir * ext_dir = null_ptr;
  const sc_char * file_name = null_ptr;
  fModuleFunc func = null_ptr;

#if SC_IS_PLATFORM_MAC
  const gchar * moduleSuffix = "dylib";
#else
  const gchar * moduleSuffix = G_MODULE_SUFFIX;
#endif

  // doesn't need to initialize extensions
  if (ext_dir_path == null_ptr)
    return SC_RESULT_OK;

  // check if modules supported on this platform
  if (g_module_supported() == SC_FALSE)
  {
    sc_warning("Modules not supported on this platform");
    return SC_RESULT_ERROR;
  }

  sc_message("Initialize extensions from %s", ext_dir_path);

  // check if specified directory exist
  if (g_file_test(ext_dir_path, G_FILE_TEST_IS_DIR) == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // get list of files in extension directory
  ext_dir = g_dir_open(ext_dir_path, 0, null_ptr);
  if (ext_dir == null_ptr)
    return SC_RESULT_ERROR;

  // list all files in directory and try to load them
  file_name = g_dir_read_name(ext_dir);
  while (file_name != null_ptr)
  {
    // check if it should be loaded
    if (enabled_list)
    {
      sc_int32 i = 0;
      gboolean shouldSkip = SC_TRUE;
      while (enabled_list[i] != null_ptr)
      {
        const sc_char * name = enabled_list[i];
        if (g_str_has_prefix(file_name, name) && g_str_has_suffix(file_name, moduleSuffix))
        {
          shouldSkip = (strlen(file_name) != (strlen(name) + strlen(moduleSuffix) + 1));
        }

        ++i;
      }

      if (shouldSkip)
        goto next;
    }

    sc_module_info * mi = sc_mem_new(sc_module_info, 1);
    mi->path = g_module_build_path(ext_dir_path, file_name);

    // open module
    mi->ptr = g_module_open(mi->path, G_MODULE_BIND_LOCAL);
    if (mi->ptr == null_ptr)
    {
      sc_warning("Can't load module %s: %s", mi->path, g_module_error());
      goto clean;
    }

    // skip non module files
    if (g_str_has_suffix(file_name, moduleSuffix) == SC_TRUE)
    {
      if (g_module_symbol(mi->ptr, "sc_module_initialize", (gpointer *)&func) == SC_FALSE)
      {
        sc_warning("Can't find 'sc_module_initialize' symbol in module: %s", mi->path);
        goto clean;
      }
      mi->init_func = func;

      if (g_module_symbol(mi->ptr, "sc_module_shutdown", (gpointer *)&func) == SC_FALSE)
      {
        sc_warning("Can't find 'sc_module_shutdown' symbol in module: %s", mi->path);
        goto clean;
      }
      mi->shut_func = func;

      fModulePriorityFunc pfunc;
      if (g_module_symbol(mi->ptr, "sc_module_load_priority", (gpointer *)&pfunc) == SC_FALSE)
        mi->priority = G_MAXUINT32;
      else
        mi->priority = pfunc();
    }

    modules_priority_list = g_list_insert_sorted(modules_priority_list, (gpointer)mi, sc_priority_less);
    goto next;

  clean:
  {
    sc_module_info_free(mi);
  }

  next:
  {
    file_name = g_dir_read_name(ext_dir);
  }
  }

  g_dir_close(ext_dir);

  // initialize modules
  GList * item = modules_priority_list;
  while (item != null_ptr)
  {
    sc_module_info * module = (sc_module_info *)item->data;
    sc_message("Initialize module: %s", module->path);
    if (module->init_func() != SC_RESULT_OK)
    {
      sc_warning("Something happens, on module initialization: %s", module->path);
      module->shut_func();
      g_module_close(module->ptr);
      module->ptr = null_ptr;
    }

    item = item->next;
  }

  return SC_RESULT_OK;
}

void sc_ext_shutdown()
{
  modules_priority_list = g_list_sort(modules_priority_list, sc_priority_great);
  GList * item = modules_priority_list;
  while (item != null_ptr)
  {
    sc_module_info * module = (sc_module_info *)item->data;
    sc_message("Shutdown module: %s", module->path);
    if (module->ptr != null_ptr)
    {
      if (module->shut_func() != SC_RESULT_OK)
        sc_warning("Something happens, on module shutdown: %s", module->path);
    }

    item = item->next;
  }

  g_list_free_full(modules_priority_list, sc_module_info_free);
  modules_priority_list = null_ptr;
}
