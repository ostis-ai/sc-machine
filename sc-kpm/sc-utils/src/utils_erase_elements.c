/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils.h"
#include "utils_erase_elements.h"

#include <sc-common/sc_keynodes.h>
#include <sc-common/sc_utils.h>

#include "utils_keynodes.h"

/*!
 *  Erase sc-elements from memory if they dont belong to init memory structure
 */
sc_result agent_erase_elements(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action_addr;

  if (sc_memory_get_arc_end(s_erase_elements_ctx, arg, &action_addr) != SC_RESULT_OK)
  {
    finish_action_unsuccessfully(s_erase_elements_ctx, action_addr);
    return SC_RESULT_ERROR_INVALID_STATE;
  }

  if (sc_helper_check_arc(s_erase_elements_ctx, keynode_action_erase_elements, action_addr, sc_type_const_perm_pos_arc)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  sc_iterator5 * get_set_it = sc_iterator5_f_a_a_a_f_new(
      s_erase_elements_ctx,
      action_addr,
      sc_type_const_perm_pos_arc,
      sc_type_node,
      sc_type_const_perm_pos_arc,
      keynode_rrel_1);

  if (sc_iterator5_next(get_set_it) == SC_FALSE)
  {
    sc_iterator5_free(get_set_it);
    finish_action_unsuccessfully(s_erase_elements_ctx, action_addr);
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  sc_addr set_addr = sc_iterator5_value(get_set_it, 2);
  sc_iterator5_free(get_set_it);

  sc_iterator3 * set_it = sc_iterator3_f_a_a_new(s_erase_elements_ctx, set_addr, 0, 0);
  while (sc_iterator3_next(set_it) == SC_TRUE)
  {
    sc_addr element_addr = sc_iterator3_value(set_it, 2);

    if (SC_ADDR_IS_EQUAL(element_addr, action_addr))
    {
      sc_iterator3_free(set_it);
      finish_action_unsuccessfully(s_erase_elements_ctx, action_addr);
      return SC_RESULT_ERROR;
    }

    sc_iterator3 * unerase_it = sc_iterator3_f_a_f_new(
        s_erase_elements_ctx, keynode_init_memory_generated_structure, sc_type_const_perm_pos_arc, element_addr);
    if (sc_iterator3_next(unerase_it) == SC_TRUE)
    {
      sc_iterator3_free(unerase_it);
      continue;
    }
    sc_iterator3_free(unerase_it);

    sc_type type;
    sc_memory_get_element_type(s_erase_elements_ctx, element_addr, &type);
    if (type & sc_type_arc_mask)
    {
      sc_addr begin_addr;
      sc_memory_get_arc_begin(s_erase_elements_ctx, element_addr, &begin_addr);
      if (SC_ADDR_IS_EQUAL(begin_addr, keynode_init_memory_generated_structure))
      {
        continue;
      }
    }

    sc_memory_element_free(s_erase_elements_ctx, element_addr);
  }

  sc_iterator3_free(set_it);
  // @TODO: edge from finish_action_successfully to action doesn't create
  finish_action_successfully(s_erase_elements_ctx, action_addr);
  return SC_RESULT_OK;
}
