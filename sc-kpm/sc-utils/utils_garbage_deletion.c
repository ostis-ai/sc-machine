/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils.h"
#include "utils_garbage_deletion.h"
#include "../sc-common/sc_keynodes.h"
#include "utils_keynodes.h"
#include "../sc-search/search_keynodes.h"

sc_result agent_garbage_delete(const sc_event * event, sc_addr arg)
{
  //! TODO: when sc-memory will start event in many threads (not in one), then need to check if element exists
  /// because it can be already deleted as an input or outout arc of element deleted in other thread
  /// Also object can be deleted by another agent, for that moment just this agent can delete objects,
  /// all other agents must to add objects to the set sc_garbage

  sc_addr addr;
  sc_type type;
  sc_addr set_addr;
  sc_addr element_addr;

  if (sc_memory_get_arc_end(s_garbage_ctx, arg, &addr) != SC_RESULT_OK)
    return SC_RESULT_ERROR_INVALID_STATE;

  if (sc_helper_check_arc(s_default_ctx, keynode_question_erase_element, addr, sc_type_arc_pos_const_perm) == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  sc_iterator5 * get_set_it = sc_iterator5_f_a_a_a_f_new(
      s_garbage_ctx, addr, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, keynode_rrel_1);

  if (sc_iterator5_next(get_set_it) == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_PARAMS;

  set_addr = sc_iterator5_value(get_set_it, 2);

  sc_iterator3 * set_it = sc_iterator3_f_a_a_new(s_garbage_ctx, set_addr, 0, 0);

  while (sc_iterator3_next(set_it) == SC_TRUE)
  {
    element_addr = sc_iterator3_value(set_it, 2);

    if (SC_ADDR_IS_EQUAL(element_addr, addr))
      continue;

    sc_iterator3 * unerase_it = sc_iterator3_f_a_f_new(
        s_garbage_ctx, keynode_init_memory_generated_structure, sc_type_arc_pos_const_perm, element_addr);
    if (sc_iterator3_next(unerase_it) == SC_FALSE)
      sc_memory_element_free(s_garbage_ctx, element_addr);
  }
  return SC_RESULT_OK;
}
