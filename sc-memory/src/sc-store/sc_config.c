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

#include "sc_config.h"

#include <glib.h>

const char str_group_memory[] = "memory";

const char str_key_max_loaded_segments[] = "max_loaded_segments";


// Maximum number of segments, that can be loaded into memory at one moment
sc_uint config_max_loaded_segments = 0;

void sc_config_initialize(const sc_char *file_path)
{
    GKeyFile *key_file = 0;

    key_file = g_key_file_new();
    if ((file_path != nullptr) && (g_key_file_load_from_file(key_file, file_path, G_KEY_FILE_NONE, 0) == TRUE))
    {
        // parse settings

        if (g_key_file_has_key(key_file, str_group_memory, str_key_max_loaded_segments, 0) == TRUE)
            config_max_loaded_segments = g_key_file_get_integer(key_file, str_group_memory, str_key_max_loaded_segments, 0);

    }else
    {
        // setup default values
        config_max_loaded_segments = 10;
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
