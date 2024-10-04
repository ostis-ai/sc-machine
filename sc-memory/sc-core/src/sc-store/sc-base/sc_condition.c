/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_condition_private.h"

#include "sc_mutex_private.h"

void sc_cond_init(sc_condition * condition)
{
  g_cond_init(&condition->instance);
}

void sc_cond_wait(sc_condition * condition, sc_mutex * mutex)
{
  g_cond_wait(&condition->instance, &mutex->instance);
}

void sc_cond_signal(sc_condition * condition)
{
  g_cond_signal(&condition->instance);
}

void sc_cond_broadcast(sc_condition * condition)
{
  g_cond_broadcast(&condition->instance);
}

void sc_cond_destroy(sc_condition * condition)
{
  g_cond_clear(&condition->instance);
}
