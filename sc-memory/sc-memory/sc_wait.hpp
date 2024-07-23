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

/*!
 * @class ScWait
 * @brief Implements common wait logic.
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

    /*!
     * @brief Waits for condition to be resolved or a timeout.
     * @param timeout_ms Timeout in milliseconds.
     * @param startDelegate Delegate function to call at the start of the wait.
     * @return True if resolved, false if timeout.
     */
    sc_bool Wait(sc_uint32 timeout_ms, DelegateFunc const & startDelegate);

  private:
    std::mutex m_mutex;               ///< Mutex for thread safety.
    std::condition_variable m_cond;   ///< Condition variable for waiting.
    sc_bool m_isResolved = SC_FALSE;  ///< Flag indicating if the condition is resolved.
  };

public:
  _SC_EXTERN ~ScWait() override;

  /*!
   * @brief Resolves the wait condition.
   */
  _SC_EXTERN void Resolve();

  /*!
   * @brief Sets a delegate function to be called at the start of the wait.
   * @param startDelegate Delegate function.
   * @return Pointer to the current ScWait object.
   */
  _SC_EXTERN ScWait * SetOnWaitStartDelegate(DelegateFunc const & startDelegate);

  /*!
   * @brief Waits for the condition to be resolved or a timeout.
   * @param timeout_ms Timeout in milliseconds.
   * @param onWaitSuccess Function to call on successful wait.
   * @param onWaitUnsuccess Function to call on unsuccessful wait.
   * @return True if resolved, false if timeout.
   */
  _SC_EXTERN sc_bool Wait(
      sc_uint32 timeout_ms = 5000,
      std::function<void(void)> const & onWaitSuccess = {},
      std::function<void(void)> const & onWaitUnsuccess = {});

protected:
  void Initialize(ScMemoryContext *, ScAddr const &) override;
  void Shutdown(ScMemoryContext *) override;

private:
  Impl m_impl;                       ///< Implementation of the wait logic.
  DelegateFunc m_waitStartDelegate;  ///< Delegate function for wait start.
};

/*!
 * @class ScWaitEvent
 * @brief Implements event wait logic. Should be alive while sc-memory context is alive.
 * @tparam TScEvent Type of the event.
 */
template <class TScEvent>
class _SC_EXTERN ScWaitEvent : public ScWait
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  /*!
   * @brief Constructor for ScWaitEvent.
   * @param ctx Sc-memory context.
   * @param addr An address of sc-element to wait event for.
   */

  _SC_EXTERN ScWaitEvent(ScMemoryContext const & ctx, ScAddr const & addr);

protected:
  /*!
   * @brief Event handler.
   * @param event Event object.
   * @return Result of the event handling.
   */
  virtual sc_bool OnEvent(TScEvent const &);

private:
  ScElementaryEventSubscription<TScEvent> m_event;
};

/*!
 * @class ScWaitCondition
 * @brief Implements waiting for a condition based on an event.
 * @tparam TScEvent Type of event.
 */
template <class TScEvent>
class _SC_EXTERN ScWaitCondition final : public ScWaitEvent<TScEvent>
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  using DelegateCheckFunc = std::function<sc_bool(TScEvent const &)>;

  /*!
   * @brief Constructor for ScWaitCondition.
   * @param ctx Sc-memory context.
   * @param addr An address of sc-element to wait event for.
   * @param func Delegate function to check the condition.
   */
  _SC_EXTERN ScWaitCondition(ScMemoryContext const & ctx, ScAddr const & addr, DelegateCheckFunc func);

private:
  /*!
   * @brief Event handler.
   * @param event Event object.
   */
  sc_bool OnEvent(TScEvent const & event) override;

private:
  DelegateCheckFunc m_checkFunc;
};

/*!
 * @class ScWaitActionFinished
 * @brief Implements waiting for an action to finish.
 */
class _SC_EXTERN ScWaitActionFinished final : public ScWaitEvent<ScEventAddInputArc>
{
public:
  /*!
   * @brief Constructor for ScWaitActionFinished.
   * @param ctx Sc-memory context.
   * @param actionAddr An address of the action.
   */
  _SC_EXTERN ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr);

private:
  /*!
   * @brief Event handler.
   * @param event Event object.
   * @return Result of the event handling.
   */
  sc_bool OnEvent(ScEventAddInputArc const & event) override;
};

#include "sc_wait.tpp"
