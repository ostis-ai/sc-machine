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


#endif

