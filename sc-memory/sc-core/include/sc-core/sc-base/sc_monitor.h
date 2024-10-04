/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_monitor_h_
#define _sc_monitor_h_

#include "sc-core/sc_types.h"

#include "sc-core/sc-base/sc_mutex.h"
#include "sc-core/sc-base/sc_condition.h"

#include "sc-core/sc-container/sc_queue.h"

typedef struct _sc_request sc_request;
typedef struct _sc_monitor sc_monitor;
typedef struct _sc_monitor_table sc_monitor_table;

/*! Initializes a monitor instance
 * @param monitor Pointer to the sc_monitor to be initialized
 * @remarks This function prepares the monitor for use
 */
_SC_EXTERN void sc_monitor_init(sc_monitor * monitor);

/*! Destroys a monitor instance
 * @param monitor Pointer to the sc_monitor to be destroyed
 * @remarks This function cleans up the monitor and its resources
 */
_SC_EXTERN void sc_monitor_destroy(sc_monitor * monitor);

/*! Acquires a read lock on the specified monitor
 * @param monitor Pointer to the sc_monitor
 * @remarks This function blocks if a writer currently holds the lock
 */
_SC_EXTERN void sc_monitor_acquire_read(sc_monitor * monitor);

/*! Releases a read lock from the specified monitor
 * @param monitor Pointer to the sc_monitor
 */
_SC_EXTERN void sc_monitor_release_read(sc_monitor * monitor);

/*! Acquires a write lock on the specified monitor
 * @param monitor Pointer to the sc_monitor
 * @remarks This function blocks if another writer or any reader currently holds the lock
 */
_SC_EXTERN void sc_monitor_acquire_write(sc_monitor * monitor);

/*! Releases a write lock from the specified monitor
 * @param monitor Pointer to the sc_monitor
 */
_SC_EXTERN void sc_monitor_release_write(sc_monitor * monitor);

/*! Acquires read locks for multiple monitors
 * @param n Count of monitors
 * @param ... Variable argument list containing pointers to sc_monitors
 * @remarks This function acquires read locks for the specified number of monitors
 */
_SC_EXTERN void sc_monitor_acquire_read_n(sc_uint32 n, ...);

/*! Releases read locks from multiple monitors
 * @param n Count of monitors
 * @param ... Variable argument list containing pointers to sc_monitors
 * @remarks This function releases read locks for the specified number of monitors
 */
_SC_EXTERN void sc_monitor_release_read_n(sc_uint32 n, ...);

/*! Acquires write locks for multiple monitors
 * @param n Count of monitors
 * @param ... Variable argument list containing pointers to sc_monitors
 * @remarks This function acquires write locks for the specified number of monitors
 */
_SC_EXTERN void sc_monitor_acquire_write_n(sc_uint32 n, ...);

/*! Releases write locks from multiple monitors
 * @param n Count of monitors
 * @param ... Variable argument list containing pointers to sc_monitors
 * @remarks This function releases write locks for the specified number of monitors
 */
_SC_EXTERN void sc_monitor_release_write_n(sc_uint32 n, ...);

#endif
