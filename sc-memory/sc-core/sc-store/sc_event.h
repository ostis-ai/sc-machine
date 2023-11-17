/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_h_
#define _sc_event_h_

#include "sc_event/sc_event_types.h"

typedef struct _sc_event_registration_manager sc_event_registration_manager;

/*! Event callback function type.
 * It takes 3 parameters:
 * - pointer to emitted event description
 * - sc-address of added/removed sc-connector
 * - sc-address of another end of added/removed sc-connector
 * So it can be empty.
 */
typedef sc_result (*sc_event_callback_with_user)(
    sc_event const * event,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr);

/// Backward compatibility
typedef sc_result (*sc_event_callback_ext)(sc_event const * event, sc_addr connector_addr, sc_addr other_addr);

/// Backward compatibility
typedef sc_result (*sc_event_callback)(sc_event const * event, sc_addr connector_addr);

//! Delete listened element callback function type
typedef sc_result (*sc_event_delete_function)(sc_event const * event);

/*! Initializes the sc-event registration manager.
 * @param manager Pointer to the sc-event registration manager.
 */
void sc_event_registration_manager_initialize(sc_event_registration_manager ** manager);

/*! Shuts down the sc-event registration manager.
 * @param manager Pointer to the sc-event registration manager.
 */
void sc_event_registration_manager_shutdown(sc_event_registration_manager * manager);

/*! Subscribe for events from specified sc-element
 * @param subscription_addr sc-address of subscribed sc-element events
 * @param type Type of listening sc-events
 * @param data Pointer to user data
 * @param callback Pointer to callback function. It would be calls, when event emitted
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletion
 * @return Returns pointer to created sc-event
 * @remarks Callback functions can be called from any thread, so they need to be a thread safe
 */
_SC_EXTERN sc_event * sc_event_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_pointer data,
    sc_event_callback callback,
    sc_event_delete_function delete_callback);

/*! Subscribe for events from specified sc-element
 * @param subscription_addr sc-address of subscribed sc-element events
 * @param type Type of listening sc-events
 * @param data Pointer to user data
 * @param callback Pointer to callback function. It would be calls, when event emitted
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletion
 * @return Returns pointer to created sc-event
 * @remarks Callback functions can be called from any thread, so they need to be a thread safe
 */
_SC_EXTERN sc_event * sc_event_new_ex(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_pointer data,
    sc_event_callback_ext callback,
    sc_event_delete_function delete_callback);

_SC_EXTERN sc_event * sc_event_with_user_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type type,
    sc_pointer data,
    sc_event_callback_with_user callback,
    sc_event_delete_function delete_callback);

/*! Destroys the specified sc-event.
 * @param event Pointer to the sc-event to be destroyed.
 * @return Returns SC_RESULT_OK if the operation is successful, SC_RESULT_NO otherwise.
 */
_SC_EXTERN sc_result sc_event_destroy(sc_event * event);

/*! Checks if the specified sc-event is deletable.
 * @param event Pointer to the sc-event.
 * @return Returns SC_TRUE if the event is deletable, SC_FALSE otherwise.
 */
_SC_EXTERN sc_bool sc_event_is_deletable(sc_event const * event);

/*! Gets the user-defined data associated with the specified sc-event.
 * @param event Pointer to the sc-event.
 * @return Returns a pointer to the user-defined data.
 */
_SC_EXTERN sc_pointer sc_event_get_data(sc_event const * event);

/*! Gets the sc-address of the associated sc-element for the specified sc-event.
 * @param event Pointer to the sc-event.
 * @return Returns the sc-addr of the associated sc-element.
 */
_SC_EXTERN sc_addr sc_event_get_element(sc_event const * event);

#endif
