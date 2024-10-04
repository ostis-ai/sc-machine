/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_storage_dumper_
#define _sc_storage_dumper_

#include "sc-core/sc_memory_params.h"

typedef struct _sc_storage_dump_manager sc_storage_dump_manager;

void sc_storage_dump_manager_initialize(sc_storage_dump_manager ** manager, sc_memory_params const * params);

void sc_storage_dump_manager_shutdown(sc_storage_dump_manager * manager);

#endif  // _sc_storage_dumper_
