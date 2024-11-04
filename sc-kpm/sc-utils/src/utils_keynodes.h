/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _utils_keynodes_h_
#define _utils_keynodes_h_

#include <sc-core/sc_memory.h>

extern sc_addr keynode_action_erase_elements;
extern sc_addr keynode_init_memory_generated_structure;

sc_result utils_keynodes_initialize(sc_addr const init_memory_generated_structure);

#endif
