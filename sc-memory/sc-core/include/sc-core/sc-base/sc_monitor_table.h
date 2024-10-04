/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_monitor_table_h_
#define _sc_monitor_table_h_

#include "sc-core/sc-base/sc_mutex.h"
#include "sc-core/sc-base/sc_condition.h"
#include "sc-core/sc-base/sc_thread.h"

#include "sc-core/sc-container/sc_hash_table.h"
#include "sc-core/sc-container/sc_queue.h"
#include "sc-core/sc-base/sc_monitor.h"

#include "sc-core/sc_types.h"

typedef pthread_t sc_cleaner;

typedef struct
{
  sc_bool is_working;
  sc_hash_table * monitors;             // Hash table storing addr_monitors_table for each identifier;
  sc_uint32 global_monitor_id_counter;  // Monitors count
  sc_mutex rw_mutex;                    // Mutex for data protection
  sc_cleaner cleaner;
} sc_monitor_table;

/*! Initializes the global monitor table
 * @param table Pointer to the sc_monitor_table to be initialized
 * @remarks This function prepares the monitor table for use (for internal usage)
 */
_SC_EXTERN void _sc_monitor_table_init(sc_monitor_table * table);

/*! Destroys the global monitor table
 * @param table Pointer to the sc_monitor_table to be destroyed
 * @remarks This function cleans up the monitor table and its resources (for internal usage)
 */
_SC_EXTERN void _sc_monitor_table_destroy(sc_monitor_table * table);

/*! Fetches or creates a monitor for a specific address
 * @param table Pointer to the sc_monitor_table
 * @param addr Address for which a monitor should be fetched or created
 * @return Returns pointer to the associated sc_monitor
 * @remarks If a monitor for the address does not exist, one is created
 */
_SC_EXTERN sc_monitor * sc_monitor_table_get_monitor_for_addr(sc_monitor_table * table, sc_addr addr);

_SC_EXTERN sc_monitor * sc_monitor_table_get_monitor_from_table(sc_monitor_table * table, sc_pointer key);

#endif
