/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_storage_dump_manager.h"

#include "sc_storage.h"
#include "../sc_memory_private.h"

#include "sc-base/sc_allocator.h"

struct _sc_storage_dump_manager
{
  sc_bool dump_memory;
  sc_uint32 dump_memory_period;
  timer_t dump_memory_timer;
  sc_bool dump_memory_statistics;
  sc_uint32 dump_memory_statistics_period;
  timer_t dump_memory_statistics_timer;
};

timer_t _sc_storage_dump_manager_create_timer(
    void (*callback_function)(sc_int32, siginfo_t *, void *),
    sc_uint32 period)
{
  timer_t timer;
  struct sigevent sev;

  // Set up the signal handler
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = callback_function;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGALRM, &sa, NULL);

  // Set up the timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM;
  sev.sigev_value.sival_ptr = &timer;
  timer_create(CLOCK_REALTIME, &sev, &timer);

  // Set the timer to call the callback function in `period` seconds
  struct itimerspec its;
  its.it_value.tv_sec = period;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = period;
  its.it_interval.tv_nsec = 0;
  timer_settime(timer, 0, &its, NULL);

  return timer;
}

void _sc_storage_dump_manager_delete_timer(timer_t timer)
{
  timer_delete(timer);
}

void _sc_storage_dump_timer(sc_int32 signum, siginfo_t * siginfo, void * result)
{
  sc_memory_info("Dump sc-memory by period");
  sc_storage_save(null_ptr);
}

void _sc_storage_dump_statistics_timer(sc_int32 signum, siginfo_t * siginfo, void * result)
{
  sc_memory_info("Dump sc-memory statistics by period");
  sc_stat statistics;
  sc_storage_get_elements_stat(&statistics);

  sc_memory_info("Dump sc-memory statistics by period");
  sc_uint64 const allElements = statistics.node_count + statistics.link_count + statistics.arc_count;
  sc_message("Nodes: %llu (%f)", statistics.node_count, (sc_float)statistics.node_count / (sc_float)allElements * 100);
  sc_message("Links: %llu (%f)", statistics.link_count, (sc_float)statistics.link_count / (sc_float)allElements * 100);
  sc_message("Edges: %llu (%f)", statistics.arc_count, (sc_float)statistics.arc_count / (sc_float)allElements * 100);
  sc_message("Total: %llu", allElements);
}

void sc_storage_dump_manager_initialize(sc_storage_dump_manager ** manager, sc_memory_params const * params)
{
  *manager = sc_mem_new(sc_storage_dump_manager, 1);
  (*manager)->dump_memory = params->dump_memory;
  (*manager)->dump_memory_period = params->dump_memory_period;
  (*manager)->dump_memory_statistics = params->dump_memory_statistics;
  (*manager)->dump_memory_statistics_period = params->dump_memory_statistics_period;

  sc_memory_info("Initialize dump manager");
  sc_memory_info("Sc-memory dump manager configuration");
  sc_message("\tDump memory: %s", params->dump_memory ? "On" : "Off");
  sc_message("\tDump memory period: %d seconds", params->dump_memory_period);
  sc_message("\tDump memory statistics: %s", params->dump_memory_statistics ? "On" : "Off");
  sc_message("\tDump memory statistics period: %d seconds", params->dump_memory_statistics_period);

  if ((*manager)->dump_memory == SC_TRUE)
  {
    sc_memory_info("Set timer for sc-memory dumps");
    (*manager)->dump_memory_timer =
        _sc_storage_dump_manager_create_timer(_sc_storage_dump_timer, (*manager)->dump_memory_period);
  }

  if ((*manager)->dump_memory_statistics == SC_TRUE)
  {
    sc_memory_info("Set timer for sc-memory statistics dumps");
    (*manager)->dump_memory_statistics_timer = _sc_storage_dump_manager_create_timer(
        _sc_storage_dump_statistics_timer, (*manager)->dump_memory_statistics_period);
  }
}

void sc_storage_dump_manager_shutdown(sc_storage_dump_manager * manager)
{
  if (manager->dump_memory == SC_TRUE)
  {
    sc_memory_info("Unset timer for sc-memory dumps");
    _sc_storage_dump_manager_delete_timer(manager->dump_memory_timer);
  }

  if (manager->dump_memory_statistics_period == SC_TRUE)
  {
    sc_memory_info("Unset timer for sc-memory statistics dumps");
    _sc_storage_dump_manager_delete_timer(manager->dump_memory_statistics_timer);
  }
}
