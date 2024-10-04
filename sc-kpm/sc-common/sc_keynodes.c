/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include "sc-core/sc_iterator3.h"

sc_result sc_common_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_addr init_memory_generated_structure,
    sc_addr * keynode)
{
  sc_system_identifier_fiver fiver;
  sc_result const result = sc_helper_resolve_system_identifier_ext(ctx, sys_idtf, sc_type_const_node, keynode, &fiver);

  if (SC_ADDR_IS_NOT_EMPTY(init_memory_generated_structure))
  {
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr1);
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr2);
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr3);
    sc_memory_arc_new(ctx, sc_type_const_perm_pos_arc, init_memory_generated_structure, fiver.addr4);
  }

  return result;
}
