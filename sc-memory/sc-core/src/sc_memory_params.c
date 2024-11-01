/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-core/sc_memory_params.h"

void sc_memory_params_clear(sc_memory_params * params)
{
  params->version = (sc_version){
      SC_MACHINE_VERSION_MAJOR, SC_MACHINE_VERSION_MINOR, SC_MACHINE_VERSION_PATCH, SC_MACHINE_VERSION_SUFFIX};

  params->clear = SC_FALSE;
  params->binaries = (sc_char const *)null_ptr;
  params->ext_path = (sc_char const *)null_ptr;
  params->enabled_exts = (sc_char const **)null_ptr;

  params->max_loaded_segments = DEFAULT_MAX_LOADED_SEGMENTS;
  params->limit_max_threads_by_max_physical_cores = DEFAULT_LIMIT_MAX_THREADS_BY_MAX_PHYSICAL_CORES;
  params->max_events_and_agents_threads = DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS;

  params->dump_memory = SC_TRUE;
  params->save_period = params->dump_memory_period = DEFAULT_DUMP_MEMORY_PERIOD;  // seconds
  params->dump_memory_statistics = SC_TRUE;
  params->update_period = params->dump_memory_statistics_period = DEFAULT_DUMP_MEMORY_STATISTICS_PERIOD;  // seconds

  params->log_type = DEFAULT_LOG_TYPE;
  params->log_file = DEFAULT_LOG_FILE;
  params->log_level = DEFAULT_LOG_LEVEL;

  params->init_memory_generated_structure = (sc_char const *)null_ptr;
  params->init_memory_generated_upload = SC_FALSE;
  params->user_mode = SC_FALSE;

  params->max_strings_channels = DEFAULT_MAX_STRINGS_CHANNELS;
  params->max_strings_channel_size = DEFAULT_MAX_STRINGS_CHANNEL_SIZE;
  params->max_searchable_string_size = DEFAULT_MAX_SEARCHABLE_STRING_SIZE;
  params->term_separators = DEFAULT_TERM_SEPARATORS;
  params->search_by_substring = DEFAULT_SEARCH_BY_SUBSTRING;
}
