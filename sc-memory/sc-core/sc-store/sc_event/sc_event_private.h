/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_private_h_
#define _sc_event_private_h_

#include "sc_event_queue.h"

#include "../sc_event.h"
#include "../sc_types.h"
#include "../sc-base/sc_monitor.h"

/* Events life cycle:
 * - create event - set reference count to 1
 * - emit event - if there are no SC_EVENT_REQUEST_DESTROY flag, then ref sc_event and add it into pending queue
 * - destroy event - wait set flag SC_EVENT_REQUEST_DESTROY and wait until
 *   all pending calls of this event would be processed (ref count == 1).
 *   After that destroy event.
 */

#define SC_EVENT_REQUEST_DESTROY (1 << 31)

/*! Structure that contains information about event
 */
struct _sc_event
{
  //! sc-addr of listened sc-element
  sc_addr element;
  //! Event type
  sc_event_type type;
  //! Pointer to user data
  sc_pointer data;
  //! Pointer to callback function, that calls on event emit (for backward compatibility)
  fEventCallback callback;
  //! Pointer to callback function, that calls on event emit
  fEventCallbackEx callback_ex;
  //! Pointer to callback function, that calls, when subscribed sc-element deleted
  fDeleteCallback delete_callback;
  sc_monitor monitor;
  volatile sc_uint32 ref_count;
  //! Access levels
  sc_access_levels access_levels;
};

//! Function to initialize sc-events module with user processors number
sc_event_queue * sc_events_initialize_ext(sc_uint32 max_events_and_agents_threads);

//! Function to shutdown sc-events module
void sc_events_shutdown(sc_event_queue * event_queue);

//! Waits while all emitted events will be processed, then returns. After calling that function all new emitted events
//! will be ignored
void sc_events_stop_processing(sc_event_queue * event_queue);

/*! Notify about sc-element deletion.
 * @param element sc-addr of deleted sc-element
 * @remarks This function call deletion callback function for event.
 * And destroy all events for deleted sc-element
 */
sc_result sc_event_notify_element_deleted(sc_addr element);

/*! Emit event with \p type for sc-element \p el with argument \p arg.
 * If \ctx is in a pending mode, then event will be pend for emit
 * @param ctx pointer to context, that emits event
 * @param el sc-addr of element that emitting event
 * @param el_access Access level of \p el
 * @param type Emitting event type
 * @param edge sc-addr of added/remove edge (just for specified events)
 * @param other_el sc-addr of the second element of edge. If \p el is a source, then \p other_el is a target.
 * If \p el is a target, then \p other_el is a source.
 * @return If event emitted without any errors, then return SC_OK; otherwise return SC_ERROR code
 */
sc_result sc_event_emit(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_access_levels el_access,
    sc_event_type type,
    sc_addr edge,
    sc_addr other_el);

/*! Emit event immediately
 */
sc_result sc_event_emit_impl(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_access_levels el_access,
    sc_event_type type,
    sc_addr edge,
    sc_addr other_el);

#endif
