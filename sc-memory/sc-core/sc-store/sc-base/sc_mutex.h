/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_mutex_h_
#define _sc_mutex_h_

#define GLIB

#ifdef GLIB
#  include <glib.h>
#endif

typedef GMutex sc_mutex;

#define sc_mutex_init(mutex) g_mutex_init(mutex)

#define sc_mutex_lock(mutex) // g_mutex_lock(mutex)

#define sc_mutex_unlock(mutex) // g_mutex_unlock(mutex)

#define sc_mutex_destroy(mutex) g_mutex_clear(mutex)

#undef GLIB

#endif
