/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "utils_keynodes.h"
#include "../sc-common/sc_keynodes.h"
#include "utils.h"
#include <glib.h>

const char keynode_sc_garbage_str[] = "sc_garbage";

sc_addr keynode_sc_garbage;

sc_result utils_keynodes_initialize()
{
  RESOLVE_KEYNODE(s_default_ctx, keynode_sc_garbage);

  return SC_RESULT_OK;
}
