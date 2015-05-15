/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_translators_h_
#define _ui_translators_h_

#include "uiTypes.h"

extern "C"
{
#include "sc_memory_headers.h"
}

//! Initialize all translator operations
void ui_initialize_translators();

//! Shutdown all translator operations
void ui_shutdown_translators();

/*! Function, that determine output format and input construction for translate command
 * @param cmd_addr sc-addr of node that designate translation command
 * @param output_fmt_addr Pointer to resolved format sc-addr
 * @param source_addr Pointer to resolved source construction sc-addr
 * @return If format and source construction resolved, then return SC_RESULT_OK;
 * otherwise return SC_RESULT_ERROR code
 */
sc_result ui_translate_command_resolve_arguments(sc_addr cmd_addr, sc_addr *output_fmt_addr, sc_addr *source_addr);

/*! Resolve system identifier of specified sc-elmenet. This function
 * tries to find system identifier and return string, that represent it.
 * If it doesn't exist, then this function returns generated string "segment_offset"
 * based on sc-element
 * @param el sc-addr of sc-element ot resolve system identifier
 * @param sys_idtf Reference to string that will contain result.
 * @return If system identifier was founeded, then return SC_TRUE. If it was generated,
 * then return SC_FALSE. If sys_idtf is null, then there are some errors in function.
 */
sc_bool ui_translate_resolve_system_identifier(sc_addr el, String &sys_idtf);

#endif // _ui_translators_h_
