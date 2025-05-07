/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_private_h_
#define _sc_event_private_h_

#include "sc_event_queue.h"

#include "sc-core/sc_event_subscription.h"
#include "sc-core/sc_types.h"
#include "sc-store/sc-base/sc_condition_private.h"  
#include "sc-core/sc-container/sc_list.h"


#define SC_EVENT_REQUEST_DESTROY (sc_uint32)(1 << 31)

//! Structure that contains information about event
struct _sc_event_subscription
{
  //! sc-addr of listened sc-element
  sc_addr subscription_addr;
  //! Event type
  sc_event_type event_type_addr;
  //! Connector type required to trigger the event
  sc_type event_element_type;
  //! Pointer to user data
  sc_pointer data;
  //! Pointer to callback function, that calls on event emit (for backward compatibility)
  sc_event_callback callback;
  //! Pointer to callback function, that calls on event emit
  sc_event_callback_with_user callback_with_user;
  //! Pointer to callback function, that calls, when subscribed sc-element deleted
  sc_event_subscription_delete_function delete_callback;
  //! Monitor used to synchronize state of fields of sc-event subscription
  sc_monitor monitor;
  //! Count of references (users) of this sc-event subscription
  sc_uint32 ref_count;
  
  //! flag: whether the event is complex
  sc_bool is_complex_event_subscription;
   //! flag for activation regulation
  int counter_of_activated_events;
  //! value of max activated events to activate complex event
  int max_value_of_activated_events;
  // counter of functions sc_event_emit and _sc_event_emission_pool_worker execution
  int execution_counter;
  //! condition for increasing the counter
  sc_condition cond_increase;
  //! condition for decreasing the counter
  sc_condition cond_decrease;
  
  //! Events list
  sc_list* events_list;
  
};


/*! Notify about sc-element deletion.
 * @param addr sc-address of deleted sc-element
 * @remarks This function call deletion callback function for event.
 * And destroy all events for deleted sc-element
 */
sc_result sc_event_notify_element_deleted(sc_addr addr);

/*! Emits event with \p type for sc-element \p subscription_addr with argument \p arg.
 * If \ctx is in a pending mode, then event will be pend for emit
 * @param ctx A pointer to context, that emits event
 * @param subscription_addr sc-addr of element that emitting event
 * @param event_type_addr Emitting event type
 * @param connector_addr A sc-address of added/removed sc-connector (just for specified events)
 * @param edge_type A sc-type of added/removed sc-connector (just for specified events)
 * @param other_addr A sc-address of the second sc-element of sc-connector. If \p subscription_addr is a source, then \p
 * other_addr is a target. If \p subscription_addr is a target, then \p other_addr is a source.
 * @param callback A pointer function that is executed after the execution of a function that was called on the
 * initiated event (it is used for events of erasing sc-connectors and sc-elements and event of changing link content).
 * @param event_addr An argument of callback.
 * @return If event emitted without any errors, then return SC_RESULT_OK; otherwise return SC_RESULT_ERROR code.
 */
sc_result sc_event_emit(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_addr connector_addr,
    sc_type edge_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr);

/*! Emits event immediately
 */
sc_result sc_event_emit_impl(
    sc_memory_context const * ctx,
    sc_addr subscription_addr,
    sc_event_type event_type_addr,
    sc_addr connector_addr,
    sc_type edge_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr);

#endif
