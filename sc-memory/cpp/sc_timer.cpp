/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_timer.hpp"

ScTimer::ScTimer(double timeoutSec /*= -1.0*/)
    : mStartTime(Now())
    , mTimeOutSec(timeoutSec)
{
}

double ScTimer::Seconds() const
{
    std::chrono::duration<double> const diff = Now() - mStartTime;
    return diff.count();
}

bool ScTimer::IsTimeOut() const
{
    return ((mTimeOutSec > 0) && (Seconds() > mTimeOutSec));
}

