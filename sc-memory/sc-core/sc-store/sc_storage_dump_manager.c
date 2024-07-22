/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_storage_dump_manager.h"

#include <unistd.h>

#include "sc_storage.h"
#include "../sc_memory_private.h"

#include "sc-base/sc_allocator.h"

typedef void (*sc_timed_callback)();
typedef pthread_t sc_timer;

typedef struct
{
  sc_bool dump;
  sc_uint32 dump_period;
  sc_timed_callback timed_dump_callback;
  sc_timer dump_timer;
} sc_dump_info;

struct _sc_storage_dump_manager
{
  sc_dump_info dump_memory_info;
  sc_dump_info dump_memory_statistics_info;
};

void * _sc_timer_check_periodic(void * arg)
{
  sc_dump_info * dump_info = arg;

  while (dump_info->dump)
  {
    for (sc_uint32 i = 0; i < dump_info->dump_period && dump_info->dump; ++i)
      sleep(1);

    if (!dump_info->dump)
      break;

    dump_info->timed_dump_callback();
  }

  pthread_exit(null_ptr);
}

sc_timer _sc_storage_dump_manager_create_timer(sc_dump_info * dump_info)
{
  sc_timer timer;
  pthread_create(&timer, null_ptr, _sc_timer_check_periodic, dump_info);
  return timer;
}

void _sc_storage_dump_manager_delete_timer(sc_timer timer)
{
  pthread_join(timer, null_ptr);
}

void _sc_storage_dump_timer()
{
  sc_memory_info("Dump sc-memory by period");
  sc_storage_save(null_ptr);
}

void _sc_storage_dump_statistics_timer()
{
  sc_memory_info("Dump sc-memory statistics by period");
  sc_stat statistics;
  sc_storage_get_elements_stat(&statistics);

  sc_uint64 const allElements = statistics.node_count + statistics.link_count + statistics.arc_count;
  sc_message(
      "Nodes: %" PRIu64 "(%f)", statistics.node_count, (sc_float)statistics.node_count / (sc_float)allElements * 100);
  sc_message(
      "Links: %" PRIu64 "(%f)", statistics.link_count, (sc_float)statistics.link_count / (sc_float)allElements * 100);
  sc_message(
      "Edges: %" PRIu64 "(%f)", statistics.arc_count, (sc_float)statistics.arc_count / (sc_float)allElements * 100);
  sc_message("Total: %" PRIu64, allElements);
}

void sc_storage_dump_manager_initialize(sc_storage_dump_manager ** manager, sc_memory_params const * params)
{
  *manager = sc_mem_new(sc_storage_dump_manager, 1);
  (*manager)->dump_memory_info = (sc_dump_info){
      .dump = params->dump_memory,
      .dump_period = params->dump_memory_period,
      .timed_dump_callback = _sc_storage_dump_timer};
  (*manager)->dump_memory_statistics_info = (sc_dump_info){
      .dump = params->dump_memory_statistics,
      .dump_period = params->dump_memory_statistics_period,
      .timed_dump_callback = _sc_storage_dump_statistics_timer};

  sc_memory_info("Initialize dump manager");
  sc_memory_info("Sc-memory dump manager configuration");
  sc_message("\tDump memory: %s", (*manager)->dump_memory_info.dump ? "On" : "Off");
  sc_message("\tDump memory period: %d seconds", (*manager)->dump_memory_info.dump_period);
  sc_message("\tDump memory statistics: %s", params->dump_memory_statistics ? "On" : "Off");
  sc_message("\tDump memory statistics period: %d seconds", params->dump_memory_statistics_period);

  if ((*manager)->dump_memory_info.dump == SC_TRUE)
  {
    sc_memory_info("Set timer for sc-memory dumps");
    (*manager)->dump_memory_info.dump_timer = _sc_storage_dump_manager_create_timer(&(*manager)->dump_memory_info);
  }

  if ((*manager)->dump_memory_statistics_info.dump == SC_TRUE)
  {
    sc_memory_info("Set timer for sc-memory statistics dumps");
    (*manager)->dump_memory_statistics_info.dump_timer =
        _sc_storage_dump_manager_create_timer(&(*manager)->dump_memory_statistics_info);
  }
}

void sc_storage_dump_manager_shutdown(sc_storage_dump_manager * manager)
{
  if (manager->dump_memory_info.dump == SC_TRUE)
  {
    manager->dump_memory_info.dump = SC_FALSE;
    sc_memory_info("Unset timer for sc-memory dumps");
    _sc_storage_dump_manager_delete_timer(manager->dump_memory_info.dump_timer);
  }

  if (manager->dump_memory_statistics_info.dump == SC_TRUE)
  {
    manager->dump_memory_statistics_info.dump = SC_FALSE;
    sc_memory_info("Unset timer for sc-memory statistics dumps");
    _sc_storage_dump_manager_delete_timer(manager->dump_memory_statistics_info.dump_timer);
  }
  sc_mem_free(manager);
}
