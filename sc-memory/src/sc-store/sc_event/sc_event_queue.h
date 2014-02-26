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
    GMutex mutex;
    sc_bool deleted;
};

typedef struct _sc_event_queue sc_event_queue;

//! Create new sc-event queue
sc_event_queue* sc_event_queue_new();

//! Destroys event queue. This function doesn't wait while all events in queue will be processed.
void sc_event_queue_destroy(sc_event_queue *queue);

//! Destroys event queue. It waits until all events in queue will be processed
void sc_event_queue_destroy_wait(sc_event_queue *queue);

#endif
