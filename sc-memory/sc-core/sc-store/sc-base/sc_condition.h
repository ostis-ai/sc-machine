/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_condition_h_
#define _sc_condition_h_

#define GLIB

#ifdef GLIB
#  include <glib.h>
#endif

typedef GCond sc_condition;

#define sc_cond_init(condition) g_cond_init(condition)

#define sc_cond_wait(condition, mutex) g_cond_wait(condition, mutex)

#define sc_cond_signal(condition) g_cond_signal(condition)

#define sc_cond_broadcast(condition) g_cond_broadcast(condition)

#undef GLIB

#endif
