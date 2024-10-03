/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <functional>

#include "sc_event.hpp"

#include "utils/sc_lock.hpp"

/*!
 * Base class for sc-events subscriptions.
 */
class _SC_EXTERN ScEventSubscription : public ScObject
{
public:
  _SC_EXTERN ~ScEventSubscription() noexcept override;

  _SC_EXTERN virtual void RemoveDelegate() noexcept = 0;
};

SHARED_PTR_TYPE(ScEventSubscription);

template <class TScEvent = ScElementaryEvent>
class _SC_EXTERN ScElementaryEventSubscription final : public ScEventSubscription
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  friend class ScMemoryContext;
  friend class ScAgentContext;
  template <class TScEventType>
  friend class ScEventWaiter;
  template <class TScAgent>
  friend class ScAgentManager;
  friend class ScMemoryJsonEventsHandler;

  SC_DISALLOW_COPY_AND_MOVE(ScElementaryEventSubscription);

public:
  using DelegateFunc = std::function<void(TScEvent const & event)>;

  _SC_EXTERN ~ScElementaryEventSubscription() noexcept override;

  /* Set specified function as a delegate that will be called on event emit */
  _SC_EXTERN void SetDelegate(DelegateFunc && func) noexcept;

  _SC_EXTERN void RemoveDelegate() noexcept override;

protected:
  explicit _SC_EXTERN ScElementaryEventSubscription(
      ScMemoryContext const & context,
      ScAddr const & subscriptionElementAddr,
      DelegateFunc const & func = DelegateFunc()) noexcept;

  explicit _SC_EXTERN ScElementaryEventSubscription(
      ScMemoryContext const & context,
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      DelegateFunc const & func = DelegateFunc()) noexcept;

  _SC_EXTERN static sc_result Handle(
      sc_event_subscription const * event_subscription,
      sc_addr userAddr,
      sc_addr connectorAddr,
      sc_type connectorType,
      sc_addr otherAddr) noexcept;

  _SC_EXTERN static sc_result HandleDelete(sc_event_subscription const * event) noexcept;

private:
  sc_event_subscription * m_event_subscription;

  DelegateFunc m_delegate;
  utils::ScLock m_lock;
};

#include "_template/sc_event_subscription.tpp"
