/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_params.h"

void sc_memory_params_clear(sc_memory_params * params)
{
  params->version = (sc_char const *)null_ptr;
  params->clear = SC_FALSE;
  params->repo_path = (sc_char const *)null_ptr;
  params->ext_path = (sc_char const *)null_ptr;
  params->enabled_exts = (sc_char const **)null_ptr;
  params->save_period = 3600;    // seconds
  params->update_period = 1200;  // seconds

  params->debug_type = (sc_char const *)null_ptr;
  params->debug_mode = (sc_char const *)null_ptr;
  params->debug_file = (sc_char const *)null_ptr;

  params->max_loaded_segments = 1000;
  params->max_threads = 32;
}
