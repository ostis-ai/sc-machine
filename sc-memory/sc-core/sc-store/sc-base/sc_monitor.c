/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_allocator.h"
#include "sc_monitor.h"

void sc_monitor_global_init(sc_monitor_table * table)
{
  table->monitors = g_hash_table_new_full(g_direct_hash, g_direct_equal, null_ptr, g_free);
  sc_mutex_init(&table->rw_mutex);
}

void sc_monitor_global_destroy(sc_monitor_table * table)
{
  g_hash_table_destroy(table->monitors);
  sc_mutex_destroy(&table->rw_mutex);
}

sc_monitor * sc_monitor_get_monitor_for_addr(sc_monitor_table * table, sc_addr addr)
{
  sc_mutex_lock(&table->rw_mutex);

  gpointer key = (gpointer)(sc_uint64)SC_ADDR_LOCAL_TO_INT(addr);
  sc_monitor * monitor = (sc_monitor *)g_hash_table_lookup(table->monitors, key);

  if (monitor == null_ptr)
  {
    monitor = sc_mem_new(sc_monitor, 1);
    sc_monitor_init(monitor);
    g_hash_table_insert(table->monitors, key, monitor);
  }

  sc_mutex_unlock(&table->rw_mutex);

  return monitor;
}

void sc_monitor_init(sc_monitor * monitor)
{
  sc_mutex_init(&monitor->rw_mutex);
  sc_cond_init(&monitor->reader_condition);
  sc_cond_init(&monitor->writer_condition);
  monitor->active_readers = 0;
  monitor->waiting_writers = 0;
  monitor->active_writer = 0;
}

void sc_monitor_start_read(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  while (monitor->active_writer || monitor->waiting_writers)
    sc_cond_wait(&monitor->reader_condition, &monitor->rw_mutex);

  ++monitor->active_readers;

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_end_read(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  --monitor->active_readers;
  if (monitor->active_readers == 0 && monitor->waiting_writers > 0)
    sc_cond_signal(&monitor->writer_condition);

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_start_write(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  ++monitor->waiting_writers;

  while (monitor->active_readers || monitor->active_writer)
    sc_cond_wait(&monitor->writer_condition, &monitor->rw_mutex);

  --monitor->waiting_writers;
  monitor->active_writer = 1;

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_end_write(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  monitor->active_writer = 0;

  if (monitor->waiting_writers > 0)
    sc_cond_signal(&monitor->writer_condition);
  else
    sc_cond_broadcast(&monitor->reader_condition);

  sc_mutex_unlock(&monitor->rw_mutex);
}
