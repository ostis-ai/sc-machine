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

#ifndef _sc_event_h_
#define _sc_event_h_

#include "sc_types.h"

/*! Event callback function type.
 * It takes 3 parameters:
 * - pointer to emited event description
 * - sc-addr event argument (output/intput arc for example). It depends on event type.
 * So it can be empty.
 */
typedef sc_result (*fEventCallback)(const sc_event *event, sc_addr arg);
//! Delete listened element callback function type
typedef sc_result (*fDeleteCallback)(const sc_event *event);


/*! Subscribe for events from specified sc-element
 * @param el sc-addr of subscribed sc-element events
 * @param type Type of listening sc-events
 * @param data Pointer to user data
 * @param callback Pointer to callback function. It would be calls, when event emited
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletio
 * @return Returns pointer to created sc-event
 * @remarks Callback functions can be called from any thred, so they need to be a thread safe
 */
sc_event* sc_event_new(sc_memory_context *ctx, sc_addr el, sc_event_type type, sc_pointer data, fEventCallback callback, fDeleteCallback delete_callback);

/*! Destroys specified sc-event
 * @param event Poitner to sc-event, that need to be destroyed
 * @return If event destoyed correctly, then return SC_OK; otherwise return SC_ERROR code.
 */
sc_result sc_event_destroy(sc_event *event);

/*! Returns type of specified sc-event
 */
sc_event_type sc_event_get_type(const sc_event *event);

//! Returns data of specified sc-event
sc_pointer sc_event_get_data(const sc_event *event);

//! Returns sc-addr of sc-element where event subscribed
sc_addr sc_event_get_element(const sc_event *event);

#endif // SC_EVENT_H
