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
  sc_mutex rw_mutex;              // Mutex for data protection
  sc_condition writer_condition;  // Condition variable for writers
  sc_condition reader_condition;  // Condition variable for readers
  sc_queue * queue;               // Queue of writers and readers
  sc_uint32 active_readers;       // Number of readers currently accessing the data
  sc_uint32 waiting_writers;      // Number of writers waiting to write
  sc_uint32 active_writer;        // Flag to indicate if a writer is writing
  sc_uint32 id;                   // Unique identifier of monitor
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
  sc_thread * thread;
  sc_request_type type;
  sc_condition condition;
} sc_request;

void _sc_monitor_global_init(sc_monitor_table * table);

void _sc_monitor_global_destroy(sc_monitor_table * table);

sc_monitor * sc_monitor_get_monitor_for_addr(sc_monitor_table * table, sc_addr addr);

void sc_monitor_init(sc_monitor * monitor);

void sc_monitor_destroy(sc_monitor * monitor);

void sc_monitor_acquire_read(sc_monitor * monitor);

void sc_monitor_release_read(sc_monitor * monitor);

void sc_monitor_acquire_write(sc_monitor * monitor);

void sc_monitor_release_write(sc_monitor * monitor);

void sc_monitor_acquire_read_n(sc_uint32 n, ...);

void sc_monitor_release_read_n(sc_uint32 n, ...);

void sc_monitor_acquire_write_n(sc_uint32 n, ...);

void sc_monitor_release_write_n(sc_uint32 n, ...);

#endif
