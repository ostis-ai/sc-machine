/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_config.h"

void value_table_destroy_key_value(void * data)
{
  g_free(data);
}

sc_char * value_table_create_hash_key(sc_char const * group, sc_char const * key)
{
  return g_strdup_printf("%s/%s", group, key);
}

sc_bool sc_config_initialize(sc_config ** config, sc_char const * file_path)
{
  GKeyFile * key_file = g_key_file_new();

  if (file_path == null_ptr || g_key_file_load_from_file(key_file, file_path, G_KEY_FILE_NONE, null_ptr) == SC_FALSE)
  {
    g_key_file_free(key_file);
    return SC_FALSE;
  }

  // load all values into hash table
  *config =
      g_hash_table_new_full(g_str_hash, g_str_equal, value_table_destroy_key_value, value_table_destroy_key_value);

  sc_ulong groups_len = 0;
  sc_char ** groups = g_key_file_get_groups(key_file, &groups_len);
  for (sc_ulong i = 0; i < groups_len; ++i)
  {
    // get all keys in group
    sc_ulong keys_len = 0;
    sc_char ** keys = g_key_file_get_keys(key_file, groups[i], &keys_len, 0);
    for (sc_ulong j = 0; j < keys_len; ++j)
    {
      // append keys into hash table
      sc_char * hash_key = value_table_create_hash_key(groups[i], keys[j]);
      g_hash_table_insert(*config, hash_key, g_key_file_get_string(key_file, groups[i], keys[j], 0));
    }
    g_strfreev(keys);
  }
  g_strfreev(groups);

  g_key_file_free(key_file);

  return SC_TRUE;
}

void sc_config_shutdown(sc_config * config)
{
  // empty memory allocated for values table
  if (config != null_ptr)
  {
    g_hash_table_remove_all(config);
    g_hash_table_destroy(config);
  }

  config = null_ptr;
}

sc_char * sc_config_get_value_string(sc_config * config, sc_char const * group, sc_char const * key)
{
  sc_char * hash_key = value_table_create_hash_key(group, key);
  const void * res = g_hash_table_lookup(config, hash_key);
  g_free(hash_key);
  return (sc_char *)res;
}

sc_int sc_config_get_value_int(sc_config * config, sc_char const * group, sc_char const * key)
{
  sc_char const * str_value = sc_config_get_value_string(config, group, key);
  if (str_value == null_ptr)
    return 0;

  return atoi(str_value);
}

float sc_config_get_value_float(sc_config * config, sc_char const * group, sc_char const * key)
{
  return (float)sc_config_get_value_int(config, group, key);
}

sc_bool sc_config_get_value_boolean(sc_config * config, sc_char const * group, sc_char const * key)
{
  sc_char const * str_value = sc_config_get_value_string(config, group, key);
  if (str_value == null_ptr)
    return SC_FALSE;

  if (g_str_equal(str_value, "true") || g_str_equal(str_value, "True") || g_str_equal(str_value, "1"))
    return SC_TRUE;

  return SC_FALSE;
}
