/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_thread.h"

#include "sc-core/sc_types.h"

sc_thread * sc_thread_new(sc_thread_name * name, sc_thread_func func, sc_thread_data data)
{
  return g_thread_new(name, func, data);
}

void sc_thread_join(sc_thread * thread)
{
  if (thread != null_ptr)
  {
    g_thread_join(thread);
  }
}

void sc_thread_unref(sc_thread * thread)
{
  if (thread != null_ptr)
  {
    g_thread_unref(thread);
  }
}

void sc_thread_sleep(sc_uint32 milliseconds)
{
  g_usleep((gulong)milliseconds * 1000);
}
