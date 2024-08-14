/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.hpp"

#include "sc_memory.hpp"

template <class TScEvent>
ScElementaryEventSubscription<TScEvent>::ScElementaryEventSubscription(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    DelegateFunc const & func)
{
  m_delegate = func;
  m_event_subscription = sc_event_subscription_with_user_new(
      *ctx,
      *subscriptionAddr,
      *TScEvent::eventClassAddr,
      *TScEvent::elementType,
      (sc_pointer)this,
      &ScElementaryEventSubscription::Handler,
      &ScElementaryEventSubscription::HandlerDelete);
}

template <class TScEvent>
ScElementaryEventSubscription<TScEvent>::ScElementaryEventSubscription(
    ScMemoryContext const & ctx,
    ScAddr const & eventClassAddr,
    ScAddr const & subscriptionAddr,
    DelegateFunc const & func)
{
  m_delegate = func;
  m_event_subscription = sc_event_subscription_with_user_new(
      *ctx,
      *subscriptionAddr,
      *eventClassAddr,
      *ScType::Unknown,
      (sc_pointer)this,
      &ScElementaryEventSubscription::Handler,
      &ScElementaryEventSubscription::HandlerDelete);
}

template <class TScEvent>
ScElementaryEventSubscription<TScEvent>::~ScElementaryEventSubscription()
{
  if (m_event_subscription)
    sc_event_subscription_destroy(m_event_subscription);
}

template <class TScEvent>
void ScElementaryEventSubscription<TScEvent>::SetDelegate(DelegateFunc && func)
{
  m_delegate = func;
}

template <class TScEvent>
void ScElementaryEventSubscription<TScEvent>::RemoveDelegate()
{
  m_delegate = DelegateFunc();
}

template <class TScEvent>
sc_result ScElementaryEventSubscription<TScEvent>::Handler(
    sc_event_subscription const * event_subscription,
    sc_addr userAddr,
    sc_addr connectorAddr,
    sc_type connectorType,
    sc_addr otherAddr)
{
  sc_result result = SC_RESULT_ERROR;

  auto * eventSubscription = (ScElementaryEventSubscription *)sc_event_subscription_get_data(event_subscription);

  DelegateFunc delegateFunc = eventSubscription->m_delegate;
  if (delegateFunc == nullptr)
    return result;

  try
  {
    if constexpr (std::is_same<TScEvent, ScElementaryEvent>::value)
      delegateFunc(TScEvent(
          userAddr,
          sc_event_subscription_get_event_type(event_subscription),
          sc_event_subscription_get_element(event_subscription),
          connectorAddr,
          connectorType,
          otherAddr));
    else
      delegateFunc(TScEvent(
          userAddr, sc_event_subscription_get_element(event_subscription), connectorAddr, connectorType, otherAddr));
  }
  catch (utils::ScException & e)
  {
    SC_LOG_ERROR("Uncaught exception: " << e.Message());
  }

  return SC_RESULT_OK;
}

template <class TScEvent>
sc_result ScElementaryEventSubscription<TScEvent>::HandlerDelete(sc_event_subscription const * event_subscription)
{
  auto * eventSubscription = (ScElementaryEventSubscription *)sc_event_subscription_get_data(event_subscription);

  utils::ScLockScope lock(eventSubscription->m_lock);
  if (eventSubscription->m_event_subscription)
  {
    eventSubscription->m_delegate = nullptr;
    eventSubscription->m_event_subscription = nullptr;
  }

  return SC_RESULT_OK;
}
