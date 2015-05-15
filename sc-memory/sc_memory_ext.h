/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
