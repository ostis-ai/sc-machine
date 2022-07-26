/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_params.h"

#define DEFAULT_SAVE_PERIOD 32000
#define DEFAULT_UPDATE_PERIOD 16000
#define DEFAULT_MAX_THREADS 32
#define DEFAULT_MAX_LOADED_SEGMENTS 1000
#define DEFAULT_LOG_TYPE "Console"
#define DEFAULT_LOG_FILE ""
#define DEFAULT_LOG_LEVEL "Info"

void sc_memory_params_clear(sc_memory_params * params)
{
  params->version = (sc_char const *)null_ptr;
  params->clear = SC_FALSE;
  params->repo_path = (sc_char const *)null_ptr;
  params->ext_path = (sc_char const *)null_ptr;
  params->enabled_exts = (sc_char const **)null_ptr;
  params->save_period = DEFAULT_SAVE_PERIOD;      // seconds
  params->update_period = DEFAULT_UPDATE_PERIOD;  // seconds

  params->log_type = DEFAULT_LOG_TYPE;
  params->log_file = DEFAULT_LOG_FILE;
  params->log_level = DEFAULT_LOG_LEVEL;

  params->max_loaded_segments = DEFAULT_MAX_LOADED_SEGMENTS;
  params->max_threads = DEFAULT_MAX_THREADS;
}
