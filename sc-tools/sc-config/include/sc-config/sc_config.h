/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_config_h_
#define _sc_config_h_

#include <sc-core/sc_types.h>

// Config, that contains all configuration options: ['<group>/<key>'] = <value>
typedef struct _GHashTable sc_config;

#define sc_config_get_keys(config) g_hash_table_get_keys(config)
#define sc_list_get_first(list) g_list_first(list)
#define sc_list_free(list) g_list_free(list)

/*! Initialize sc-memory configuration from specified configuration file
 * @param config Config table
 * @param file_path Path to configuration file
 */
sc_bool sc_config_initialize(sc_config ** config, sc_char const * file_path);

/*! Shutdown sc-memory configuration. Free allocated data.
 */
void sc_config_shutdown(sc_config * config);

// --- api for extensions ---
/*!
 * Returns string value of \p key in specified \p group
 * @param config Config table
 * @param group Name of configuration group
 * @param key Name of key to return value
 * @returns Returns the value associated with the key as string, or null if the key was not found of could not be
 * parsed.
 * @attention Returned value managed by sc_config and shouldn't be freed
 */

_SC_EXTERN sc_char * sc_config_get_value_string(sc_config * config, sc_char const * group, sc_char const * key);

#endif
