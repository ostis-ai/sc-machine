/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_params_h_
#define _sc_memory_params_h_

#include "sc-store/sc_types.h"

#define DEFAULT_SAVE_PERIOD 32000
#define DEFAULT_UPDATE_PERIOD 16000
#define DEFAULT_MAX_THREADS 32
#define DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS 32
#define DEFAULT_MIN_EVENTS_AND_AGENTS_THREADS 1
#define DEFAULT_MAX_LOADED_SEGMENTS 1000
#define DEFAULT_MAX_SEARCHABLE_STRING_SIZE 1000
#define DEFAULT_LOG_TYPE "Console"
#define DEFAULT_LOG_FILE ""
#define DEFAULT_LOG_LEVEL "Info"

typedef struct _sc_memory_params
{
  const sc_char * version;
  sc_bool clear;
  const sc_char * repo_path;
  const sc_char * ext_path;
  const sc_char ** enabled_exts;
  sc_uint32 save_period;
  sc_uint32 update_period;

  const sc_char * log_type;
  const sc_char * log_file;
  const sc_char * log_level;

  sc_uint32 max_loaded_segments;
  sc_uint32 max_searchable_string_size;
  sc_uint8 max_threads;
  sc_uint32 max_events_and_agents_threads;

  const sc_char * init_memory_generated_structure;
  sc_bool init_memory_generated_upload;
} sc_memory_params;

_SC_EXTERN void sc_memory_params_clear(sc_memory_params * params);

#endif
