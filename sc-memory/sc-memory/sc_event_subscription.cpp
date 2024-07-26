/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.hpp"

#include "sc_memory.hpp"

#include "sc_keynodes.hpp"

ScEventSubscription::~ScEventSubscription() = default;

sc_result ScEventSubscription::Handler(sc_event const *, sc_addr, sc_addr, sc_type, sc_addr)
{
  return SC_RESULT_OK;
}

sc_result ScEventSubscription::HandlerDelete(sc_event const *)
{
  return SC_RESULT_OK;
}

ScElementaryEventSubscription::ScElementaryEventSubscription()
  : m_event(nullptr)
  , m_delegate()
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
  m_event = sc_event_with_user_new(
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
  if (m_event)
    sc_event_destroy(m_event);
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
    sc_event const * event,
    sc_addr userAddr,
    sc_addr connectorAddr,
    sc_type connectorType,
    sc_addr otherAddr)
{
  sc_result result = SC_RESULT_ERROR;

  auto * eventObj = (ScElementaryEventSubscription *)sc_event_get_data(event);

  if (eventObj == nullptr)
    return result;

  DelegateFunc delegateFunc = eventObj->m_delegate;
  if (delegateFunc == nullptr)
    return result;

  try
  {
    delegateFunc(ScElementaryEvent(userAddr, sc_event_get_element(event), connectorAddr, connectorType, otherAddr));
  }
  catch (utils::ScException & e)
  {
    SC_LOG_ERROR("Uncaught exception: " << e.Message());
  }

  return SC_RESULT_OK;
}

sc_result ScElementaryEventSubscription::HandlerDelete(sc_event const * event)
{
  auto * eventObj = (ScElementaryEventSubscription *)sc_event_get_data(event);

  utils::ScLockScope lock(eventObj->m_lock);
  if (eventObj->m_event)
  {
    eventObj->m_delegate = nullptr;
    eventObj->m_event = nullptr;
  }

  return SC_RESULT_OK;
}

ScEventSubscriptionEraseElement::ScEventSubscriptionEraseElement(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventEraseElement const &)> const & func)
  : TScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionChangeContent::ScEventSubscriptionChangeContent(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventChangeContent const &)> const & func)
  : TScElementaryEventSubscription(ctx, addr, func)
{
}
