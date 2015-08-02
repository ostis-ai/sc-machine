/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _utils_collect_identifiers_h_
#define _utils_collect_identifiers_h_

#if defined (SC_PLATFORM_WIN)
#define INCL_WINSOCK_API_PROTOTYPES 0
#include <WinSock2.h>
#endif

#include "sc_memory_headers.h"
#include <hiredis/hiredis.h>

sc_result utils_collect_identifiers_initialize();

sc_result utils_collect_identifiers_shutdown();

#endif
