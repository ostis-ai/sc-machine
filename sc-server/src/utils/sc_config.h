/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_config_h_
#define _sc_config_h_

#include <glib.h>

<<<<<<< HEAD:sc-memory/sc-core/sc-store/sc_config.h
/*! Initialize sc-memory configuration from specified configuration file
=======
#include "sc-core/sc-store/sc_types.h"

// Config, that contains all configuration options: ['<group>/<key>'] = <value>
typedef GHashTable sc_config;

/*! Initialize sc-memory configuration from specified configuration file
 * @param config Config table
>>>>>>> [tools][sc-server][sc-memory][config] Implement unversal configurator on C and C++:sc-server/src/utils/sc_config.h
 * @param file_path Path to configuration file
 */
sc_bool sc_config_initialize(sc_config ** config, sc_char const * file_path);

/*! Shutdown sc-memory configuration. Free allocated data.
 */
<<<<<<< HEAD:sc-memory/sc-core/sc-store/sc_config.h
void sc_config_shutdown();

/*! Return number of segments, that can be loaded into memory at one moment
 */
sc_uint32 sc_config_get_max_loaded_segments();

//! Returns file memory engine
const sc_char * sc_config_fm_engine();
=======
void sc_config_shutdown(sc_config * config);
>>>>>>> [tools][sc-server][sc-memory][config] Implement unversal configurator on C and C++:sc-server/src/utils/sc_config.h

// --- api for extensions ---
/*!
 * Returns string value of \p key in specified \p group
 * @param config Config table
 * @param group Name of configuration group
 * @param key Name of key to return value
 * @returns Returns the value associated with the key as string, or null if the key was not found of could not be
 * parsed.
 * @attention Returned value managed by sc_confing and shouldn't be freed
 */
<<<<<<< HEAD:sc-memory/sc-core/sc-store/sc_config.h
_SC_EXTERN const sc_char * sc_config_get_value_string(const sc_char * group, const sc_char * key);
=======
_SC_EXTERN sc_char * sc_config_get_value_string(sc_config * config, sc_char const * group, sc_char const * key);
>>>>>>> [tools][sc-server][sc-memory][config] Implement unversal configurator on C and C++:sc-server/src/utils/sc_config.h

/*!
 * Returns the value associated with the key as an integer, or 0 if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
<<<<<<< HEAD:sc-memory/sc-core/sc-store/sc_config.h
_SC_EXTERN sc_int sc_config_get_value_int(const sc_char * group, const sc_char * key);
=======
_SC_EXTERN sc_int sc_config_get_value_int(sc_config * config, sc_char const * group, sc_char const * key);
>>>>>>> [tools][sc-server][sc-memory][config] Implement unversal configurator on C and C++:sc-server/src/utils/sc_config.h

/*!
 * Returns the value associated with the key as a boolean, or SC_FALSE if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
<<<<<<< HEAD:sc-memory/sc-core/sc-store/sc_config.h
_SC_EXTERN sc_bool sc_config_get_value_boolean(const sc_char * group, const sc_char * key);
=======
_SC_EXTERN sc_bool sc_config_get_value_boolean(sc_config * config, sc_char const * group, sc_char const * key);
>>>>>>> [tools][sc-server][sc-memory][config] Implement unversal configurator on C and C++:sc-server/src/utils/sc_config.h

/*!
 * Returns the value associated with the key as a float, or 0.0 if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
<<<<<<< HEAD:sc-memory/sc-core/sc-store/sc_config.h
_SC_EXTERN float sc_config_get_value_float(const sc_char * group, const sc_char * key);
=======
_SC_EXTERN float sc_config_get_value_float(sc_config * config, sc_char const * group, sc_char const * key);
>>>>>>> [tools][sc-server][sc-memory][config] Implement unversal configurator on C and C++:sc-server/src/utils/sc_config.h

#endif
