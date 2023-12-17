/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include "sc-core/sc-store/sc-base/sc_message.h"
#include "sc-core/sc-store/sc_iterator3.h"

void _sc_common_add_to_init_generated_memory_structure(
    sc_memory_context * ctx,
    sc_addr keynode,
    sc_addr const init_memory_generated_structure)
{
  if (SC_ADDR_IS_EMPTY(init_memory_generated_structure))
    return;
  sc_iterator3 * it3 =
      sc_iterator3_f_a_f_new(ctx, init_memory_generated_structure, sc_type_arc_pos_const_perm, keynode);
  if (sc_iterator3_next(it3) == SC_FALSE)
    sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, keynode);
  sc_iterator3_free(it3);
}

sc_result sc_common_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_addr * keynode,
    sc_addr const init_memory_generated_structure)
{
  return sc_helper_resolve_system_identifier(ctx, sys_idtf, keynode);
}
