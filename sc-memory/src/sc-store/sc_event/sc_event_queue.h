/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _sc_event_queue_h_
#define _sc_event_queue_h_

#include "sc_types.h"
#include <glib.h>

struct _sc_event_queue
{
    GQueue *queue;
    GThread *thread;
    GRecMutex mutex;
    sc_bool running;    // flag that determine if queue is running
    sc_event *event_process;    // currently processing event
    GRecMutex proc_mutex;   // mutex to lock event for process
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
