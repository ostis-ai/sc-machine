/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_ext_h_
#define _sc_memory_ext_h_

#include "sc-core/sc_types.h"

/*!
 * @brief Initializes extensions from multiple directories.
 *
 * This function initializes all extensions found in the provided list of directories.
 * It checks if module loading is supported on the current platform and proceeds to load
 * each directory's extensions.
 *
 * This function scans the given directories for dynamic modules (extensions), loads them,
 * and initializes them. It handles module loading, symbol resolution for initialization and
 * shutdown functions, and maintains a priority list for module initialization order.
 *
 * @param extension_directories An array of paths to directories containing extension modules.
 * @param extension_directories_count Number of directories in the extension_directories array.
 * @param enabled_list A list of enabled extensions (currently unused).
 * @param init_memory_generated_structure_addr The address of the initial memory structure
 *        that will be passed to the initialization functions of the loaded modules.
 * @return SC_RESULT_OK on success, or an error code indicating the type of failure.
 */
sc_result sc_ext_initialize(
    sc_char const ** extension_directories,
    sc_uint32 const extension_directories_count,
    sc_char const ** enabled_list,
    sc_addr init_memory_generated_structure);

/*! Shutdown all loaded extensions
 */
void sc_ext_shutdown();

#endif  // _sc_memory_ext_h_
