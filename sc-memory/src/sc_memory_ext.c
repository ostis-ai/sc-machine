/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#include "sc_memory_ext.h"

#include <glib.h>
#include <gmodule.h>

// Table of loaded extension modules
GHashTable *modules_table = 0;

//! Type of module function
typedef sc_result (*fModuleFunc)();

gboolean modules_table_equal_func(gconstpointer a, gconstpointer b)
{
    return a == b;
}


sc_result sc_ext_initialize(const sc_char *ext_dir_path)
{
    GDir *ext_dir = nullptr;
    const gchar *file_name = 0;
    GModule *module = 0;
    gchar *module_path = 0;
    fModuleFunc func = 0;

    // doesn't need to initialize extensions
    if (ext_dir_path == nullptr)
        return SC_RESULT_OK;

    // check if modules supported on this platform
    if (g_module_supported() == FALSE)
    {
        g_warning("Modules not supported on this platform");
        return SC_RESULT_ERROR;
    }

    g_message("Initialize extensions from %s", ext_dir_path);

    // check if specified directory exist
    if (g_file_test(ext_dir_path, G_FILE_TEST_IS_DIR) == FALSE)
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // get list of files in extension directory
    ext_dir = g_dir_open(ext_dir_path, 0, 0);
    if (ext_dir == nullptr)
        return SC_RESULT_ERROR;

    modules_table = g_hash_table_new(&g_str_hash, &modules_table_equal_func);

    // list all files in directory and try to load them
    file_name = g_dir_read_name(ext_dir);
    while (file_name != nullptr)
    {
        // build module path
        module_path = g_module_build_path(ext_dir_path, file_name);

        // open module
        module = g_module_open(module_path, G_MODULE_BIND_LAZY & G_MODULE_BIND_LOCAL);

        // skip non module files
        if (g_str_has_suffix(file_name, G_MODULE_SUFFIX) == TRUE)
        {

            if (module == nullptr)
            {
                g_warning("Can't load module: %s. Error: %s", file_name, g_module_error());
            }else
            {
                g_message("Initialize module: %s", file_name);
                if (g_module_symbol(module, "initialize", (gpointer*) &func) == FALSE)
                {
                    g_warning("Can't find 'initialize' symbol in module: %s", file_name);
                }else
                {
                    if (func() != SC_RESULT_OK)
                    {
                        g_warning("Something happends, on module initialization: %s", file_name);
                    }
                }

                g_hash_table_insert(modules_table, module_path, (gpointer)module);
            }
        }

        file_name = g_dir_read_name(ext_dir);
    }

    g_dir_close(ext_dir);

    return SC_RESULT_OK;
}

void module_table_unload_func(gpointer key, gpointer value, gpointer user_data)
{
    gchar *module_name = (gchar*)key;
    GModule *module = (GModule*)value;
    fModuleFunc func = 0;

    g_assert(module_name != nullptr);
    g_assert(module != nullptr);

    if (g_module_symbol(module, module_name, (gpointer*)&func) == FALSE)
    {
        g_warning("Can't find shutdown symbol in module: %s", module_name);
    }else
    {
        if (func() != SC_RESULT_OK)
        {
            g_warning("Something happends, on module shutdown: %s", module_name);
        }
    }

    g_module_close(module);
    g_free(module_name);

}

void sc_ext_shutdown()
{
    if (modules_table == nullptr)
        return; // extensions wasn't initialized

    // itrate all loaded modules and try to unload them
    g_hash_table_foreach(modules_table, &module_table_unload_func, 0);

    g_hash_table_destroy(modules_table);
    modules_table = nullptr;
}
