/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _utils_h_
#define _utils_h_

#include "sc-core/sc_memory.h"

extern sc_memory_context * s_erase_elements_ctx;

_SC_EXT_EXTERN sc_result
sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure);
_SC_EXT_EXTERN sc_uint32 sc_module_load_priority();
_SC_EXT_EXTERN sc_result sc_module_shutdown();

#endif
