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

#ifndef _sc_event_h_
#define _sc_event_h_

#include "sc_types.h"

//! Event callback function type
typedef sc_result (*fEventCallback)(sc_event *event);

/*! Structure that contains information about event
 */
struct _sc_event
{
    //! sc-addr of listened sc-element
    sc_addr element;
    //! Event type
    sc_event_type type;
    //! Event id
    sc_uint32 id;
    //! Pointer to callback function, that calls, when event emited
    fEventCallback callback;
    //! Pointer to callback function, that calls, when subscribed sc-element deleted
    fEventCallback delete_callback;
};

/*! Subscribe for events from specified sc-element
 * @param el sc-addr of subscribed sc-element events
 * @param type Type of listening sc-events
 * @param id Event id, that will be used to determine event. Usualy need, when used one callback function
 * for many events.
 * @param callback Pointer to callback function. It would be calls, when event emited
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletion
 * @return Returns pointer to created sc-event
 * @remarks Callback functions can be called from any thred, so they need to be a thread safe
 */
sc_event* sc_event_new(sc_addr el, sc_event_type type, sc_uint32 id, fEventCallback callback, fEventCallback delete_callback);

/*! Emit output arc adding event
 * @param el sc-addr of subscribed sc-element
 * @param arc_addr sc-addr of added output arc
 * @returns If there are no any errors, then return SC_OK; otherwise return error code
 */
sc_result sc_event_emit_add_output_arc(sc_addr el, sc_addr arc_addr);

/*! Emit input arc adding event
 * @param el sc-addr of subscribed sc-element
 * @param arc_addr sc-addr of added input arc
 * @returns If there are no any errors, then return SC_OK; otherwise return error code
 */
sc_result sc_event_emit_add_input_arc(sc_addr el, sc_addr arc_addr);

/*! Emit output arc removing event
 * @param el sc-addr of subscribed sc-element
 * @param arc_addr sc-addr of removing output arc
 * @returns If there are no any errors, then return SC_OK; otherwise return error code
 */
sc_result sc_event_emit_remove_output_arc(sc_addr el, sc_addr arc_addr);

/*! Emit input arc removing event
 * @param el sc-addr of subscribed sc-element
 * @param arc_addr sc-addr of removing input arc
 * @returns If there are no any errors, then return SC_OK; otherwise return error code
 */
sc_result sc_event_emit_remove_input_arc(sc_addr el, sc_addr arc_addr);

/*! Emit link content changing event
 * @param el sc-addr of sc-link, that has changed content
 * @returns If there are no any errors, then return SC_OK; otherwise return error code
 */
sc_result sc_event_emit_change_link_content(sc_addr el);


#endif // SC_EVENT_H
