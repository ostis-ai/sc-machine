/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_allocator.h"
#include "sc_monitor.h"

void _sc_monitor_destroy(void * monitor)
{
  sc_monitor_destroy((sc_monitor *)monitor);
  sc_mem_free(monitor);
}

void _sc_monitor_global_init(sc_monitor_table * table)
{
  table->monitors = g_hash_table_new_full(g_direct_hash, g_direct_equal, null_ptr, _sc_monitor_destroy);
  sc_mutex_init(&table->rw_mutex);
  table->global_monitor_id_counter = 0;
}

void _sc_monitor_global_destroy(sc_monitor_table * table)
{
  g_hash_table_destroy(table->monitors);
  sc_mutex_destroy(&table->rw_mutex);
  table->global_monitor_id_counter = 0;
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
    monitor->id = table->global_monitor_id_counter++;
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
  sc_queue_init(&monitor->queue);
}

void sc_monitor_destroy(sc_monitor * monitor)
{
  sc_mutex_destroy(&monitor->rw_mutex);
  sc_cond_destroy(&monitor->reader_condition);
  sc_cond_destroy(&monitor->writer_condition);
  monitor->active_readers = 0;
  monitor->waiting_writers = 0;
  monitor->active_writer = 0;
  monitor->id = 0;
  while (!sc_queue_empty(monitor->queue))
    ;
  sc_queue_destroy(monitor->queue);
}

void sc_monitor_acquire_read(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  sc_request * current_request = sc_mem_new(sc_request, 1);
  *current_request = (sc_request){.thread = sc_thread_self(), .type = READER};
  sc_cond_init(&current_request->condition);
  sc_queue_push(monitor->queue, current_request);

  while (sc_queue_front(monitor->queue) != current_request || monitor->active_writer)
    sc_cond_wait(&current_request->condition, &monitor->rw_mutex);

  sc_mem_free(sc_queue_pop(monitor->queue));
  ++monitor->active_readers;

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_release_read(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  --monitor->active_readers;
  if (monitor->active_readers == 0)
  {
    if (!sc_queue_empty(monitor->queue))
      sc_cond_signal(&((sc_request *)sc_queue_front(monitor->queue))->condition);
  }

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_acquire_write(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  sc_request * current_request = sc_mem_new(sc_request, 1);
  *current_request = (sc_request){.thread = sc_thread_self(), .type = WRITER};
  sc_cond_init(&current_request->condition);
  sc_queue_push(monitor->queue, current_request);

  while (sc_queue_front(monitor->queue) != current_request || monitor->active_writer || monitor->active_readers > 0)
    sc_cond_wait(&current_request->condition, &monitor->rw_mutex);

  sc_mem_free(sc_queue_pop(monitor->queue));
  monitor->active_writer = 1;

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_release_write(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->rw_mutex);

  monitor->active_writer = 0;

  if (!sc_queue_empty(monitor->queue))
    sc_cond_signal(&((sc_request *)sc_queue_front(monitor->queue))->condition);

  sc_mutex_unlock(&monitor->rw_mutex);
}

sc_int32 compare_monitors(void const * a, void const * b)
{
  sc_monitor * monitor_a = *(sc_monitor **)a;
  sc_monitor * monitor_b = *(sc_monitor **)b;
  return (sc_int32)(monitor_a->id - monitor_b->id);
}

void sc_monitor_acquire_read_n(sc_uint32 n, ...)
{
  va_list args;
  va_start(args, n);
  sc_monitor * monitors[n];
  sc_uint32 unique_count = 0;

  for (sc_uint32 i = 0; i < n; ++i)
  {
    sc_monitor * temp = va_arg(args, sc_monitor *);
    sc_uint32 j;
    for (j = 0; j < unique_count; ++j)
    {
      if (monitors[j]->id == temp->id)
        break;
    }
    if (j == unique_count)
      monitors[unique_count++] = temp;
  }

  n = unique_count;
  qsort(monitors, n, sizeof(sc_monitor *), compare_monitors);

  for (sc_uint32 i = 0; i < n; ++i)
    sc_monitor_acquire_read(monitors[i]);

  va_end(args);
}

void sc_monitor_release_read_n(sc_uint32 n, ...)
{
  va_list args;
  va_start(args, n);
  sc_monitor * monitors[n];
  sc_uint32 unique_count = 0;

  for (sc_uint32 i = 0; i < n; ++i)
  {
    sc_monitor * temp = va_arg(args, sc_monitor *);
    sc_uint32 j;
    for (j = 0; j < unique_count; ++j)
    {
      if (monitors[j]->id == temp->id)
        break;
    }
    if (j == unique_count)
      monitors[unique_count++] = temp;
  }

  n = unique_count;
  qsort(monitors, n, sizeof(sc_monitor *), compare_monitors);

  for (sc_int32 i = (sc_int32)n - 1; i >= 0; --i)
    sc_monitor_release_read(monitors[i]);

  va_end(args);
}

void sc_monitor_acquire_write_n(sc_uint32 n, ...)
{
  va_list args;
  va_start(args, n);
  sc_monitor * monitors[n];
  sc_uint32 unique_count = 0;

  for (sc_uint32 i = 0; i < n; ++i)
  {
    sc_monitor * temp = va_arg(args, sc_monitor *);
    sc_uint32 j;
    for (j = 0; j < unique_count; ++j)
    {
      if (monitors[j]->id == temp->id)
        break;
    }
    if (j == unique_count)
      monitors[unique_count++] = temp;
  }

  n = unique_count;
  qsort(monitors, n, sizeof(sc_monitor *), compare_monitors);

  for (sc_uint32 i = 0; i < n; ++i)
    sc_monitor_acquire_write(monitors[i]);

  va_end(args);
}

void sc_monitor_release_write_n(sc_uint32 n, ...)
{
  va_list args;
  va_start(args, n);
  sc_monitor * monitors[n];
  sc_uint32 unique_count = 0;

  for (sc_uint32 i = 0; i < n; ++i)
  {
    sc_monitor * temp = va_arg(args, sc_monitor *);
    sc_uint32 j;
    for (j = 0; j < unique_count; ++j)
    {
      if (monitors[j]->id == temp->id)
        break;
    }
    if (j == unique_count)
      monitors[unique_count++] = temp;
  }

  n = unique_count;
  qsort(monitors, n, sizeof(sc_monitor *), compare_monitors);

  for (sc_int32 i = (sc_int32)n - 1; i >= 0; --i)
    sc_monitor_release_write(monitors[i]);

  va_end(args);
}
