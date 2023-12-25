/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_context_private_h_
#define _sc_memory_context_private_h_

#include "sc-store/sc_types.h"
#include "sc-store/sc-base/sc_monitor.h"

struct _sc_memory_context
{
  sc_addr user_addr;
  sc_uint32 ref_count;
  sc_access_levels access_levels;
  sc_uint8 flags;
  sc_hash_table_list * pend_events;
  sc_monitor monitor;
};

#endif
