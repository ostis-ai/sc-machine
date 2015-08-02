/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_config_h_
#define _sc_config_h_

#include "sc_types.h"

/*! Iniitalize sc-memory configuration from specified configuration file
 * @param file_path Path to configuration file
 */
void sc_config_initialize(const sc_char *file_path);

/*! Shutdown sc-memory configuration. Free allocated data.
 */
void sc_config_shutdown();

/*! Return number of segments, that can be loaded into memory at one moment
 */
sc_int32 sc_config_get_max_loaded_segments();

//! Returns file memory engine
const sc_char* sc_config_fm_engine();


// --- api for extensions ---
/*!
 * Returns string value of \p key in specified \p group
 * @param group Name of configuration group
 * @param key Name of key to return value
 * @returns Returns the value associated with the key as string, or null if the key was not found of could not be parsed.
 * @attention Returned value managed by sc_confing and shouldn't be freed
 */
_SC_EXTERN const char* sc_config_get_value_string(const char *group, const char *key);

/*!
 * Returns the value associated with the key as an integer, or 0 if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
_SC_EXTERN int sc_config_get_value_int(const char *group, const char *key);

/*!
 * Returns the value associated with the key as a boolean, or SC_FALSE if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
_SC_EXTERN sc_bool sc_config_get_value_boolean(const char *group, const char *key);

/*!
 * Returns the value associated with the key as a float, or 0.0 if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
_SC_EXTERN float sc_config_get_value_float(const char *group, const char *key);


#endif

