/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_common_keynodes_h_
#define _sc_common_keynodes_h_

#include <sc-core/sc_helper.h>
#include <sc-core/sc_memory.h>

#include "sc_common_types.h"

extern sc_addr keynode_action;
extern sc_addr keynode_action_initiated;
extern sc_addr keynode_action_finished;
extern sc_addr keynode_action_finished_successfully;
extern sc_addr keynode_action_finished_unsuccessfully;
extern sc_addr keynode_nrel_result;

extern sc_addr keynode_rrel_1;
extern sc_addr keynode_rrel_2;

extern sc_addr keynode_system_element;

sc_result sc_common_keynodes_initialize(sc_memory_context * context, sc_addr const init_memory_generated_structure);

_SC_KPM_EXTERN sc_result sc_common_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_addr init_memory_generated_structure,
    sc_addr * keynode);

#define RESOLVE_KEYNODE(ctx, keynode, init_memory_generated_structure) \
  ({ \
    if (sc_common_resolve_keynode(ctx, keynode##_str, init_memory_generated_structure, &keynode) != SC_RESULT_OK) \
      return SC_RESULT_ERROR; \
  })

#endif
