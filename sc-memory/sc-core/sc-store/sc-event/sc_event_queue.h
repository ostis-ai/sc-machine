/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_queue_h_
#define _sc_event_queue_h_

#include "../sc_types.h"
#include "../sc-base/sc_mutex.h"
#include "../sc-container/sc-hash-table/sc_hash_table.h"
#include "../sc-base/sc_monitor.h"

typedef struct
{
  sc_queue * deletable_events;  // events needed to be deleted after sc-memory shutdown
  sc_bool running;              // flag that determine if queue is running
  sc_monitor destroy_monitor;
  GThreadPool * thread_pool;  // thread pool that used for a workers
  sc_monitor pool_monitor;
} sc_event_emission_manager;

//! Create new sc-event emitting manager with user processors number
void sc_event_emission_manager_initialize(
    sc_event_emission_manager ** manager,
    sc_uint32 max_events_and_agents_threads);

//! Stop sc-events emitting manager
void sc_event_emission_manager_stop(sc_event_emission_manager * manager);

//! Destroys sc-event emitting manager
void sc_event_emission_manager_shutdown(sc_event_emission_manager * manager);

//! Appends sc-event for emitting
void _sc_event_emission_manager_add(
    sc_event_emission_manager * manager,
    sc_event * event,
    sc_addr edge,
    sc_addr other_el);

#endif
