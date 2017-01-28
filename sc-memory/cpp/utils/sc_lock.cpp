/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_lock.hpp"

#include <thread>

namespace utils
{

ScLock::ScLock()
{
  Unlock();
}

void ScLock::Lock()
{
  bool expected = false;
  while (true)
  {
    if (m_locked.compare_exchange_strong(expected, true))
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}

void ScLock::Unlock()
{
  m_locked.store(false);
}

} // namespace utils
