/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_queue_h_
#define _sc_event_queue_h_

#include "../sc_types.h"
#include <glib.h>

struct _sc_event_queue
{
  GMutex mutex;
  sc_bool running;            // flag that determine if queue is running
  GRecMutex proc_mutex;       // mutex to lock event for process
  GThreadPool * thread_pool;  // thread pool that used for a workers
};

struct _sc_event_queue_item
{
  sc_event * event;
  sc_addr edge;
  sc_addr other_el;
};

typedef struct _sc_event_queue sc_event_queue;
typedef struct _sc_event_queue_item sc_event_queue_item;

//! Create new sc-event queue
sc_event_queue * sc_event_queue_new();

//! Stop events processing
void sc_event_queue_stop_processing(sc_event_queue * queue);

//! Destroys event queue. It waits until all events in queue will be processed
void sc_event_queue_destroy_wait(sc_event_queue * queue);

//! Appends \p event to queue
void sc_event_queue_append(sc_event_queue * queue, sc_event * event, sc_addr edge, sc_addr other_el);

#endif
