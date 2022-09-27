/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_lock.hpp"

#include <thread>

extern "C"
{
#include "sc-core/sc-store/sc-base/sc_atomic.h"
}

namespace utils
{
ScLock::ScLock()
  : m_locked(0)
{
  Unlock();
}

void ScLock::Lock()
{
  while (sc_atomic_int_compare_and_exchange((int *)&m_locked, 0, 1) == FALSE)
  {
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}

void ScLock::Unlock()
{
  sc_atomic_int_set(&m_locked, 0);
}

bool ScLock::IsLocked() const
{
  return sc_atomic_int_get(&m_locked) == 1;
}

}  // namespace utils
