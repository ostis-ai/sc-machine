/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_monitor_h_
#define _sc_monitor_h_

#include "sc_mutex.h"
#include "sc_condition.h"
#include "sc_thread.h"

#include "../sc_types.h"
#include "../sc-container/sc-hash-table/sc_hash_table.h"
#include "../sc-container/sc-queue/sc_queue.h"

typedef struct
{
  sc_mutex rw_mutex;          // Mutex for data protection
  sc_queue * queue;           // Queue of writers and readers
  sc_uint32 active_readers;   // Number of readers currently accessing the data
  sc_uint32 waiting_writers;  // Number of writers waiting to write
  sc_uint32 active_writer;    // Flag to indicate if a writer is writing
  sc_uint32 id;               // Unique identifier of monitor
} sc_monitor;

typedef struct
{
  sc_hash_table * monitors;             // Hash table storing addr_monitors_table for each identifier;
  sc_uint32 global_monitor_id_counter;  // Monitors count
  sc_mutex rw_mutex;                    // Mutex for data protection
} sc_monitor_table;

typedef enum
{
  READER,
  WRITER
} sc_request_type;

typedef struct
{
  sc_thread * thread;      // Thread instance of writer or reader
  sc_request_type type;    // READER or WRITER
  sc_condition condition;  // Condition variable of writer or reader
} sc_request;

/*! Initializes the global monitor table
 * @param table Pointer to the sc_monitor_table to be initialized
 * @remarks This function prepares the monitor table for use (for internal usage)
 */
_SC_EXTERN void _sc_monitor_global_init(sc_monitor_table * table);

/*! Destroys the global monitor table
 * @param table Pointer to the sc_monitor_table to be destroyed
 * @remarks This function cleans up the monitor table and its resources (for internal usage)
 */
_SC_EXTERN void _sc_monitor_global_destroy(sc_monitor_table * table);

/*! Fetches or creates a monitor for a specific address
 * @param table Pointer to the sc_monitor_table
 * @param addr Address for which a monitor should be fetched or created
 * @return Returns pointer to the associated sc_monitor
 * @remarks If a monitor for the address does not exist, one is created
 */
_SC_EXTERN sc_monitor * sc_monitor_get_monitor_for_addr(sc_monitor_table * table, sc_addr addr);

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

_SC_EXTERN sc_bool sc_monitor_try_acquire_write(sc_monitor * monitor);

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
