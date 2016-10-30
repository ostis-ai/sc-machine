/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_config.h"

#include <glib.h>

const char str_group_memory[] = "memory";
const char str_group_fm[] = "filememory";

const char str_key_max_loaded_segments[] = "max_loaded_segments";
const char str_key_fm_engine[] = "engine";


// Maximum number of segments, that can be loaded into memory at one moment
sc_uint config_max_loaded_segments = G_MAXUINT16;

// Hash table, that contains all configuration options: ['<group>/<key>'] = <value>
GHashTable *values_table = 0;

// --- file memory ---
const char fm_default_engine[] = "filesystem";
const char *config_fm_engine = fm_default_engine;

void value_table_destroy_key_value(gpointer data)
{
    g_free(data);
}

gchar* value_table_create_hash_key(const char *group, const char *key)
{
    return g_strdup_printf("%s/%s", group, key);
}


void sc_config_initialize(const sc_char *file_path)
{
    GKeyFile *key_file = 0;
    key_file = g_key_file_new();
    if ((file_path != null_ptr) && (g_key_file_load_from_file(key_file, file_path, G_KEY_FILE_NONE, 0) == TRUE))
    {
        // parse settings
        if (g_key_file_has_key(key_file, str_group_memory, str_key_max_loaded_segments, 0) == TRUE)
            config_max_loaded_segments = g_key_file_get_integer(key_file, str_group_memory, str_key_max_loaded_segments, 0);

        // file memory
        if (g_key_file_has_key(key_file, str_group_fm, str_key_fm_engine, 0) == TRUE)
            config_fm_engine = g_key_file_get_string(key_file, str_group_fm, str_key_fm_engine, 0);
    }else
    {
        // setup default values
        config_max_loaded_segments = G_MAXUINT16;
    }

    // load all values into hash table
    values_table = g_hash_table_new_full(g_str_hash, g_str_equal, value_table_destroy_key_value, value_table_destroy_key_value);

    gsize groups_len = 0, i, j;
    gchar **groups = g_key_file_get_groups(key_file, &groups_len);
    for (i = 0; i < groups_len; ++i)
    {
        // get all keys in group
        gsize keys_len = 0;
        gchar **keys = g_key_file_get_keys(key_file, groups[i], &keys_len, 0);
        for (j = 0; j < keys_len; ++j)
        {
            // append keys into hash table
            gchar *hash_key = value_table_create_hash_key(groups[i], keys[j]);
            g_hash_table_insert(values_table, hash_key, g_key_file_get_string(key_file, groups[i], keys[j], 0));
        }
        g_strfreev(keys);

    }
    g_strfreev(groups);


    g_key_file_free(key_file);
}

void sc_config_shutdown()
{
    // empty memory allocated for values table
    g_hash_table_remove_all(values_table);
    g_hash_table_destroy(values_table);
    values_table = 0;
}

sc_int32 sc_config_get_max_loaded_segments()
{
    return config_max_loaded_segments;
}


const char* sc_config_get_value_string(const char *group, const char *key)
{
    gchar *hash_key = value_table_create_hash_key(group, key);
    gconstpointer *res = g_hash_table_lookup(values_table, hash_key);
    g_free(hash_key);
    return (const char*)res;
}

int sc_config_get_value_int(const char *group, const char *key)
{
    const char *str_value = sc_config_get_value_string(group, key);
    if (str_value == 0)
        return 0;

    return atoi(str_value);
}

sc_bool sc_config_get_value_boolean(const char *group, const char *key)
{
    const char *str_value = sc_config_get_value_string(group, key);
    if (str_value == 0)
        return SC_FALSE;

    if (g_str_equal(str_value, "true") || g_str_equal(str_value, "1"))
        return SC_TRUE;

    return SC_FALSE;
}

float sc_config_get_value_float(const char *group, const char *key)
{
    const char *str_value = sc_config_get_value_string(group, key);
    if (str_value == 0)
        return SC_FALSE;

    return (float)atof(str_value);
}

const sc_char* sc_config_fm_engine()
{
    return config_fm_engine;
}
