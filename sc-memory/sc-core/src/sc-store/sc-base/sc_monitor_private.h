/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_monitor_private_h_
#define _sc_monitor_private_h_

#include "sc-core/sc-base/sc_monitor.h"

#include "sc-core/sc-container/sc_queue.h"

#include "sc-core/sc-base/sc_mutex_private.h"

struct _sc_monitor
{
  sc_mutex rw_mutex;         // Mutex for data protection
  sc_queue queue;            // Queue of writers and readers
  sc_uint32 active_readers;  // Number of readers currently accessing the data
  sc_uint32 active_writer;   // Flag to indicate if a writer is writing
  sc_uint32 id;              // Unique identifier of monitor
  sc_mutex ref_count_mutex;
  sc_uint32 ref_count;
};

#endif
