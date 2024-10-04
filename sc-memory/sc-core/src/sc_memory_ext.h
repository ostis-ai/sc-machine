/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_ext_h_
#define _sc_memory_ext_h_

#include "sc-core/sc_types.h"

/*! Initialize extensions from specified directory.
 * This function find all available extensions in specified directory and try to load them.
 * @param ext_dir_path Path to directory, that contains extensions. This function doesn't take
 * ownership on this parameter, so you need to free it after end using the last one.
 * @param enabled_list Null terminated list of enabled extension names. If it null, then all extensions in a directory
 * will be loaded. Otherwise just extensions in a list will be loaded.
 * @return If specified directory doesn't exist, then return SC_ERROR_INVALID_PARAMS. If
 * there are any other errors to load extensions, then return SC_ERROR
 */
sc_result sc_ext_initialize(
    sc_char const * ext_path,
    sc_char const ** enabled_list,
    sc_addr init_memory_generated_structure);

/*! Shutdown all loaded extensions
 */
void sc_ext_shutdown();

#endif  // _sc_memory_ext_h_
