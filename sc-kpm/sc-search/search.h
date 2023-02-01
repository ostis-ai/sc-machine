/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_h_
#define _search_h_

#include "sc-core/sc_memory.h"

extern sc_memory_context * s_default_ctx;

_SC_EXT_EXTERN sc_result sc_module_initialize();

_SC_EXT_EXTERN sc_result sc_module_shutdown();

#endif
