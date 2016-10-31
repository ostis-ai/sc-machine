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
    GQueue *queue;
    GThread *thread;
    GRecMutex mutex;
    sc_bool running;    // flag that determine if queue is running
    sc_event *event_process;    // currently processing event
    GRecMutex proc_mutex;   // mutex to lock event for process
    GThreadPool *thread_pool;	// thread pool that used for a workers
};


struct _sc_event_queue_item
{
    sc_event *event;
    sc_addr arg;
};

typedef struct _sc_event_queue sc_event_queue;
typedef struct _sc_event_queue_item sc_event_queue_item;

//! Create new sc-event queue
sc_event_queue* sc_event_queue_new();

//! Destroys event queue. It waits until all events in queue will be processed
void sc_event_queue_destroy_wait(sc_event_queue *queue);

//! Appends \p event to queue
void sc_event_queue_append(sc_event_queue *queue, sc_event *event, sc_addr arg);

/*! Removes event from queue. This function removes all events from queue that
 * equal to \p event
 */
void sc_event_queue_remove(sc_event_queue *queue, sc_event *event);

//! Delete events with specified element from queue
void sc_event_queue_remove_element(sc_event_queue *queue, sc_addr addr);


#endif
