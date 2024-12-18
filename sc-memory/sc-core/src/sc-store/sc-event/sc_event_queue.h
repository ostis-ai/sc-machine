/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_queue_h_
#define _sc_event_queue_h_

#include "sc-core/sc_memory_params.h"

#include "sc-core/sc_types.h"
#include "sc-core/sc-base/sc_mutex.h"
#include "sc-core/sc-base/sc_monitor.h"

#include "sc-store/sc-container/sc_hash_table.h"
#include "sc-store/sc-base/sc_monitor_private.h"

typedef sc_result (*sc_event_do_after_callback)(sc_memory_context const * ctx, sc_addr addr);

/*! Structure representing an sc-event emission manager.
 * @note This structure manages the asynchronous processing of sc-events using a thread pool.
 */
typedef struct
{
  ///< Boolean indicating whether sc-memory limit `max_events_and_agents_threads` by maximum physical core number.
  sc_bool limit_max_threads_by_max_physical_cores;
  sc_uint32 max_events_and_agents_threads;  ///< Maximum number of threads for processing events and agents.
  sc_queue deletable_events_subscriptions;  ///< Queue of sc-events subscriptions that need to be deleted after
                                            ///< sc-memory shutdown.
  sc_bool running;                          ///< Flag indicating whether the event emission manager is running.
  sc_monitor destroy_monitor;               ///< Monitor for synchronizing access to the destruction process.
  GThreadPool * thread_pool;                ///< Thread pool used for worker threads processing events.
  sc_monitor pool_monitor;                  ///< Monitor for synchronizing access to the thread pool.
  sc_hash_table * emitted_erase_events;  ///< Table that stores amount of active event subscriptions that were initiated
                                         ///< due to erasure of sc-element which sc-addr is stored as a key
} sc_event_emission_manager;

/*! Function that initializes an sc-event emission manager.
 * @param manager Pointer to the sc_event_emission_manager to be initialized.
 * @param params Pointer to the sc-memory params.
 * @note This function initializes the event emission manager, creating a thread pool and necessary monitors.
 */
void sc_event_emission_manager_initialize(sc_event_emission_manager ** manager, sc_memory_params const * params);

/*! Function that stops an sc-event emission manager.
 * @param manager Pointer to the sc_event_emission_manager to be stopped.
 * @note This function stops the event emission manager, preventing further processing of events.
 */
void sc_event_emission_manager_stop(sc_event_emission_manager * manager);

/*! Function that shuts down and frees resources associated with an sc-event emission manager.
 * @param manager Pointer to the sc_event_emission_manager to be shut down.
 * @note This function shuts down and frees resources associated with the event emission manager.
 */
void sc_event_emission_manager_shutdown(sc_event_emission_manager * manager);

/*! Function that adds an sc-event to the event emission manager for processing.
 * @param manager Pointer to the sc_event_emission_manager managing event emission.
 * @param event_subscription A pointer to sc-event subscription.
 * @param connector_addr A sc-address of added/removed sc-connector (just for specified events).
 * @param connector_type A sc-type of added/removed sc-connector (just for specified events).
 * @param other_addr A sc-address of the second sc-element of sc-connector. If \p subscription_addr is a source, then \p
 * other_addr is a target. If \p subscription_addr is a target, then \p other_addr is a source.
 * @param callback A pointer function that is executed after the execution of a function that was called on the
 * initiated event (it is used for events of erasing sc-connectors and sc-elements and event of changing link content).
 * @param event_addr An argument of callback.
 * @note This function adds an sc-event to the event emission manager for asynchronous processing.
 */
void _sc_event_emission_manager_add(
    sc_event_emission_manager * manager,
    sc_event_subscription * event_subscription,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr,
    sc_event_do_after_callback callback,
    sc_addr event_addr);

#endif
