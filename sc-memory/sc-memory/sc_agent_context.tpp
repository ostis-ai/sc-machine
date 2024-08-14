/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include "sc_event_wait.hpp"

#include "sc_agent_builder.hpp"

template <class TScEvent>
std::shared_ptr<ScElementaryEventSubscription<TScEvent>> ScAgentContext::CreateElementaryEventSubscription(
    ScAddr const & subscriptionElementAddr,
    std::function<void(TScEvent const &)> const & eventCallback)
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event subscription due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event subscription of changing link content due subscription sc-element is not "
          "sc-link.");
  }

  return std::shared_ptr<ScElementaryEventSubscription<TScEvent>>(
      new ScElementaryEventSubscription<TScEvent>(*this, subscriptionElementAddr, eventCallback));
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::CreateEventWaiter(
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback)
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams, "Not able to create sc-event waiter due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event waiter of changing link content due subscription sc-element is not "
          "sc-link.");
  }

  auto eventWait =
      std::shared_ptr<ScEventWaiter<TScEvent>>(new ScEventWaiter<TScEvent>(*this, subscriptionElementAddr));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::CreateEventWaiterWithCondition(
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback,
    std::function<sc_bool(TScEvent const &)> const & checkCallback)
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create sc-event waiter with condition due subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create sc-event waiter with condition of changing link content due subscription sc-element is "
          "not "
          "sc-link.");
  }

  auto eventWait = std::shared_ptr<ScConditionWaiter<TScEvent>>(
      new ScConditionWaiter<TScEvent>(*this, subscriptionElementAddr, checkCallback));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::CreateEventWaiterWithCondition(
    ScAddr const & subscriptionElementAddr,
    std::function<sc_bool(TScEvent const &)> const & checkCallback)
{
  return CreateEventWaiterWithCondition(subscriptionElementAddr, {}, checkCallback);
}

template <class TScAgent, class... TScAddr>
typename std::enable_if<!std::is_base_of<ScActionAgent, TScAgent>::value>::type ScAgentContext::SubscribeAgent(
    TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent::template Subscribe<TScAgent>(this, ScAddr::Empty, subscriptionAddrs...);
}

template <class TScAgent>
typename std::enable_if<std::is_base_of<ScActionAgent, TScAgent>::value>::type ScAgentContext::SubscribeAgent()
{
  TScAgent::template Subscribe<TScAgent>(this, ScAddr::Empty, ScKeynodes::action_initiated);
}

template <class TScAgent, class... TScAddr>
typename std::enable_if<!std::is_base_of<ScActionAgent, TScAgent>::value>::type ScAgentContext::UnsubscribeAgent(
    TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent::template Unsubscribe<TScAgent>(this, ScAddr::Empty, subscriptionAddrs...);
}

template <class TScAgent>
typename std::enable_if<std::is_base_of<ScActionAgent, TScAgent>::value>::type ScAgentContext::UnsubscribeAgent()
{
  TScAgent::template Unsubscribe<TScAgent>(this, ScAddr::Empty, ScKeynodes::action_initiated);
}

template <class TScAgent>
void ScAgentContext::LoadAndSubscribeAgent(ScAddr const & agentImplementationAddr)
{
  ScAgentBuilder<TScAgent> builder{agentImplementationAddr};
  builder.LoadSpecification(this);

  TScAgent::template Subscribe<TScAgent>(this, agentImplementationAddr);
}

template <class TScAgent>
void ScAgentContext::DestroyAndUnsubscribeAgent(ScAddr const & agentImplementationAddr)
{
  TScAgent::template Unsubscribe<TScAgent>(this, agentImplementationAddr);
}
