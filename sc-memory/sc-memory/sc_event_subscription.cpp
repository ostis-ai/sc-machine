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

void ScEventSubscription::Initialize(ScMemoryContext *, ScAddr const &) {}

void ScEventSubscription::Shutdown(ScMemoryContext *) {}

ScEventSubscriptionAddOutputArc::ScEventSubscriptionAddOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddOutputArc const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionAddInputArc::ScEventSubscriptionAddInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddInputArc const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionAddEdge::ScEventSubscriptionAddEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventAddEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveOutputArc::ScEventSubscriptionRemoveOutputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveOutputArc const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveInputArc::ScEventSubscriptionRemoveInputArc(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveInputArc const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionRemoveEdge::ScEventSubscriptionRemoveEdge(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventRemoveEdge const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionEraseElement::ScEventSubscriptionEraseElement(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventEraseElement const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

ScEventSubscriptionChangeContent::ScEventSubscriptionChangeContent(
    ScMemoryContext const & ctx,
    ScAddr const & addr,
    std::function<void(ScEventChangeContent const &)> const & func)
  : ScElementaryEventSubscription(ctx, addr, func)
{
}

std::unordered_map<ScAddr, ScEventSubscriptionFactory::ScCreateEventSubscriptionCallback, ScAddrHashFunc<sc_uint32>>
    ScEventSubscriptionFactory::m_eventTypesToCreateSubscriptionCallbacks;

void ScEventSubscriptionFactory::Initialize(ScMemoryContext *, ScAddr const &)
{
  m_eventTypesToCreateSubscriptionCallbacks = {
      {ScKeynodes::event_add_output_arc, CreateEventSubscriptionWrapper<sc_event_add_output_arc>()},
      {ScKeynodes::event_add_input_arc, CreateEventSubscriptionWrapper<sc_event_add_input_arc>()},
      {ScKeynodes::event_add_edge, CreateEventSubscriptionWrapper<sc_event_add_edge>()},
      {ScKeynodes::event_remove_output_arc, CreateEventSubscriptionWrapper<sc_event_remove_output_arc>()},
      {ScKeynodes::event_remove_input_arc, CreateEventSubscriptionWrapper<sc_event_remove_input_arc>()},
      {ScKeynodes::event_remove_edge, CreateEventSubscriptionWrapper<sc_event_remove_edge>()},
      {ScKeynodes::event_erase_element, CreateEventSubscriptionWrapper<sc_event_erase_element>()},
      {ScKeynodes::event_erase_element, CreateEventSubscriptionWrapper<sc_event_change_content>()},
  };
}

void ScEventSubscriptionFactory::Shutdown(ScMemoryContext *)
{
  m_eventTypesToCreateSubscriptionCallbacks.clear();
}

ScEventSubscription * ScEventSubscriptionFactory::CreateSubscription(
    ScMemoryContext * context,
    ScAddr const & evenTypeAddr,
    ScAddr const & subscriptionAddr,
    ScEventCallback const & onEventCallback)
{
  auto const & it = m_eventTypesToCreateSubscriptionCallbacks.find(evenTypeAddr);
  if (it == m_eventTypesToCreateSubscriptionCallbacks.cend())
    return nullptr;

  auto const & createSubscriptionFunc = it->second;
  return createSubscriptionFunc(context, subscriptionAddr, onEventCallback);
}

ScEventSubscription * ScEventSubscriptionFactory::CreateSubscription(
    ScMemoryContext * context,
    std::string const & eventTypeSystemIdtf,
    ScAddr const & subscriptionAddr,
    ScEventCallback const & onEventCallback)
{
  ScAddr const & eventTypeAddr = context->HelperFindBySystemIdtf(eventTypeSystemIdtf);
  if (!eventTypeAddr.IsValid())
    return nullptr;

  return CreateSubscription(context, eventTypeAddr, subscriptionAddr, onEventCallback);
}
