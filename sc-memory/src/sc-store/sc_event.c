/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

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
#include <glib.h>
#include "sc_event.h"

sc_event* sc_event_new(sc_addr el, sc_event_type type, sc_uint32 id, fEventCallback callback, fEventCallback delete_callback)
{
    sc_event *event = g_new0(sc_event, 1);
    event->element = el;
    event->type = type;
    event->id = id;
    event->callback = callback;
    event->delete_callback = delete_callback;

    // register created event

    return event;
}

sc_result sc_event_emit_add_output_arc(sc_addr el, sc_addr arc_addr)
{
    return SC_ERROR;
}

sc_result sc_event_emit_add_input_arc(sc_addr el, sc_addr arc_addr)
{
    return SC_ERROR;
}

sc_result sc_event_emit_remove_output_arc(sc_addr el, sc_addr arc_addr)
{
    return SC_ERROR;
}

sc_result sc_event_emit_remove_input_arc(sc_addr el, sc_addr arc_addr)
{
    return SC_ERROR;
}

sc_result sc_event_emit_change_link_content(sc_addr el)
{
    return SC_ERROR;
}
