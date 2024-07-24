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
      TScEvent::elementType,
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

template <ScType const & arcType>
ScEventSubscriptionAddOutputArc<arcType>::ScEventSubscriptionAddOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddOutputArc<arcType> const &)> const & func)
  : ScElementaryEventSubscription<ScEventAddOutputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & arcType>
ScEventSubscriptionAddInputArc<arcType>::ScEventSubscriptionAddInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddInputArc<arcType> const &)> const & func)
  : ScElementaryEventSubscription<ScEventAddInputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & edgeType>
ScEventSubscriptionAddEdge<edgeType>::ScEventSubscriptionAddEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddEdge<edgeType> const &)> const & func)
  : ScElementaryEventSubscription<ScEventAddEdge<edgeType>>(ctx, addr, func)
{
}

template <ScType const & arcType>
ScEventSubscriptionRemoveOutputArc<arcType>::ScEventSubscriptionRemoveOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveOutputArc<arcType> const &)> const & func)
  : ScElementaryEventSubscription<ScEventRemoveOutputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & arcType>
ScEventSubscriptionRemoveInputArc<arcType>::ScEventSubscriptionRemoveInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveInputArc<arcType> const &)> const & func)
  : ScElementaryEventSubscription<ScEventRemoveInputArc<arcType>>(ctx, addr, func)
{
}

template <ScType const & edgeType>
ScEventSubscriptionRemoveEdge<edgeType>::ScEventSubscriptionRemoveEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveEdge<edgeType> const &)> const & func)
  : ScElementaryEventSubscription<ScEventRemoveEdge<edgeType>>(ctx, addr, func)
{
}

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_add_input_arc>
{
  using SubscriptionType = ScEventSubscriptionAddInputArc<ScType::Unknown>;
  using EventTypeClass = ScEventAddInputArc<ScType::Unknown>;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_add_output_arc>
{
  using SubscriptionType = ScEventSubscriptionAddOutputArc<ScType::Unknown>;
  using EventTypeClass = ScEventAddOutputArc<ScType::Unknown>;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_add_edge>
{
  using SubscriptionType = ScEventSubscriptionAddEdge<ScType::Unknown>;
  using EventTypeClass = ScEventAddEdge<ScType::Unknown>;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_remove_input_arc>
{
  using SubscriptionType = ScEventSubscriptionRemoveInputArc<ScType::Unknown>;
  using EventTypeClass = ScEventRemoveInputArc<ScType::Unknown>;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_remove_output_arc>
{
  using SubscriptionType = ScEventSubscriptionRemoveOutputArc<ScType::Unknown>;
  using EventTypeClass = ScEventRemoveOutputArc<ScType::Unknown>;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_remove_edge>
{
  using SubscriptionType = ScEventSubscriptionRemoveEdge<ScType::Unknown>;
  using EventTypeClass = ScEventRemoveEdge<ScType::Unknown>;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_erase_element>
{
  using SubscriptionType = ScEventSubscriptionEraseElement;
  using EventTypeClass = ScEventEraseElement;
};

template <>
struct ScEventSubscriptionFactory::ScEventTypeConverter<sc_event_change_content>
{
  using SubscriptionType = ScEventSubscriptionChangeContent;
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
