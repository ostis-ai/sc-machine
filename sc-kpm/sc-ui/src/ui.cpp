/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ui.h"

#include "uiTranslators.h"
#include "uiCommands.h"
#include "uiKeynodes.h"

sc_memory_context * s_default_ctx = nullptr;

// ------------------- Module ------------------------------
_SC_EXT_EXTERN sc_result
sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure)
{
  s_default_ctx = s_memory_default_ctx;

  if (!initialize_keynodes(init_memory_generated_structure))
    return SC_RESULT_ERROR;

  ui_initialize_commands();
  ui_initialize_translators();

  return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_result sc_module_shutdown()
{
  ui_shutdown_translators();
  ui_shutdown_commands();

  return SC_RESULT_OK;
}
