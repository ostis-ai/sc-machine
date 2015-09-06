/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_h_
#define _ui_h_

extern "C"
{
#include "sc_memory_headers.h"
}

extern "C"
{
// --------------- Module ----------------
_SC_EXT_EXTERN sc_result initialize();
_SC_EXT_EXTERN sc_result shutdown();
}

#endif
