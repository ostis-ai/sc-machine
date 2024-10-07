/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_monitor_table_private_h_
#define _sc_monitor_table_private_h_

#include "sc_monitor_table.h"

#include "sc-store/sc-container/sc_hash_table.h"

#include "sc_mutex_private.h"

typedef pthread_t sc_cleaner;

struct _sc_monitor_table
{
  sc_bool is_working;
  sc_hash_table * monitors;             // Hash table storing addr_monitors_table for each identifier;
  sc_uint32 global_monitor_id_counter;  // Monitors count
  sc_mutex rw_mutex;                    // Mutex for data protection
  sc_cleaner cleaner;
};

#endif
