/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_event_queue_h_
#define _sc_event_queue_h_

#include "../../sc_memory_params.h"

#include "../sc_types.h"
#include "../sc-base/sc_mutex.h"
#include "../sc-container/sc-hash-table/sc_hash_table.h"
#include "../sc-base/sc_monitor.h"

/*! Structure representing an sc-event emission manager.
 * @note This structure manages the asynchronous processing of sc-events using a thread pool.
 */
typedef struct
{
  ///< Boolean indicating whether sc-memory limit `max_events_and_agents_threads` by maximum physical core number.
  sc_bool limit_max_threads_by_max_physical_cores;
  sc_uint32 max_events_and_agents_threads;  ///< Maximum number of threads for processing events and agents.
  sc_queue deletable_events;                ///< Queue of events that need to be deleted after sc-memory shutdown.
  sc_bool running;                          ///< Flag indicating whether the event emission manager is running.
  sc_monitor destroy_monitor;               ///< Monitor for synchronizing access to the destruction process.
  GThreadPool * thread_pool;                ///< Thread pool used for worker threads processing events.
  sc_monitor pool_monitor;                  ///< Monitor for synchronizing access to the thread pool.
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
 * @param event Pointer to the sc-event to be added for processing.
 * @param connector_addr sc-address representing the sc-connector associated with the event.
 * @param other_addr sc-address representing the other sc-element associated with the event.
 * @note This function adds an sc-event to the event emission manager for asynchronous processing.
 */
void _sc_event_emission_manager_add(
    sc_event_emission_manager * manager,
    sc_event * event,
    sc_addr user_addr,
    sc_addr connector_addr,
    sc_type connector_type,
    sc_addr other_addr);

#endif
