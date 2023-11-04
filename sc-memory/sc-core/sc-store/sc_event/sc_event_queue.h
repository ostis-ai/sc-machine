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
  sc_hash_table * processes_segments_table;
  sc_monitor processes_monitor;
  sc_queue * deletable_events;  // events needed to be deleted after sc-memory shutdown
  sc_bool running;              // flag that determine if queue is running
  sc_monitor destroy_monitor;
  GThreadPool * thread_pool;  // thread pool that used for a workers
  sc_monitor pool_monitor;
} sc_event_queue;

//! Create new sc-event queue with user processors number
sc_event_queue * sc_event_queue_new_ext(sc_uint32 max_events_and_agents_threads);

//! Stop events processing
void sc_event_queue_stop_processing(sc_event_queue * queue);

//! Destroys event queue. It waits until all events in queue will be processed
void sc_event_queue_destroy_wait(sc_event_queue * queue);

//! Appends \p event to queue
void sc_event_queue_append(sc_event_queue * queue, sc_event * event, sc_addr edge, sc_addr other_el);

#endif
