/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_mutex_private.h"

void sc_mutex_init(sc_mutex * mutex)
{
  g_mutex_init(&mutex->instance);
}

void sc_mutex_lock(sc_mutex * mutex)
{
  g_mutex_lock(&mutex->instance);
}

void sc_mutex_unlock(sc_mutex * mutex)
{
  g_mutex_unlock(&mutex->instance);
}

void sc_mutex_destroy(sc_mutex * mutex)
{
  g_mutex_clear(&mutex->instance);
}
