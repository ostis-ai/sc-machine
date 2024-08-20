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
  m_isResolved = SC_TRUE;
  m_cond.notify_one();
}

sc_bool ScWaiter::Impl::Wait(sc_uint32 timeout_ms, DelegateFunc const & startDelegate)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  sc_bool actionPerformed = SC_FALSE;
  while (!m_isResolved)
  {
    if (actionPerformed == SC_FALSE)
    {
      if (startDelegate)
        startDelegate();
      actionPerformed = SC_TRUE;
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

sc_bool ScWaiter::Wait(
    sc_uint32 timeout_ms,
    std::function<void(void)> const & onWaitSuccess,
    std::function<void(void)> const & onWaitUnsuccess)
{
  sc_bool const result = m_impl.Wait(timeout_ms, m_waitStartDelegate);

  if (result == SC_TRUE)
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
  : ScEventWaiter<ScEventAddIncomingArc<ScType::EdgeAccessConstPosPerm>>(ctx, actionAddr)
{
}

sc_bool ScWaiterActionFinished::OnEvent(ScEventAddIncomingArc<ScType::EdgeAccessConstPosPerm> const & event)
{
  return event.GetArcSourceElement() == ScKeynodes::action_finished;
}
