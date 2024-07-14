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

sc_bool ScWait::Impl::Wait(sc_uint32 timeout_ms)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  while (!m_isResolved)
  {
    if (m_cond.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout)
      return SC_FALSE;
  }

  return SC_TRUE;
}

ScWait::~ScWait() = default;

void ScWait::Resolve()
{
  m_impl.Resolve();
}

ScWait & ScWait::SetOnWaitStartDelegate(DelegateFunc const & startDelegate)
{
  m_waitStartDelegate = startDelegate;
  return *this;
}

sc_bool ScWait::Wait(
    sc_uint32 timeout_ms,
    std::function<void(void)> const & onWaitSuccess,
    std::function<void(void)> const & onWaitUnsuccess)
{
  if (m_waitStartDelegate)
    m_waitStartDelegate();

  sc_bool const result = m_impl.Wait(timeout_ms);

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

sc_result ScWait::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

sc_result ScWait::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

ScWaitActionFinished::ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr)
  : ScWaitEvent<ScEventAddInputEdge>(ctx, actionAddr)
{
}

sc_result ScWaitActionFinished::OnEvent(ScEventAddInputEdge const & event)
{
  return event.GetOtherElement() == ScKeynodes::action_finished ? SC_RESULT_OK : SC_RESULT_NO;
}
