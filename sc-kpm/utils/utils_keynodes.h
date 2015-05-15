/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _utils_keynodes_h_
#define _utils_keynodes_h_

#include "sc_memory.h"

extern sc_addr keynode_nrel_idtf;
extern sc_addr keynode_nrel_main_idtf;
extern sc_addr keynode_nrel_system_identifier;
extern sc_addr keynode_system_element;

extern sc_addr keynode_sc_garbage;

sc_result utils_collect_keynodes_initialize();

sc_result utils_keynodes_initialize();

#endif
