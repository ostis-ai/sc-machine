/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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
typedef sc_result (*fEventCallback)(sc_event *event, sc_addr arg);
//! Delete listened element callback function type
typedef sc_result (*fDeleteCallback)(sc_event *event);

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
    fDeleteCallback delete_callback;
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
sc_event* sc_event_new(sc_addr el, sc_event_type type, sc_uint32 id, fEventCallback callback, fDeleteCallback delete_callback);

/*! Destroys specified sc-event
 * @param event Poitner to sc-event, that need to be destroyed
 * @return If event destoyed correctly, then return SC_OK; otherwise return SC_ERROR code.
 */
sc_result sc_event_destroy(sc_event *event);

/*! Notificate about sc-element deletion.
 * @param element sc-addr of deleted sc-element
 * @remarks This function call deletion callback function for event.
 * And destroy all events for deleted sc-element
 */
sc_result sc_event_notify_element_deleted(sc_addr element);

/*! Emit event with \p type for sc-element \p el with argument \p arg
 * @param el sc-addr of element that emitting event
 * @param type emitting event type
 * @param arg argument of emitting event (depend of event type)
 * @return If event emitted without any errors, then return SC_OK; otherwise return SC_ERROR code
 */
sc_result sc_event_emit(sc_addr el, sc_event_type type, sc_addr arg);



#endif // SC_EVENT_H
