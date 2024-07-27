/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.hpp"

#include "sc_memory.hpp"

template <class TScEvent>
TScElementaryEventSubscription<TScEvent>::TScElementaryEventSubscription(
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
      &TScElementaryEventSubscription::Handler,
      &TScElementaryEventSubscription::HandlerDelete);
}

template <class TScEvent>
TScElementaryEventSubscription<TScEvent>::~TScElementaryEventSubscription()
{
  if (m_event_subscription)
    sc_event_subscription_destroy(m_event_subscription);
}

template <class TScEvent>
void TScElementaryEventSubscription<TScEvent>::SetDelegate(DelegateFunc && func)
{
  m_delegate = func;
}

template <class TScEvent>
void TScElementaryEventSubscription<TScEvent>::RemoveDelegate()
{
  m_delegate = DelegateFunc();
}

template <class TScEvent>
sc_result TScElementaryEventSubscription<TScEvent>::Handler(
    sc_event_subscription const * event_subscription,
    sc_addr userAddr,
    sc_addr connectorAddr,
    sc_type connectorType,
    sc_addr otherAddr)
{
  sc_result result = SC_RESULT_ERROR;

  auto * eventSubscription = (TScElementaryEventSubscription *)sc_event_subscription_get_data(event_subscription);

  DelegateFunc delegateFunc = eventSubscription->m_delegate;
  if (delegateFunc == nullptr)
    return result;

  try
  {
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
sc_result TScElementaryEventSubscription<TScEvent>::HandlerDelete(sc_event_subscription const * event_subscription)
{
  auto * eventSubscription = (TScElementaryEventSubscription *)sc_event_subscription_get_data(event_subscription);

  utils::ScLockScope lock(eventSubscription->m_lock);
  if (eventSubscription->m_event_subscription)
  {
    eventSubscription->m_delegate = nullptr;
    eventSubscription->m_event_subscription = nullptr;
  }

  return SC_RESULT_OK;
}

template <ScType const & arcType>
ScEventSubscriptionAddOutputArc<arcType>::ScEventSubscriptionAddOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventAddOutputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventAddOutputArc<arcType>>(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of adding output sc-arc due subscription sc-element is not valid.");
}

template <ScType const & arcType>
ScEventSubscriptionAddInputArc<arcType>::ScEventSubscriptionAddInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventAddInputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventAddInputArc<arcType>>(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of adding input sc-arc due subscription sc-element is not valid.");
}

template <ScType const & edgeType>
ScEventSubscriptionAddEdge<edgeType>::ScEventSubscriptionAddEdge(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventAddEdge<edgeType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventAddEdge<edgeType>>(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of adding sc-edge due subscription sc-element is not valid.");
}

template <ScType const & arcType>
ScEventSubscriptionRemoveOutputArc<arcType>::ScEventSubscriptionRemoveOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventRemoveOutputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventRemoveOutputArc<arcType>>(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of removing output sc-arc due subscription sc-element is not valid.");
}

template <ScType const & arcType>
ScEventSubscriptionRemoveInputArc<arcType>::ScEventSubscriptionRemoveInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventRemoveInputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventRemoveInputArc<arcType>>(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of removing input sc-arc due subscription sc-element is not valid.");
}

template <ScType const & edgeType>
ScEventSubscriptionRemoveEdge<edgeType>::ScEventSubscriptionRemoveEdge(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventRemoveEdge<edgeType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventRemoveEdge<edgeType>>(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of removing sc-edge due subscription sc-element is not valid.");
}
