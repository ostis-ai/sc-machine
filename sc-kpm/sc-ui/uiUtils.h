/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _uiUtils_h_
#define _uiUtils_h_

#include "uiTypes.h"

extern "C"
{
#include "sc_memory_headers.h"
}

/*! Function, that check if specified ui command has an specified type
 * @param cmd_addr sc-addr of node that designate ui command
 * @param cmd_class sc-addr of node that designate specified class of commands
 * @return If command is exists in specified class, then return SC_RESULT_OK;
 * otherwise return SC_RESULT_ERROR code
 */
sc_result ui_check_cmd_type(sc_addr cmd_addr, sc_addr cmd_class);

#endif // _uiUtils_h_
