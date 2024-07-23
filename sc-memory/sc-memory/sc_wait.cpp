/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_wait.hpp"

ScWait::Impl::Impl() = default;
ScWait::Impl::~Impl() = default;

void ScWait::Impl::Resolve()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_isResolved = SC_TRUE;
  m_cond.notify_one();
}

sc_bool ScWait::Impl::Wait(sc_uint32 timeout_ms, DelegateFunc const & startDelegate)
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

ScWait::~ScWait() = default;

void ScWait::Resolve()
{
  m_impl.Resolve();
}

ScWait * ScWait::SetOnWaitStartDelegate(DelegateFunc const & startDelegate)
{
  m_waitStartDelegate = startDelegate;
  return this;
}

sc_bool ScWait::Wait(
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

void ScWait::Initialize(ScMemoryContext *, ScAddr const &) {}

void ScWait::Shutdown(ScMemoryContext *) {}

ScWaitActionFinished::ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr)
  : ScWaitEvent<ScEventAddInputArc>(ctx, actionAddr)
{
}

sc_bool ScWaitActionFinished::OnEvent(ScEventAddInputArc const & event)
{
  return event.GetArcSourceElement() == ScKeynodes::action_finished;
}
