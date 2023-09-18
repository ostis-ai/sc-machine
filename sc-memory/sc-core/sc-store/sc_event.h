/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_h_
#define _sc_event_h_

#include "sc_types.h"

/*! Event callback function type.
 * It takes 3 parameters:
 * - pointer to emitted event description
 * - sc-addr of added/remove edge
 * - sc-addr of another end of added/remove edge
 * So it can be empty.
 */
typedef sc_result (*fEventCallbackEx)(sc_event const * event, sc_addr arg, sc_addr other_el);

/// Backward compatibility
typedef sc_result (*fEventCallback)(sc_event const * event, sc_addr arg);

//! Delete listened element callback function type
typedef sc_result (*fDeleteCallback)(sc_event const * event);

/*! Subscribe for events from specified sc-element
 * @param el sc-addr of subscribed sc-element events
 * @param type Type of listening sc-events
 * @param data Pointer to user data
 * @param callback Pointer to callback function. It would be calls, when event emitted
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletion
 * @return Returns pointer to created sc-event
 * @remarks Callback functions can be called from any thread, so they need to be a thread safe
 */
SC_DEPRECATED("0.3.0", "Use sc_event_new_ex instead of this function. This function will be removed in next release.")
_SC_EXTERN sc_event * sc_event_new(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_event_type type,
    sc_pointer data,
    fEventCallback callback,
    fDeleteCallback delete_callback);

_SC_EXTERN sc_event * sc_event_new_ex(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_event_type type,
    sc_pointer data,
    fEventCallbackEx callback,
    fDeleteCallback delete_callback);

/*! Destroys specified sc-event
 * @param event Pointer to sc-event, that need to be destroyed
 * @return If event destroyed correctly, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR code.
 */
_SC_EXTERN sc_result sc_event_destroy(sc_event * event);

//! Returns data of specified sc-event
_SC_EXTERN sc_pointer sc_event_get_data(sc_event const * event);

//! Returns sc-addr of sc-element where event subscribed
_SC_EXTERN sc_addr sc_event_get_element(sc_event const * event);

#endif  // SC_EVENT_H
