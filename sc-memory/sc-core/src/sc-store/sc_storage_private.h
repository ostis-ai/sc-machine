/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_storage_private_h_
#define _sc_storage_private_h_

#include "sc-store/sc-base/sc_monitor_table.h"

#include "sc-store/sc-event/sc_event_private.h"

#include "sc-store/sc_storage_dump_manager.h"

#include "sc-store/sc-base/sc_monitor_table_private.h"

struct _sc_storage
{
  sc_segment ** segments;
  sc_addr_seg segments_count;
  sc_addr_seg max_segments_count;
  sc_addr_seg last_not_engaged_segment_num;
  sc_addr_seg last_released_segment_num;
  sc_monitor segments_monitor;
  sc_monitor_table addr_monitors_table;
  sc_hash_table * processes_segments_table;
  sc_monitor processes_monitor;
  sc_storage_dump_manager * dump_manager;
  sc_event_emission_manager * events_emission_manager;
  sc_event_subscription_manager * events_subscription_manager;
};

struct _sc_storage * sc_storage_get();

sc_event_emission_manager * sc_storage_get_event_emission_manager();

sc_event_subscription_manager * sc_storage_get_event_subscription_manager();

sc_element * sc_storage_allocate_new_element(sc_memory_context const * ctx, sc_addr * addr);

sc_result sc_storage_get_element_by_addr(sc_addr addr, sc_element ** el);

sc_result sc_storage_free_element(sc_addr addr);

#endif
