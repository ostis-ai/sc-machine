/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_condition_h_
#define _sc_condition_h_

#include "sc-core/sc_defines.h"

typedef struct _sc_condition sc_condition;
typedef struct _sc_mutex sc_mutex;

_SC_EXTERN void sc_cond_init(sc_condition * condition);

_SC_EXTERN void sc_cond_wait(sc_condition * condition, sc_mutex * mutex);

_SC_EXTERN void sc_cond_signal(sc_condition * condition);

_SC_EXTERN void sc_cond_broadcast(sc_condition * condition);

_SC_EXTERN void sc_cond_destroy(sc_condition * condition);

#endif
