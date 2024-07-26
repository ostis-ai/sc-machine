/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.hpp"

#include "sc_memory.hpp"

#include "sc_keynodes.hpp"

ScEventSubscription::~ScEventSubscription() = default;

ScElementaryEventSubscription::ScElementaryEventSubscription()
  : m_event_subscription(nullptr)
  , m_delegate(nullptr)
{
}

ScElementaryEventSubscription::ScElementaryEventSubscription(
    ScMemoryContext const & ctx,
    ScAddr const & eventTypeAddr,
    ScType const & elementType,
    ScAddr const & subscriptionAddr,
    DelegateFunc const & func)
{
  m_delegate = func;
  m_event_subscription = sc_event_subscription_with_user_new(
      *ctx,
      *subscriptionAddr,
      *eventTypeAddr,
      *elementType,
      (sc_pointer)this,
      &ScElementaryEventSubscription::Handler,
      &ScElementaryEventSubscription::HandlerDelete);
}

ScElementaryEventSubscription::~ScElementaryEventSubscription()
{
  if (m_event_subscription)
    sc_event_subscription_destroy(m_event_subscription);
}

void ScElementaryEventSubscription::SetDelegate(DelegateFunc && func)
{
  m_delegate = func;
}

void ScElementaryEventSubscription::RemoveDelegate()
{
  m_delegate = DelegateFunc();
}

sc_result ScElementaryEventSubscription::Handler(
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
    delegateFunc(ScElementaryEvent(
        userAddr, sc_event_subscription_get_element(event_subscription), connectorAddr, connectorType, otherAddr));
  }
  catch (utils::ScException & e)
  {
    SC_LOG_ERROR("Uncaught exception: " << e.Message());
  }

  return SC_RESULT_OK;
}

sc_result ScElementaryEventSubscription::HandlerDelete(sc_event_subscription const * event_subscription)
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

ScEventSubscriptionRemoveElement::ScEventSubscriptionRemoveElement(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventRemoveElement const &)> const & func)
  : TScElementaryEventSubscription(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of removing sc-element due subscription sc-element is not valid.");
}

ScEventSubscriptionChangeLinkContent::ScEventSubscriptionChangeLinkContent(
    ScMemoryContext const & ctx,
    ScAddr const & subscriptionAddr,
    std::function<void(ScEventChangeLinkContent const &)> const & func)
  : TScElementaryEventSubscription(ctx, subscriptionAddr, func)
{
  if (!ctx.IsElement(subscriptionAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of changing link content due subscription sc-element is not valid.");

  if (!ctx.GetElementType(subscriptionAddr).IsLink())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription of changing link content due subscription sc-element is not "
        "sc-link.");
}
