/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_context_private_h_
#define _sc_memory_context_private_h_

#include "sc-store/sc_types.h"
#include "sc-store/sc-base/sc_monitor.h"

/*! Structure representing a memory context.
 * @note This structure represents a memory context associated with a specific user in the sc-memory.
 */
struct _sc_memory_context
{
  sc_addr user_addr;                 ///< sc_addr representing the user associated with the memory context.
  sc_uint32 ref_count;               ///< Reference count to manage the number of references to the memory context.
  sc_access_levels access_levels;    ///< Access levels for the memory context.
  sc_uint8 flags;                    ///< Flags indicating the state of the memory context.
  sc_hash_table_list * pend_events;  ///< List of pending events to be emitted in the memory context.
  sc_monitor monitor;                ///< Monitor for synchronizing access to the memory context.
};

#endif
