/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_mutex_h_
#define _sc_mutex_h_

#include "sc-core/sc_defines.h"

typedef struct _sc_mutex sc_mutex;

_SC_EXTERN void sc_mutex_init(sc_mutex * mutex);

_SC_EXTERN void sc_mutex_lock(sc_mutex * mutex);

_SC_EXTERN void sc_mutex_unlock(sc_mutex * mutex);

_SC_EXTERN void sc_mutex_destroy(sc_mutex * mutex);

#endif
