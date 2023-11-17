/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_timer.hpp"
#include "sc-memory/sc_object.hpp"

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

  std::string GetName() override
  {
    return "ScWait";
  }

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
  sc_result Initialize() override
  {
    return SC_RESULT_OK;
  }

  sc_result Initialize(std::string const & initMemoryGeneratedStructure) override
  {
    return SC_RESULT_OK;
  }

  sc_result Shutdown() override
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
template <class ScEventClassT>
class ScWaitEvent : public ScWait
{
public:
  ScWaitEvent(ScMemoryContext const & ctx, const ScAddr & addr)
    : m_event(
          ctx,
          addr,
          [this](ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) -> sc_result {
            if (OnEvent(listenAddr, edgeAddr, otherAddr) == SC_RESULT_OK)
            {
              ScWait::Resolve();
              return SC_RESULT_OK;
            }
            return SC_RESULT_ERROR;
          })
  {
  }

protected:
  virtual sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    return SC_RESULT_OK;
  }

private:
  ScEventClassT m_event;
};

template <class ScEventClassT>
class ScWaitCondition final : public ScWaitEvent<ScEventClassT>
{
public:
  using DelegateCheckFunc = std::function<sc_result(ScAddr const &, ScAddr const &, ScAddr const &)>;

  ScWaitCondition(ScMemoryContext const & ctx, const ScAddr & addr, DelegateCheckFunc func)
    : ScWaitEvent<ScEventClassT>(ctx, addr)
    , m_checkFunc(std::move(func))
  {
  }

private:
  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override
  {
    return m_checkFunc(listenAddr, edgeAddr, otherAddr);
  }

private:
  DelegateCheckFunc m_checkFunc;
};

/* Implements waiting for action finish
 */
class ScWaitActionFinished final : public ScWaitEvent<ScEventAddInputEdge>
{
public:
  _SC_EXTERN ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr);

private:
  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};