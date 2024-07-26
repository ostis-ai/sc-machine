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
  m_event = sc_event_with_user_new(
      *ctx,
      *subscriptionAddr,
      *TScEvent::eventType,
      *TScEvent::elementType,
      (sc_pointer)this,
      &TScElementaryEventSubscription::Handler,
      &TScElementaryEventSubscription::HandlerDelete);
}

template <class TScEvent>
TScElementaryEventSubscription<TScEvent>::~TScElementaryEventSubscription()
{
  if (m_event)
    sc_event_destroy(m_event);
}

template <class TScEvent>
sc_result TScElementaryEventSubscription<TScEvent>::Handler(
    sc_event const * event,
    sc_addr userAddr,
    sc_addr connectorAddr,
    sc_type connectorType,
    sc_addr otherAddr)
{
  sc_result result = SC_RESULT_ERROR;

  auto * eventObj = (TScElementaryEventSubscription *)sc_event_get_data(event);

  if (eventObj == nullptr)
    return result;

  DelegateFunc delegateFunc = eventObj->m_delegate;
  if (delegateFunc == nullptr)
    return result;

  try
  {
    delegateFunc(TScEvent(userAddr, sc_event_get_element(event), connectorAddr, connectorType, otherAddr));
  }
  catch (utils::ScException & e)
  {
    SC_LOG_ERROR("Uncaught exception: " << e.Message());
  }

  return SC_RESULT_OK;
}

template <class TScEvent>
sc_result TScElementaryEventSubscription<TScEvent>::HandlerDelete(sc_event const * event)
{
  auto * eventObj = (TScElementaryEventSubscription *)sc_event_get_data(event);

  utils::ScLockScope lock(eventObj->m_lock);
  if (eventObj->m_event)
  {
    eventObj->m_delegate = nullptr;
    eventObj->m_event = nullptr;
  }

  return SC_RESULT_OK;
}

template <ScType const & arcType>
ScEventSubscriptionAddOutputArc<arcType>::ScEventSubscriptionAddOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddOutputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventAddOutputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & arcType>
ScEventSubscriptionAddInputArc<arcType>::ScEventSubscriptionAddInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddInputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventAddInputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & edgeType>
ScEventSubscriptionAddEdge<edgeType>::ScEventSubscriptionAddEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddEdge<edgeType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventAddEdge<edgeType>>(ctx, addr, func)
{
}

template <ScType const & arcType>
ScEventSubscriptionRemoveOutputArc<arcType>::ScEventSubscriptionRemoveOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveOutputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventRemoveOutputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & arcType>
ScEventSubscriptionRemoveInputArc<arcType>::ScEventSubscriptionRemoveInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveInputArc<arcType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventRemoveInputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & edgeType>
ScEventSubscriptionRemoveEdge<edgeType>::ScEventSubscriptionRemoveEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveEdge<edgeType> const &)> const & func)
  : TScElementaryEventSubscription<ScEventRemoveEdge<edgeType>>(ctx, addr, func)
{
}
