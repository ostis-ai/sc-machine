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
    ScAddr const & addr,
    DelegateFunc const & func)
{
  m_delegate = std::move(func);
  m_event = sc_event_with_user_new(
      *ctx,
      *addr,
      TScEvent::type,
      (sc_pointer)this,
      &ScElementaryEventSubscription::Handler,
      &ScElementaryEventSubscription::HandlerDelete);
}

template <class TScEvent>
ScElementaryEventSubscription<TScEvent>::~ScElementaryEventSubscription()
{
  if (m_event)
    sc_event_destroy(m_event);
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
    goto result;

  try
  {
    result = delegateFunc(TScEvent(userAddr, sc_event_get_element(event), connectorAddr, connectorType, otherAddr))
                 ? SC_RESULT_OK
                 : SC_RESULT_ERROR;
  }
  catch (utils::ScException & e)
  {
    SC_LOG_ERROR("Uncaught exception: " << e.Message());
  }

result:
  return result;
}

template <class TScEvent>
sc_result ScElementaryEventSubscription<TScEvent>::HandlerDelete(sc_event const * event)
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

template <>
struct ScEventTypeConverter<sc_event_add_input_arc>
{
  using SubscriptionType = ScEventSubscriptionAddInputEdge;
  using EventTypeClass = ScEventAddInputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_add_output_arc>
{
  using SubscriptionType = ScEventSubscriptionAddOutputEdge;
  using EventTypeClass = ScEventAddOutputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_remove_input_arc>
{
  using SubscriptionType = ScEventSubscriptionRemoveInputEdge;
  using EventTypeClass = ScEventRemoveInputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_remove_output_arc>
{
  using SubscriptionType = ScEventSubscriptionRemoveOutputEdge;
  using EventTypeClass = ScEventRemoveOutputEdge;
};

template <>
struct ScEventTypeConverter<sc_event_remove_element>
{
  using SubscriptionType = ScEventSubscriptionEraseElement;
  using EventTypeClass = ScEventRemoveElement;
};

template <>
struct ScEventTypeConverter<sc_event_change_content>
{
  using SubscriptionType = ScEventSubscriptionContentChanged;
  using EventTypeClass = ScEventChangeContent;
};

template <sc_event_type eventType>
ScEventSubscription * ScEventSubscriptionFactory::CreateEventSubscription(
    ScMemoryContext * context,
    ScAddr const & subscriptionAddr,
    ScEventCallback const & onEventFunc)
{
  return new ScEventSubscriptionType<eventType>(*context, subscriptionAddr, onEventFunc);
}

template <sc_event_type eventType>
ScEventSubscriptionFactory::ScCreateEventSubscriptionCallback ScEventSubscriptionFactory::
    CreateEventSubscriptionWrapper()
{
  return [](ScMemoryContext * context,
            ScAddr const & subscriptionAddr,
            ScEventCallback const & onEventCallback) -> ScEventSubscription *
  {
    return CreateEventSubscription<eventType>(context, subscriptionAddr, onEventCallback);
  };
}
