/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_thread_h_
#define _sc_thread_h_

#include <glib.h>

typedef GThread sc_thread;
typedef gpointer sc_thread_data;
typedef gpointer (*sc_thread_func)(sc_thread_data);
typedef const gchar sc_thread_name;

#define sc_thread_self g_thread_self

sc_thread * sc_thread_new(sc_thread_name * name, sc_thread_func func, sc_thread_data data);
void sc_thread_join(sc_thread * thread);
void sc_thread_unref(sc_thread * thread);

#endif
