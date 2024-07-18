/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <condition_variable>
#include <chrono>
#include <mutex>
#include <utility>

#include "sc_event_subscription.hpp"

/* Class implements common wait logic.
 */
class _SC_EXTERN ScWait : public ScObject
{
public:
  using DelegateFunc = std::function<void(void)>;

private:
  class Impl
  {
  public:
    Impl();
    virtual ~Impl();

    void Resolve();

    sc_bool Wait(sc_uint32 timeout_ms, DelegateFunc const & startDelegate);

  private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    sc_bool m_isResolved = SC_FALSE;
  };

public:
  _SC_EXTERN ~ScWait() override;

  _SC_EXTERN void Resolve();

  _SC_EXTERN ScWait * SetOnWaitStartDelegate(DelegateFunc const & startDelegate);

  _SC_EXTERN sc_bool Wait(
      sc_uint32 timeout_ms = 5000,
      std::function<void(void)> const & onWaitSuccess = {},
      std::function<void(void)> const & onWaitUnsuccess = {});

protected:
  sc_result Initialize(ScMemoryContext *, ScAddr const &) override;

  sc_result Shutdown(ScMemoryContext *) override;

private:
  Impl m_impl;
  DelegateFunc m_waitStartDelegate;
};

/* Class implements event wait logic.
 * Should be alive, while Memory context is alive.
 */
template <class TScEvent>
class _SC_EXTERN ScWaitEvent : public ScWait
{
public:
  _SC_EXTERN ScWaitEvent(ScMemoryContext const & ctx, ScAddr const & addr);

protected:
  virtual sc_result OnEvent(TScEvent const &);

private:
  ScElementaryEventSubscription<TScEvent> m_event;
};

template <class TScEvent>
class _SC_EXTERN ScWaitCondition final : public ScWaitEvent<TScEvent>
{
public:
  using DelegateCheckFunc = std::function<sc_result(TScEvent const &)>;

  _SC_EXTERN ScWaitCondition(ScMemoryContext const & ctx, ScAddr const & addr, DelegateCheckFunc func);

private:
  sc_result OnEvent(TScEvent const & event) override;

private:
  DelegateCheckFunc m_checkFunc;
};

/* Implements waiting for action finish
 */
class _SC_EXTERN ScWaitActionFinished final : public ScWaitEvent<ScEventAddInputEdge>
{
public:
  _SC_EXTERN ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr);

private:
  sc_result OnEvent(ScEventAddInputEdge const & event) override;
};

#include "sc_wait.tpp"
