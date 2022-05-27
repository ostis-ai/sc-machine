/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_defines.hpp"

#include <chrono>

class ScTimer
{
public:
  _SC_EXTERN explicit ScTimer(double timeoutSec = -1.0);

  // Returns number of seconds elapsed from timer start
  _SC_EXTERN double Seconds() const;

  // Returns true, if time since start to Now is more than timeoutSec (that passed into constructor)
  _SC_EXTERN bool IsTimeOut() const;

private:
  using TimeValue = std::chrono::steady_clock::time_point;

  inline TimeValue Now() const
  {
    return std::chrono::steady_clock::now();
  }

private:
  TimeValue m_startTime;
  double m_timeOutSec;
};
