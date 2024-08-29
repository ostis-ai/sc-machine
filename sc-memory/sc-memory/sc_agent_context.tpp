/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include "sc_event_wait.hpp"

#include "sc_agent_builder.hpp"

#include "sc_agent_manager.hpp"

template <class TScEvent>
std::shared_ptr<ScElementaryEventSubscription<TScEvent>> ScAgentContext::CreateElementaryEventSubscription(
    ScAddr const & subscriptionElementAddr,
    std::function<void(TScEvent const &)> const & eventCallback)
{
  ValidateEventElements<TScEvent>(subscriptionElementAddr, "elementary sc-event subscription");

  return std::shared_ptr<ScElementaryEventSubscription<TScEvent>>(
      new ScElementaryEventSubscription<TScEvent>(*this, subscriptionElementAddr, eventCallback));
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::CreateEventWaiter(
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback)
{
  ValidateEventElements<TScEvent>(subscriptionElementAddr, "sc-event waiter");

  auto eventWait =
      std::shared_ptr<ScEventWaiter<TScEvent>>(new ScEventWaiter<TScEvent>(*this, subscriptionElementAddr));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::CreateConditionWaiter(
    ScAddr const & subscriptionElementAddr,
    std::function<void(void)> const & initiateCallback,
    std::function<bool(TScEvent const &)> const & checkCallback)
{
  ValidateEventElements<TScEvent>(subscriptionElementAddr, "condition waiter");

  auto eventWait = std::shared_ptr<ScConditionWaiter<TScEvent>>(
      new ScConditionWaiter<TScEvent>(*this, subscriptionElementAddr, checkCallback));
  eventWait->SetOnWaitStartDelegate(initiateCallback);
  return eventWait;
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::CreateConditionWaiter(
    ScAddr const & subscriptionElementAddr,
    std::function<bool(TScEvent const &)> const & checkCallback)
{
  return CreateConditionWaiter(subscriptionElementAddr, {}, checkCallback);
}

template <class TScAgent, class... TScAddr>
typename std::enable_if<!std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type ScAgentContext::SubscribeAgent(
    TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  ScAgentManager<TScAgent>::Subscribe(this, ScAddr::Empty, subscriptionAddrs...);
}

template <class TScAgent>
typename std::enable_if<std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type ScAgentContext::SubscribeAgent()
{
  ScAgentManager<TScAgent>::Subscribe(this, ScAddr::Empty);
}

template <class TScAgent, class... TScAddr>
typename std::enable_if<!std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type ScAgentContext::
    UnsubscribeAgent(TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  ScAgentManager<TScAgent>::Unsubscribe(this, ScAddr::Empty, subscriptionAddrs...);
}

template <class TScAgent>
typename std::enable_if<std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type ScAgentContext::
    UnsubscribeAgent()
{
  ScAgentManager<TScAgent>::Unsubscribe(this, ScAddr::Empty);
}

template <class TScAgent>
void ScAgentContext::LoadAndSubscribeAgent(ScAddr const & agentImplementationAddr)
{
  ScAgentBuilder<TScAgent> builder{agentImplementationAddr};
  builder.ResolveSpecification(this);

  ScAgentManager<TScAgent>::Subscribe(this, agentImplementationAddr);
}

template <class TScAgent>
void ScAgentContext::DestroyAndUnsubscribeAgent(ScAddr const & agentImplementationAddr)
{
  ScAgentManager<TScAgent>::Unsubscribe(this, agentImplementationAddr);
}

template <class TScEvent>
void ScAgentContext::ValidateEventElements(ScAddr const & subscriptionElementAddr, std::string const & validatorName)
{
  static_assert(
      std::is_base_of<ScElementaryEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  if (!IsElement(subscriptionElementAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to create " << validatorName << " because subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventBeforeChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to create " << validatorName
                                << " of changing link content because subscription sc-element is not "
                                   "sc-link.");
  }
}
