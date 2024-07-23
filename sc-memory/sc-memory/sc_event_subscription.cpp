/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_event_subscription.hpp"

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

sc_result ScEventSubscription::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

sc_result ScEventSubscription::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

ScEventSubscriptionAddOutputEdge::ScEventSubscriptionAddOutputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventAddOutputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionAddInputEdge::ScEventSubscriptionAddInputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventAddInputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveOutputEdge::ScEventSubscriptionRemoveOutputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventRemoveOutputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveInputEdge::ScEventSubscriptionRemoveInputEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventRemoveInputEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionEraseElement::ScEventSubscriptionEraseElement(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventRemoveElement const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionContentChanged::ScEventSubscriptionContentChanged(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<sc_result(ScEventChangeContent const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

std::unordered_map<
    ScEvent::Type,
    ScEventSubscriptionFactory::ScCreateEventSubscriptionCallback,
    ScEventSubscriptionFactory::ScEventTypeHashFunc>
    ScEventSubscriptionFactory::m_eventTypesToCreateSubscriptionCallbacks;

std::unordered_map<ScAddr, ScEvent::Type, ScAddrHashFunc<sc_uint32>> ScEventSubscriptionFactory::m_namesToEventTypes;

sc_result ScEventSubscriptionFactory::Initialize(ScMemoryContext *, ScAddr const &)
{
  m_eventTypesToCreateSubscriptionCallbacks = {
      {ScEvent::Type::AddOutputEdge, CreateEventSubscriptionWrapper<sc_event_add_output_arc>()},
      {ScEvent::Type::AddInputEdge, CreateEventSubscriptionWrapper<sc_event_add_input_arc>()},
      {ScEvent::Type::RemoveOutputEdge, CreateEventSubscriptionWrapper<sc_event_remove_output_arc>()},
      {ScEvent::Type::RemoveInputEdge, CreateEventSubscriptionWrapper<sc_event_remove_input_arc>()},
      {ScEvent::Type::RemoveElement, CreateEventSubscriptionWrapper<sc_event_remove_element>()},
      {ScEvent::Type::ChangeContent, CreateEventSubscriptionWrapper<sc_event_change_content>()},
  };
  m_namesToEventTypes = {
      {ScKeynodes::event_add_output_arc, ScEvent::Type::AddOutputEdge},
      {ScKeynodes::event_add_input_arc, ScEvent::Type::AddInputEdge},
      {ScKeynodes::event_remove_output_arc, ScEvent::Type::RemoveOutputEdge},
      {ScKeynodes::event_remove_input_arc, ScEvent::Type::RemoveInputEdge},
      {ScKeynodes::event_remove_element, ScEvent::Type::RemoveElement},
      {ScKeynodes::event_change_content, ScEvent::Type::ChangeContent},
  };
  return SC_RESULT_OK;
}

sc_result ScEventSubscriptionFactory::Shutdown(ScMemoryContext *)
{
  m_eventTypesToCreateSubscriptionCallbacks.clear();
  m_namesToEventTypes.clear();
  return SC_RESULT_OK;
}

ScEventSubscription * ScEventSubscriptionFactory::CreateSubscription(
    ScMemoryContext * context,
    ScEvent::Type const & eventType,
    ScAddr const & subscriptionAddr,
    ScEventCallback const & onEventCallback)
{
  auto const & it = m_eventTypesToCreateSubscriptionCallbacks.find(eventType);
  if (it == m_eventTypesToCreateSubscriptionCallbacks.cend())
    return nullptr;

  auto const & createSubscriptionFunc = it->second;
  return createSubscriptionFunc(context, subscriptionAddr, onEventCallback);
}

ScEventSubscription * ScEventSubscriptionFactory::CreateSubscription(
    ScMemoryContext * context,
    ScAddr const & evenTypeAddr,
    ScAddr const & subscriptionAddr,
    ScEventCallback const & onEventCallback)
{
  auto const & it = m_namesToEventTypes.find(evenTypeAddr);
  if (it == m_namesToEventTypes.cend())
    return nullptr;

  ScEvent::Type const & eventType = it->second;
  return CreateSubscription(context, eventType, subscriptionAddr, onEventCallback);
}

ScEventSubscription * ScEventSubscriptionFactory::CreateSubscription(
    ScMemoryContext * context,
    std::string const & eventTypeSystemIdtf,
    ScAddr const & subscriptionAddr,
    ScEventCallback const & onEventCallback)
{
  ScAddr const & evenTypeAddr = context->HelperFindBySystemIdtf(eventTypeSystemIdtf);
  if (!evenTypeAddr.IsValid())
    return nullptr;

  auto const & it = m_namesToEventTypes.find(evenTypeAddr);
  if (it == m_namesToEventTypes.cend())
    return nullptr;

  ScEvent::Type const & eventType = it->second;
  return CreateSubscription(context, eventType, subscriptionAddr, onEventCallback);
}

sc_event_type ScEventSubscriptionFactory::ScEventTypeHashFunc::operator()(ScEvent::Type const & eventType) const
{
  return eventType.m_realType;
}
