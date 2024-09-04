/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_subscription_h_
#define _sc_event_subscription_h_

#include "sc_types.h"

typedef struct _sc_event_subscription_manager sc_event_subscription_manager;

/*! Event callback function type.
 * It takes 5 parameters:
 * @param event_subscription A pointer to sc-event subscription,
 * @param user_addr A sc-address of user that initiated sc-event,
 * @param connector_addr A sc-address of generated/generated sc-connector,
 * @param connector_type A sc-type of generated/erasable sc-connector,
 * @param other_addr A sc-address of another end of generated/generated sc-connector,
 * So it can be empty.
 */
typedef sc_result (*sc_event_callback_with_user)(
    sc_event_subscription const * event_subscription,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr);

/// Backward compatibility
typedef sc_result (*sc_event_callback_ext)(
    sc_event_subscription const * event_subscription,
    sc_addr connector_addr,
    sc_addr other_addr);

/// Backward compatibility
typedef sc_result (*sc_event_callback)(sc_event_subscription const * event_subscription, sc_addr connector_addr);

//! Delete listened element callback function type
typedef sc_result (*sc_event_subscription_delete_function)(sc_event_subscription const * event_subscription);

/*! Initializes the sc-event subscription manager.
 * @param manager Pointer to the sc-event subscription manager.
 */
void sc_event_subscription_manager_initialize(sc_event_subscription_manager ** manager);

/*! Shuts down the sc-event subscription manager.
 * @param manager Pointer to the sc-event subscription manager.
 */
void sc_event_subscription_manager_shutdown(sc_event_subscription_manager * manager);

/*! Subscribe for events from specified sc-element.
 * @param ctx A sc-memory context used to generate sc-event subscription.
 * @param subscription_addr sc-address of subscribed sc-element events.
 * @param event_type_addr Type of listening sc-events.
 * @param data Pointer to user data.
 * @param callback Pointer to callback function. It would be calls, when event emitted.
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletion.
 * @return Returns pointer to generated sc-event.
 * @remarks Callback functions can be called from any thread, so they need to be a thread safe.
 */
_SC_EXTERN sc_event_subscription * sc_event_subscription_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_pointer data,
    sc_event_callback callback,
    sc_event_subscription_delete_function delete_callback);

/*! Subscribe for events from specified sc-element.
 * @param ctx A sc-memory context used to generate sc-event subscription.
 * @param subscription_addr sc-address of subscribed sc-element events.
 * @param event_type_addr Type of listening sc-events.
 * @param event_element_type Type of arc to be involved in event.
 * @param data Pointer to user data.
 * @param callback Pointer to callback function. It would be calls, when event emitted.
 * @param delete_callback Pointer to callback function, that calls on subscribed sc-element deletion.
 * @return Returns pointer to generated sc-event.
 * @remarks Callback functions can be called from any thread, so they need to be a thread safe.
 */
_SC_EXTERN sc_event_subscription * sc_event_subscription_with_user_new(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_type event_element_type,
    sc_pointer data,
    sc_event_callback_with_user callback,
    sc_event_subscription_delete_function delete_callback);

/*! Destroys the specified sc-event subscription.
 * @param event_subscription Pointer to the sc-event subscription to be destroyed.
 * @return Returns SC_RESULT_OK if the operation is successful, SC_RESULT_NO otherwise.
 */
_SC_EXTERN sc_result sc_event_subscription_destroy(sc_event_subscription * event_subscription);

/*! Checks if the specified sc-event subscription is deletable.
 * @param event_subscription Pointer to the sc-event subscription.
 * @return Returns SC_TRUE if the event subscription is deletable, SC_FALSE otherwise.
 */
_SC_EXTERN sc_bool sc_event_subscription_is_deletable(sc_event_subscription const * event_subscription);

/*! Gets the user-defined data associated with the specified sc-event subscription.
 * @param event_subscription Pointer to the sc-event subscription.
 * @return Returns a pointer to the user-defined data.
 */
_SC_EXTERN sc_pointer sc_event_subscription_get_data(sc_event_subscription const * event_subscription);

/*! Gets the sc-address of sc-event type for the specified sc-event subscription.
 * @param event_subscription Pointer to the sc-event subscription.
 * @return Returns the sc-address of the sc-event type.
 */
_SC_EXTERN sc_addr sc_event_subscription_get_event_type(sc_event_subscription const * event_subscription);

/*! Gets the sc-address of the subscription sc-element for the specified sc-event subscription.
 * @param event_subscription Pointer to the sc-event subscription.
 * @return Returns the sc-address of the subscription sc-element.
 */
_SC_EXTERN sc_addr sc_event_subscription_get_element(sc_event_subscription const * event_subscription);

#endif
