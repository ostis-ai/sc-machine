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
std::shared_ptr<ScElementaryEventSubscription<TScEvent>> ScAgentContext::GenerateElementaryEventSubscription(
    ScAddr const & subscriptionElementAddr,
    std::function<void(TScEvent const &)> const & eventCallback)
{
  ValidateEventElements<TScEvent>(subscriptionElementAddr, "elementary sc-event subscription");

  return std::shared_ptr<ScElementaryEventSubscription<TScEvent>>(
      new ScElementaryEventSubscription<TScEvent>(*this, subscriptionElementAddr, eventCallback));
}

template <class TScEvent>
std::shared_ptr<ScWaiter> ScAgentContext::GenerateEventWaiter(
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
std::shared_ptr<ScWaiter> ScAgentContext::GenerateConditionWaiter(
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
std::shared_ptr<ScWaiter> ScAgentContext::GenerateConditionWaiter(
    ScAddr const & subscriptionElementAddr,
    std::function<bool(TScEvent const &)> const & checkCallback)
{
  return GenerateConditionWaiter(subscriptionElementAddr, {}, checkCallback);
}

template <class TScAgent, class... TScAddr>
void ScAgentContext::SubscribeAgent(TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  if constexpr (std::is_base_of<ScActionInitiatedAgent, TScAgent>::value)
    static_assert(
        sizeof...(TScAddr) == 0,
        "Not able to provide subscription sc-elements for TScAgent, because it inherits ScActionInitiatedAgent class "
        "for which `action_initiated` is used by default.");

  ScAgentManager<TScAgent>::Subscribe(this, ScAddr::Empty, subscriptionAddrs...);
}

template <class TScAgent, class... TScAddr>
void ScAgentContext::UnsubscribeAgent(TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  if constexpr (std::is_base_of<ScActionInitiatedAgent, TScAgent>::value)
    static_assert(
        sizeof...(TScAddr) == 0,
        "Not able to provide subscription sc-elements for TScAgent, because it inherits ScActionInitiatedAgent class "
        "for which `action_initiated` is used by default.");

  ScAgentManager<TScAgent>::Unsubscribe(this, ScAddr::Empty, subscriptionAddrs...);
}

template <class TScAgent>
void ScAgentContext::SubscribeSpecifiedAgent(ScAddr const & agentImplementationAddr)
{
  static_assert(
      !ScAgentManager<TScAgent>::template HasOverride<TScAgent>::GetEventClass::value,
      "`SubscribeSpecifiedAgent` method can be used for agent class that doesn't have override `GetEventClass` "
      "method.");
  static_assert(
      !ScAgentManager<TScAgent>::template HasOverride<TScAgent>::GetEventSubscriptionElement::value,
      "`SubscribeSpecifiedAgent` method can be used for agent class that doesn't have override "
      "`GetEventSubscriptionElement` method.");

  if (!IsElement(agentImplementationAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to subscribe specified agent `"
            << TScAgent::template GetName<TScAgent>()
            << "` by agent implementation because specified agent implementation is not valid.");

  ScAgentBuilder<TScAgent> builder{agentImplementationAddr};
  builder.ResolveSpecification(this);

  ScAgentManager<TScAgent>::Subscribe(this, agentImplementationAddr);
}

template <class TScAgent>
void ScAgentContext::UnsubscribeSpecifiedAgent(ScAddr const & agentImplementationAddr)
{
  static_assert(
      !ScAgentManager<TScAgent>::template HasOverride<TScAgent>::GetEventClass::value,
      "`SubscribeSpecifiedAgent` method can be used for agent class that doesn't have override `GetEventClass` "
      "method.");
  static_assert(
      !ScAgentManager<TScAgent>::template HasOverride<TScAgent>::GetEventSubscriptionElement::value,
      "`SubscribeSpecifiedAgent` method can be used for agent class that doesn't have override "
      "`GetEventSubscriptionElement` method.");

  if (!IsElement(agentImplementationAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to unsubscribe specified agent `"
            << TScAgent::template GetName<TScAgent>()
            << "` by agent implementation because specified agent implementation is not valid.");

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
        "Not able to generate " << validatorName << " because subscription sc-element is not valid.");

  if constexpr (std::is_same<TScEvent, ScEventBeforeChangeLinkContent>::value)
  {
    if (!GetElementType(subscriptionElementAddr).IsLink())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to generate " << validatorName
                                  << " of changing link content because subscription sc-element is not "
                                     "sc-link.");
  }
}
