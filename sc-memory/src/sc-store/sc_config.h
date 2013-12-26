/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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
sc_uint32 sc_config_get_max_loaded_segments();

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
const char* sc_config_get_value_string(const char *group, const char *key);

/*!
 * Returns the value associated with the key as an integer, or 0 if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
int sc_config_get_value_int(const char *group, const char *key);

/*!
 * Returns the value associated with the key as a boolean, or SC_FALSE if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
sc_bool sc_config_get_value_boolean(const char *group, const char *key);

/*!
 * Returns the value associated with the key as a float, or 0.0 if the key was not found or could not be parsed.
 * @see sc_config_get_value_string
 */
float sc_config_get_value_float(const char *group, const char *key);


#endif

