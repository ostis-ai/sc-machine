/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <atomic>

namespace utils
{

// Simple lock without mutex
class ScLock
{
public:
  ScLock();

  void Lock();
  void Unlock();

private:
  std::atomic_bool m_locked;
};

} // namespace utils
