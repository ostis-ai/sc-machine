/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils.h"
#include "utils_keynodes.h"
#include "../sc-common/sc_keynodes.h"

sc_addr keynode_question_erase_elements;
sc_addr keynode_init_memory_generated_structure;

const char keynode_question_erase_elements_str[] = "question_erase_elements";

sc_result utils_keynodes_initialize(sc_addr const init_memory_generated_structure)
{
  RESOLVE_KEYNODE(s_erase_elements_ctx, keynode_question_erase_elements, init_memory_generated_structure);
  keynode_init_memory_generated_structure = init_memory_generated_structure;

  return SC_RESULT_OK;
}
