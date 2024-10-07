/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/utils/sc_lock.hpp"

#include <thread>

namespace utils
{
ScLock::ScLock()
  : m_locked(0)
{
  Unlock();
}

void ScLock::Lock()
{
  while (m_locked.exchange(1, std::memory_order_acquire) == 1)
  {
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}

void ScLock::Unlock()
{
  m_locked.store(0, std::memory_order_release);
}

bool ScLock::IsLocked() const
{
  return m_locked.load(std::memory_order_acquire) == 1;
}

}  // namespace utils
