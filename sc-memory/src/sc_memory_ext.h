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

#ifndef _sc_memory_ext_h_
#define _sc_memory_ext_h_

#include "sc-store/sc_types.h"
#include <gmodule.h>

/*! Initialize extensions from specified directory.
 * This function find all available extensions in specified directory and try to load them.
 * @param ext_dir_path Path to directory, that contains extensions. This function doens't take
 * ownership on this parameter, so you need to free it after end using the last one.
 * @return If specified directory doesn't exist, then return SC_ERROR_INVALID_PARAMS. If
 * there are any other errors to load extensions, then return SC_ERROR
 */
sc_result sc_ext_initialize(const sc_char *ext_dir_path);

/*! Shutdown all loaded extensions
 */
void sc_ext_shutdown();

#endif // _sc_memory_h_
