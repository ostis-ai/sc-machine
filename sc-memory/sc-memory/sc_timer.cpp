/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_timer.hpp"

ScTimer::ScTimer(double timeoutSec /*= -1.0*/)
  : m_startTime(Now())
  , m_timeOutSec(timeoutSec)
{
}

double ScTimer::Seconds() const
{
  std::chrono::duration<double> const diff = Now() - m_startTime;
  return diff.count();
}

bool ScTimer::IsTimeOut() const
{
  return ((m_timeOutSec > 0) && (Seconds() > m_timeOutSec));
}
