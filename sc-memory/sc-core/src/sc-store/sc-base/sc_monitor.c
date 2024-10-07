/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-core/sc-base/sc_monitor.h"
#include "sc-core/sc-base/sc_allocator.h"

#include "sc-store/sc-base/sc_monitor_private.h"
#include "sc-store/sc-base/sc_condition_private.h"
#include "sc-store/sc-base/sc_thread.h"

#define SC_MONITOR_FREE_PERIOD_CHECK 10

struct _sc_request
{
  sc_thread * thread;      // Thread instance of writer or reader
  sc_condition condition;  // Condition variable of writer or reader
};

void sc_monitor_init(sc_monitor * monitor)
{
  sc_mutex_init(&monitor->rw_mutex);
  monitor->id = 1;
  monitor->active_readers = 0;
  monitor->active_writer = 0;
  sc_queue_init(&monitor->queue);
  sc_mutex_init(&monitor->ref_count_mutex);
  monitor->ref_count = 0;
}

void sc_monitor_destroy(sc_monitor * monitor)
{
  if (monitor == null_ptr || monitor->id == 0)
    return;

  sc_mutex_lock(&monitor->ref_count_mutex);
  while (monitor->ref_count > 0)
  {
    sc_mutex_unlock(&monitor->ref_count_mutex);
    g_usleep(SC_MONITOR_FREE_PERIOD_CHECK);
    sc_mutex_lock(&monitor->ref_count_mutex);
  }
  sc_mutex_unlock(&monitor->ref_count_mutex);

  sc_mutex_destroy(&monitor->rw_mutex);
  monitor->active_readers = 0;
  monitor->active_writer = 0;
  monitor->id = 0;
  sc_queue_destroy(&monitor->queue);
  sc_mutex_destroy(&monitor->ref_count_mutex);
  monitor->ref_count = 0;
}

void sc_monitor_acquire(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->ref_count_mutex);
  monitor->ref_count++;
  sc_mutex_unlock(&monitor->ref_count_mutex);
}

void sc_monitor_release(sc_monitor * monitor)
{
  sc_mutex_lock(&monitor->ref_count_mutex);
  if (monitor->ref_count > 0)
    monitor->ref_count--;
  sc_mutex_unlock(&monitor->ref_count_mutex);
}

void sc_monitor_acquire_read(sc_monitor * monitor)
{
  if (monitor == null_ptr || monitor->id == 0)
    return;

  sc_monitor_acquire(monitor);

  sc_mutex_lock(&monitor->rw_mutex);

  sc_request current_request = (sc_request){.thread = sc_thread_self()};
  sc_cond_init(&current_request.condition);
  sc_queue_push(&monitor->queue, &current_request);

  while (sc_queue_front(&monitor->queue) != &current_request || monitor->active_writer)
    sc_cond_wait(&current_request.condition, &monitor->rw_mutex);

  sc_request * popped_request = sc_queue_pop(&monitor->queue);
  sc_cond_destroy(&popped_request->condition);
  ++monitor->active_readers;

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_release_read(sc_monitor * monitor)
{
  if (monitor == null_ptr || monitor->id == 0)
    return;

  sc_mutex_lock(&monitor->rw_mutex);

  --monitor->active_readers;
  if (monitor->active_readers == 0)
  {
    if (!sc_queue_empty(&monitor->queue))
      sc_cond_signal(&((sc_request *)sc_queue_front(&monitor->queue))->condition);
  }

  sc_mutex_unlock(&monitor->rw_mutex);

  sc_monitor_release(monitor);
}

void sc_monitor_acquire_write(sc_monitor * monitor)
{
  if (monitor == null_ptr || monitor->id == 0)
    return;

  sc_monitor_acquire(monitor);

  sc_mutex_lock(&monitor->rw_mutex);

  sc_request current_request = (sc_request){.thread = sc_thread_self()};
  sc_cond_init(&current_request.condition);
  sc_queue_push(&monitor->queue, &current_request);

  while (sc_queue_front(&monitor->queue) != &current_request || monitor->active_writer || monitor->active_readers > 0)
    sc_cond_wait(&current_request.condition, &monitor->rw_mutex);

  sc_request * popped_request = sc_queue_pop(&monitor->queue);
  sc_cond_destroy(&popped_request->condition);
  monitor->active_writer = 1;

  sc_mutex_unlock(&monitor->rw_mutex);
}

void sc_monitor_release_write(sc_monitor * monitor)
{
  if (monitor == null_ptr || monitor->id == 0)
    return;

  sc_mutex_lock(&monitor->rw_mutex);

  monitor->active_writer = 0;

  if (!sc_queue_empty(&monitor->queue))
    sc_cond_signal(&((sc_request *)sc_queue_front(&monitor->queue))->condition);

  sc_mutex_unlock(&monitor->rw_mutex);

  sc_monitor_release(monitor);
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
    if (temp == null_ptr)
      continue;

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
    if (temp == null_ptr)
      continue;

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
    if (temp == null_ptr)
      continue;

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
    if (temp == null_ptr)
      continue;

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
