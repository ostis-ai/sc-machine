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

#include "sc_event_queue.h"

sc_event_queue* sc_event_queue_new()
{
    sc_event_queue *queue = g_new0(sc_event_queue, 1);
    queue->deleted = SC_FALSE;
    g_mutex_init(&queue->mutex);
    queue->queue = g_queue_new();

    return queue;
}

void sc_event_queue_destroy(sc_event_queue *queue)
{
    g_assert(queue != 0);


}

void sc_event_queue_destroy_wait(sc_event_queue *queue)
{
}
