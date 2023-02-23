/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include "sc-core/sc-store/sc-base/sc_message.h"
#include "sc-core/sc-store/sc_iterator3.h"

void add_to_init_generated_memory_structure(sc_memory_context* ctx, sc_addr keynode, sc_addr const init_memory_generated_structure)
{
  sc_iterator3 * it3 = sc_iterator3_f_a_f_new(
        ctx, init_memory_generated_structure, sc_type_arc_pos_const_perm, keynode);
    if (sc_iterator3_next(it3) == SC_FALSE)
      sc_memory_arc_new(ctx, sc_type_arc_pos_const_perm, init_memory_generated_structure, keynode);
}

sc_result sc_common_resolve_keynode(sc_memory_context * ctx, char const * sys_idtf, sc_addr * keynode, sc_addr const init_memory_generated_structure)
{
  if (sc_helper_resolve_system_identifier(ctx, sys_idtf, keynode) == SC_FALSE)
  {
    *keynode = sc_memory_node_new(ctx, sc_type_node | sc_type_const);
    sc_system_identifier_fiver fiver;
    if (sc_helper_set_system_identifier_ext(ctx, *keynode, sys_idtf, (sc_uint32)strlen(sys_idtf), &fiver) != SC_RESULT_OK)
      return SC_RESULT_ERROR;
    add_to_init_generated_memory_structure(ctx, fiver.addr1, init_memory_generated_structure);
    add_to_init_generated_memory_structure(ctx, fiver.addr2, init_memory_generated_structure);
    add_to_init_generated_memory_structure(ctx, fiver.addr3, init_memory_generated_structure);
    add_to_init_generated_memory_structure(ctx, fiver.addr4, init_memory_generated_structure);
    add_to_init_generated_memory_structure(ctx, fiver.addr5, init_memory_generated_structure);
    sc_message("Created element with system identifier: %s", sys_idtf);
  }
  return SC_RESULT_OK;
}
