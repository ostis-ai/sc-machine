/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_timer.hpp"
#include "sc-memory/sc_object.hpp"

#include "sc-memory/sc_keynodes.hpp"

#include <condition_variable>
#include <chrono>
#include <mutex>
#include <utility>

/* Class implements common wait logic.
 */
class ScWait : public ScObject
{
  class Impl
  {
  public:
    Impl() = default;
    virtual ~Impl() = default;

    void Resolve()
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_isResolved = SC_TRUE;
      m_cond.notify_one();
    }

    sc_bool Wait(sc_uint32 timeout_ms)
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      while (!m_isResolved)
      {
        if (m_cond.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout)
          return SC_FALSE;
      }

      return SC_TRUE;
    }

  private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    sc_bool m_isResolved = SC_FALSE;
  };

public:
  using DelegateFunc = std::function<void(void)>;

  ~ScWait() override = default;

  void Resolve()
  {
    m_impl.Resolve();
  }

  ScWait & SetOnWaitStartDelegate(DelegateFunc const & startDelegate)
  {
    m_waitStartDelegate = startDelegate;
    return *this;
  }

  sc_bool Wait(
      sc_uint32 timeout_ms = 5000,
      std::function<void(void)> const & onWaitSuccess = {},
      std::function<void(void)> const & onWaitUnsuccess = {})
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

protected:
  sc_result Initialize(ScMemoryContext *, ScAddr const &) override
  {
    return SC_RESULT_OK;
  }

  sc_result Shutdown(ScMemoryContext *) override
  {
    return SC_RESULT_OK;
  }

private:
  Impl m_impl;
  DelegateFunc m_waitStartDelegate;
};

/* Class implements event wait logic.
 * Should be alive, while Memory context is alive.
 */
template <class TScEvent>
class ScWaitEvent : public ScWait
{
public:
  ScWaitEvent(ScMemoryContext const & ctx, ScAddr const & addr)
    : m_event(
          ctx,
          addr,
          [this](TScEvent const & event) -> sc_result
          {
            if (OnEvent(event) == SC_RESULT_OK)
            {
              ScWait::Resolve();
              return SC_RESULT_OK;
            }
            return SC_RESULT_ERROR;
          })
  {
  }

protected:
  virtual sc_result OnEvent(TScEvent const &)
  {
    return SC_RESULT_OK;
  }

private:
  ScElementaryEventSubscription<TScEvent> m_event;
};

template <class TScEvent>
class ScWaitCondition final : public ScWaitEvent<TScEvent>
{
public:
  using DelegateCheckFunc = std::function<sc_result(TScEvent const &)>;

  ScWaitCondition(ScMemoryContext const & ctx, ScAddr const & addr, DelegateCheckFunc func)
    : ScWaitEvent<TScEvent>(ctx, addr)
    , m_checkFunc(std::move(func))
  {
  }

private:
  sc_result OnEvent(TScEvent const & event) override
  {
    return m_checkFunc(event);
  }

private:
  DelegateCheckFunc m_checkFunc;
};

/* Implements waiting for action finish
 */
class ScWaitActionFinished final : public ScWaitEvent<ScEventAddInputEdge>
{
public:
  _SC_EXTERN ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr)
    : ScWaitEvent<ScEventAddInputEdge>(ctx, actionAddr)
  {
  }

private:
  sc_result OnEvent(ScEventAddInputEdge const & event) override
  {
    return event.GetOtherElement() == ScKeynodes::action_finished ? SC_RESULT_OK : SC_RESULT_NO;
  }
};
