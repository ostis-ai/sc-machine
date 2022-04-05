/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils.h"
#include "utils_keynodes.h"
#include "utils_garbage_deletion.h"

sc_memory_context * s_default_ctx = 0;
sc_memory_context * s_garbage_ctx = 0;

sc_event * event_garbage_deletion;

_SC_EXT_EXTERN sc_result sc_module_initialize()
{
  s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);
  s_garbage_ctx = sc_memory_context_new(sc_access_lvl_make_max);

  if (utils_keynodes_initialize() != SC_RESULT_OK)
    return SC_RESULT_ERROR;

  event_garbage_deletion =
      sc_event_new(s_default_ctx, keynode_sc_garbage, SC_EVENT_ADD_OUTPUT_ARC, 0, agent_garbage_delete, 0);
  if (event_garbage_deletion == null_ptr)
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_uint32 sc_module_load_priority()
{
  return 0;
}

_SC_EXT_EXTERN sc_result sc_module_shutdown()
{
  sc_result res = SC_RESULT_OK;

  if (event_garbage_deletion)
    sc_event_destroy(event_garbage_deletion);

  sc_memory_context_free(s_garbage_ctx);
  sc_memory_context_free(s_default_ctx);

  return res;
}
