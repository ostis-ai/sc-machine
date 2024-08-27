/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_wait.hpp"

ScWaiter::Impl::Impl() = default;
ScWaiter::Impl::~Impl() = default;

void ScWaiter::Impl::Resolve()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_isResolved = true;
  m_cond.notify_one();
}

bool ScWaiter::Impl::Wait(sc_uint32 timeout_ms, DelegateFunc const & startDelegate)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  bool actionBeforeWaitPerformed = false;
  while (!m_isResolved)
  {
    if (actionBeforeWaitPerformed == false)
    {
      if (startDelegate)
        startDelegate();
      actionBeforeWaitPerformed = true;
    }

    if (m_cond.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout)
      return false;
  }

  return true;
}

ScWaiter::~ScWaiter() = default;

void ScWaiter::Resolve()
{
  m_impl.Resolve();
}

ScWaiter * ScWaiter::SetOnWaitStartDelegate(DelegateFunc const & startDelegate)
{
  m_waitStartDelegate = startDelegate;
  return this;
}

bool ScWaiter::Wait(
    sc_uint32 timeout_ms,
    std::function<void(void)> const & onWaitSuccess,
    std::function<void(void)> const & onWaitUnsuccess)
{
  bool const result = m_impl.Wait(timeout_ms, m_waitStartDelegate);
  if (result == true)
  {
    if (onWaitSuccess)
      onWaitSuccess();
  }
  else
  {
    if (onWaitUnsuccess)
      onWaitUnsuccess();
  }

  return result;
}

ScWaiterActionFinished::ScWaiterActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr)
  : ScEventWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(ctx, actionAddr)
{
}

bool ScWaiterActionFinished::OnEvent(ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event)
{
  return event.GetArcSourceElement() == ScKeynodes::action_finished;
}
