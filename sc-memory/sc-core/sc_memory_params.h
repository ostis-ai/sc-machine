/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_params_h_
#define _sc_memory_params_h_

#include "sc-store/sc_types.h"

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
  sc_uint8 max_threads;
} sc_memory_params;

_SC_EXTERN void sc_memory_params_clear(sc_memory_params * params);

#endif
