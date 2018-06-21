/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_command_h_
#define _ui_command_h_

extern "C"
{
#include "sc_memory.h"
}

//! Setup event handler for command operations
sc_result ui_initialize_commands();

//! Destroy event handlers for command operations
void ui_shutdown_commands();

#endif // _ui_command_h_
