/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_private_h_
#define _sc_event_private_h_

#include "../sc_types.h"


/*! Structure that contains information about event
 */
struct _sc_event
{
    //! Pointer to context that create event
    sc_memory_context const * ctx;
    //! sc-addr of listened sc-element
    sc_addr element;
    //! Event type
    sc_event_type type;
    //! Pointer to user data
    sc_pointer data;
    //! Pointer to callback function, that calls, when event emited
    fEventCallback callback;
    //! Pointer to callback function, that calls, when subscribed sc-element deleted
    fDeleteCallback delete_callback;
    //! Reference count (just references from queue)
    sc_uint32 ref_count;
};


//! Function to initialize sc-events module
sc_bool sc_events_initialize();

//! Function to shutdown sc-events module
void sc_events_shutdown();

//! Waits while all emited events will be processed, then returns. After calling that function all new emited events will be ignored
void sc_events_stop_processing();

/*! Notificate about sc-element deletion.
 * @param element sc-addr of deleted sc-element
 * @remarks This function call deletion callback function for event.
 * And destroy all events for deleted sc-element
 */
sc_result sc_event_notify_element_deleted(sc_addr element);

/*! Emit event with \p type for sc-element \p el with argument \p arg
 * @param el sc-addr of element that emitting event
 * @param el_access Acces level of \p el
 * @param type Emitting event type
 * @param arg Argument of emitting event (depend of event type)
 * @return If event emitted without any errors, then return SC_OK; otherwise return SC_ERROR code
 */
sc_result sc_event_emit(sc_addr el, sc_access_levels el_acces, sc_event_type type, sc_addr arg);

#endif
