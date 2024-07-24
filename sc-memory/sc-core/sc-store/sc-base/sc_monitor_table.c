/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_monitor_table.h"
#include "sc_allocator.h"

#define SC_MONITOR_TABLE_CLEAN_SIZE_THRESHOLD 10000
#define SC_MONITOR_TABLE_CLEAN_INTERVAL_CHECK 10

void _sc_monitor_destroy(void * monitor)
{
  sc_monitor_destroy((sc_monitor *)monitor);
  sc_mem_free(monitor);
}

void * _sc_monitor_table_clean_periodic(void * arg)
{
  sc_monitor_table * table = arg;

  sc_hash_table * previous_monitors = null_ptr;

  while (table->is_working == SC_TRUE)
  {
    g_usleep(SC_MONITOR_TABLE_CLEAN_INTERVAL_CHECK);

    sc_mutex_lock(&table->rw_mutex);
    sc_uint64 const size = sc_hash_table_size(table->monitors);
    sc_mutex_unlock(&table->rw_mutex);
    if (size < SC_MONITOR_TABLE_CLEAN_SIZE_THRESHOLD)
      continue;

    if (previous_monitors != null_ptr)
      sc_hash_table_destroy(previous_monitors);

    sc_mutex_lock(&table->rw_mutex);
    table->global_monitor_id_counter = 1;
    previous_monitors = table->monitors;
    table->monitors = sc_hash_table_init(
        sc_hash_table_default_hash_func, sc_hash_table_default_equal_func, null_ptr, _sc_monitor_destroy);
    sc_mutex_unlock(&table->rw_mutex);
  }

  if (previous_monitors != null_ptr)
    sc_hash_table_destroy(previous_monitors);

  pthread_exit(null_ptr);
}

sc_cleaner _sc_monitor_table_create_cleaner(sc_monitor_table * table)
{
  sc_cleaner cleaner;
  pthread_create(&cleaner, null_ptr, _sc_monitor_table_clean_periodic, table);
  return cleaner;
}

void _sc_monitor_table_delete_cleaner(sc_cleaner cleaner)
{
  pthread_join(cleaner, null_ptr);
}

void _sc_monitor_table_init(sc_monitor_table * table)
{
  table->monitors = sc_hash_table_init(
      sc_hash_table_default_hash_func, sc_hash_table_default_equal_func, null_ptr, _sc_monitor_destroy);
  sc_mutex_init(&table->rw_mutex);
  table->global_monitor_id_counter = 1;
  table->is_working = SC_TRUE;
  table->cleaner = _sc_monitor_table_create_cleaner(table);
}

void _sc_monitor_table_destroy(sc_monitor_table * table)
{
  if (table->is_working == SC_TRUE)
  {
    table->is_working = SC_FALSE;
    _sc_monitor_table_delete_cleaner(table->cleaner);
  }

  sc_hash_table_destroy(table->monitors);
  sc_mutex_destroy(&table->rw_mutex);
  table->global_monitor_id_counter = 1;
}

sc_monitor * sc_monitor_table_get_monitor_for_addr(sc_monitor_table * table, sc_addr addr)
{
  sc_pointer key = (sc_addr_hash_to_sc_pointer)SC_ADDR_LOCAL_TO_INT(addr);
  return key == null_ptr ? null_ptr : sc_monitor_table_get_monitor_from_table(table, key);
}

sc_monitor * sc_monitor_table_get_monitor_from_table(sc_monitor_table * table, sc_pointer key)
{
  sc_mutex_lock(&table->rw_mutex);

  sc_monitor * monitor = (sc_monitor *)sc_hash_table_get(table->monitors, key);

  if (monitor == null_ptr)
  {
    monitor = sc_mem_new(sc_monitor, 1);
    sc_monitor_init(monitor);
    monitor->id = table->global_monitor_id_counter++;
    sc_hash_table_insert(table->monitors, key, monitor);
  }

  sc_mutex_unlock(&table->rw_mutex);

  return monitor;
}
