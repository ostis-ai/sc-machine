/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils.h"
#include "utils_keynodes.h"
#include "../sc-common/sc_keynodes.h"
#include <glib.h>

sc_addr keynode_sc_garbage;
sc_addr keynode_question_erase_element;
sc_addr keynode_init_memory_generated_structure;

const char keynode_sc_garbage_str[] = "sc_garbage";
const char keynode_question_erase_element_str[] = "question_erase_element";

sc_result utils_keynodes_initialize(sc_bool const init_memory_generated_upload, sc_char const * init_memory_generated_structure)
{
  RESOLVE_KEYNODE(s_default_ctx, keynode_sc_garbage);
  RESOLVE_KEYNODE(s_default_ctx, keynode_question_erase_element);

  sc_common_resolve_keynode(s_default_ctx, init_memory_generated_structure, &keynode_init_memory_generated_structure);

  return SC_RESULT_OK;
}
