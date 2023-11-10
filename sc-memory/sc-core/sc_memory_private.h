/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_private_h_
#define _sc_memory_private_h_

#include "sc-store/sc-base/sc_message.h"

#define SC_MEMORY_PREFIX "[sc-memory] "
#define sc_memory_info(...) sc_message(SC_MEMORY_PREFIX __VA_ARGS__)
#define sc_memory_warning(...) sc_warning(SC_MEMORY_PREFIX __VA_ARGS__)
#define sc_memory_error(...) sc_critical(SC_MEMORY_PREFIX __VA_ARGS__)

#endif
