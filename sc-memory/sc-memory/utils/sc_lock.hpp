/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../sc_defines.hpp"

namespace utils
{
// Simple lock without mutex
class ScLock
{
public:
  _SC_EXTERN ScLock();

  _SC_EXTERN void Lock();
  _SC_EXTERN void Unlock();

  /* Note that lock status can be changed during
   * function run (by other thread). So there are no gurantee
   * that lock would have the same status after this function returns value.
   * It's useful just for some types of check.
   */
  _SC_EXTERN bool IsLocked() const;

private:
  volatile int m_locked;
};

struct ScLockScope
{
  explicit ScLockScope(ScLock & lock)
    : m_lock(lock)
  {
    m_lock.Lock();
  }
  ~ScLockScope()
  {
    m_lock.Unlock();
  }

private:
  ScLock & m_lock;
};

}  // namespace utils
