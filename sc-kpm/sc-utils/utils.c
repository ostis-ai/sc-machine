/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils.h"
#include "utils_keynodes.h"
#include "utils_erase_elements.h"
#include "../sc-search/search_keynodes.h"

#include "sc-core/sc_memory_context_manager.h"

sc_memory_context * s_erase_elements_ctx = 0;

sc_event * event_erase_elements;

_SC_EXT_EXTERN sc_result
sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure)
{
  s_erase_elements_ctx = s_memory_default_ctx;

  if (utils_keynodes_initialize(init_memory_generated_structure) != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  event_erase_elements = sc_event_new(
      s_erase_elements_ctx,
      keynode_question_initiated,
      SC_EVENT_ADD_OUTPUT_ARC,
      null_ptr,
      agent_erase_elements,
      null_ptr);
  if (event_erase_elements == null_ptr)
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_uint32 sc_module_load_priority()
{
  return 1000;
}

_SC_EXT_EXTERN sc_result sc_module_shutdown()
{
  sc_result res = SC_RESULT_OK;

  if (event_erase_elements)
    sc_event_destroy(event_erase_elements);

  return res;
}
