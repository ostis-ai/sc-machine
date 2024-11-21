/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_agent_manager.hpp"

#include "sc-memory/sc_action.hpp"
#include "sc-memory/sc_structure.hpp"
#include "sc-memory/sc_result.hpp"
#include "sc-memory/sc_event_subscription.hpp"
#include "sc-memory/sc_keynodes.hpp"

template <class TScAgent>
template <class... TScAddr>
void ScAgentManager<TScAgent>::Subscribe(
    ScMemoryContext * context,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs) noexcept(false)
{
  static_assert(
      std::is_base_of<ScAgent<TScEvent, TScContext>, TScAgent>::value,
      "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetInitiator(context->GetUser());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentClassName = GetAgentClassName(context, agent);
  std::string const & agentImplementationInfo = GetAgentImplementationInfo(context, agent, agentImplementationAddr);

  auto agentImplementationsToSubscriptions = ResolveAgentClassAgentImplementationSubscriptions(agentClassName);

  auto subscriptions =
      GetAgentImplementationSubscriptions(agentImplementationAddr, agentImplementationsToSubscriptions);
  if (!subscriptions)
    subscriptions =
        GenerateAgentImplementationSubscriptions(agentImplementationAddr, agentImplementationsToSubscriptions);
  else if (context->IsElement(agentImplementationAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Specified " << agentImplementationInfo << " has already been subscribed to event.");

  // If agent is specified in knowledge base, then subscriptionAddrs will be empty and subscriptionVector will contain
  // only subscription sc-element from knowledge base, otherwise (when agent is not specified in knowledge base)
  // subscriptionAddrs will be not empty and subscriptionVector will equal to subscriptionAddrs.
  ScAddrVector subscriptionVector{subscriptionAddrs...};
  if (agent.MayBeSpecified() || subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventClassName = GetEventClassName(context, agent);
  for (ScAddr const & subscriptionElementAddr : subscriptionVector)
  {
    std::string const & subscriptionElementName = GetSubscriptionElementName(context, subscriptionElementAddr);

    if (!context->IsElement(subscriptionElementAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to subscribe " << agentImplementationInfo << " to event `" << eventClassName
                                   << " because subscription sc-element `" << subscriptionElementName
                                   << "` is not valid.");

    auto const & subscription = GetSubscription(subscriptionElementAddr, *subscriptions);
    if (subscription)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified " << agentImplementationInfo << " has already been subscribed to event `" << eventClassName
                       << "` with subscription sc-element `" << subscriptionElementName << "`.");

    SC_LOG_INFO(
        "Subscribe " << agentImplementationInfo << " to event `" << eventClassName << "` with subscription sc-element `"
                     << subscriptionElementName << "`.");

    std::function<void(void)> postEraseEventCallback;
    if constexpr (std::is_same<ScElementaryEvent, TScEvent>::value)
    {
      ScAddr const & eventClassAddr = agent.GetEventClass();
      if (eventClassAddr == ScKeynodes::sc_event_before_erase_element)
        postEraseEventCallback =
            GetPostEraseEventCallback(agentClassName, eventClassName, agentImplementationAddr, subscriptionElementAddr);

      subscriptions->get().insert(
          {subscriptionElementAddr,
           new ScElementaryEventSubscription(
               *context,
               eventClassAddr,
               subscriptionElementAddr,
               ScAgentManager<TScAgent>::GetCallback(agentImplementationAddr, postEraseEventCallback))});
      ScAgentManager<TScAgent>::m_agentEventClasses.insert({agentClassName, {eventClassAddr, subscriptionElementAddr}});
    }
    else
    {
      if constexpr (std::is_same<ScEventBeforeEraseElement, TScEvent>::value)
        postEraseEventCallback =
            GetPostEraseEventCallback(agentClassName, eventClassName, agentImplementationAddr, subscriptionElementAddr);

      subscriptions->get().insert(
          {subscriptionElementAddr,
           new ScElementaryEventSubscription<TScEvent>(
               *context,
               subscriptionElementAddr,
               ScAgentManager<TScAgent>::GetCallback(agentImplementationAddr, postEraseEventCallback))});
      ScAgentManager<TScAgent>::m_agentEventClasses.insert(
          {agentClassName, {TScEvent::eventClassAddr, subscriptionElementAddr}});
    }
  }
}

template <class TScAgent>
template <class... TScAddr>
void ScAgentManager<TScAgent>::Unsubscribe(
    ScMemoryContext * context,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs) noexcept(false)
{
  static_assert(
      std::is_base_of<ScAgent<TScEvent, TScContext>, TScAgent>::value,
      "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetInitiator(context->GetUser());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentClassName = GetAgentClassName(context, agent);
  std::string const & agentImplementationInfo = GetAgentImplementationInfo(context, agent, agentImplementationAddr);

  if (WasAgentSubscribedToEventOfErasedElementErasing(context, agentClassName))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Specified " << agentImplementationInfo
                     << " has been subscribed to event of erasing sc-element. It does not need to be unsubscribed, the "
                        "sc-machine unsubscribes such an agent on its own.");

  auto agentImplementationsToSubscriptions = GetAgentClassAgentImplementationSubscriptions(agentClassName);
  if (!agentImplementationsToSubscriptions)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Specified " << agentImplementationInfo << " hasn't been subscribed to any events yet.");

  auto subscriptions =
      GetAgentImplementationSubscriptions(agentImplementationAddr, *agentImplementationsToSubscriptions);
  if (!subscriptions)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Specified " << agentImplementationInfo << " hasn't been subscribed to any events yet.");

  // If agent is specified in knowledge base, then subscriptionAddrs will be empty and subscriptionVector will contain
  // only subscription sc-element from knowledge base, otherwise (when agent is not specified in knowledge base)
  // subscriptionAddrs will be not empty and subscriptionVector will equal to subscriptionAddrs.
  ScAddrVector subscriptionVector{subscriptionAddrs...};
  if (agent.MayBeSpecified() || subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventClassName = GetEventClassName(context, agent);
  for (ScAddr const & subscriptionElementAddr : subscriptionVector)
  {
    std::string const & subscriptionElementName = GetSubscriptionElementName(context, subscriptionElementAddr);

    if (!context->IsElement(subscriptionElementAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to unsubscribe " << agentImplementationInfo << " from event `" << eventClassName
                                     << " because subscription sc-element `" << subscriptionElementName
                                     << "` is not valid.");

    auto const & subscription = GetSubscription(subscriptionElementAddr, *subscriptions);
    if (!subscription)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified " << agentImplementationInfo << " hasn't have been subscribed to event `" << eventClassName
                       << "` with subscription sc-element `" << subscriptionElementName << "` yet.");

    SC_LOG_INFO(
        "Unsubscribe " << agentImplementationInfo << " from event `" << eventClassName
                       << "` with subscription sc-element `" << subscriptionElementName << "`.");

    delete subscription;
    EraseSubscription(subscriptionElementAddr, *subscriptions);
  }

  ClearEmptyAgentImplementationSubscriptions(
      agentClassName, agentImplementationAddr, agentImplementationsToSubscriptions, subscriptions);
}

template <class TScAgent>
std::string ScAgentManager<TScAgent>::GetAgentClassName(ScMemoryContext * context, TScAgent & agent)
{
  std::string agentClassName;
  if (agent.MayBeSpecified())
  {
    ScAddr const & abstractAgentAddr = agent.GetAbstractAgent();
    agentClassName = context->GetElementSystemIdentifier(abstractAgentAddr);
  }

  if (agentClassName.empty())
    agentClassName = agent.GetName();

  return agentClassName;
}

template <class TScAgent>
std::string ScAgentManager<TScAgent>::GetAgentImplementationName(
    ScMemoryContext * context,
    ScAddr const & agentImplementationAddr)
{
  std::string agentImplementationName = context->GetElementSystemIdentifier(agentImplementationAddr);

  if (agentImplementationName.empty())
    agentImplementationName = std::to_string(agentImplementationAddr.Hash());

  return agentImplementationName;
}

template <class TScAgent>
std::string ScAgentManager<TScAgent>::GetAgentImplementationInfo(
    ScMemoryContext * context,
    TScAgent & agent,
    ScAddr const & agentImplementationAddr)
{
  std::string const & agentClassName = GetAgentClassName(context, agent);

  std::stringstream agentImplementationInfo;
  if (context->IsElement(agentImplementationAddr))
    agentImplementationInfo << "agent implementation `" << GetAgentImplementationName(context, agentImplementationAddr)
                            << "` with abstract agent `" << agentClassName << "`";
  else
    agentImplementationInfo << "agent implementation `" << agentClassName << "`";

  return agentImplementationInfo.str();
}

template <class TScAgent>
std::string ScAgentManager<TScAgent>::GetSubscriptionElementName(
    ScMemoryContext * context,
    ScAddr const & subscriptionElementAddr)
{
  std::string subscriptionElementName = context->GetElementSystemIdentifier(subscriptionElementAddr);
  if (subscriptionElementName.empty())
    subscriptionElementName = std::to_string(subscriptionElementAddr.Hash());

  return subscriptionElementName;
}

template <class TScAgent>
std::string ScAgentManager<TScAgent>::GetEventClassName(ScMemoryContext * context, TScAgent & agent)
{
  ScAddr eventClassAddr;
  if constexpr (std::is_same<ScElementaryEvent, TScEvent>::value)
    eventClassAddr = agent.GetEventClass();
  else
    return ScEvent::GetName<TScEvent>();

  std::string eventClassName = context->GetElementSystemIdentifier(eventClassAddr);
  if (eventClassName.empty())
    eventClassName = std::to_string(eventClassAddr.Hash());

  return eventClassName;
}

template <class TScAgent>
bool ScAgentManager<TScAgent>::WasAgentSubscribedToEventOfErasedElementErasing(
    ScMemoryContext * context,
    std::string const & agentClassName)
{
  auto const & eventClassIt = ScAgentManager<TScAgent>::m_agentEventClasses.find(agentClassName);
  return eventClassIt != ScAgentManager<TScAgent>::m_agentEventClasses.cend()
         && eventClassIt->second.first == ScKeynodes::sc_event_before_erase_element
         && !context->IsElement(eventClassIt->second.second);
}

template <class TScAgent>
typename ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptionsRef ScAgentManager<
    TScAgent>::ResolveAgentClassAgentImplementationSubscriptions(std::string const & agentClassName)
{
  if (!ScAgentManager<TScAgent>::m_agentClassesToAgentImplementationSubscriptions.count(agentClassName))
    ScAgentManager<TScAgent>::m_agentClassesToAgentImplementationSubscriptions.insert({agentClassName, {}});

  return *GetAgentClassAgentImplementationSubscriptions(agentClassName);
}

template <class TScAgent>
typename ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptionsOptionalRef ScAgentManager<
    TScAgent>::GetAgentClassAgentImplementationSubscriptions(std::string const & agentClassName)
{
  auto const & agentImplementationsToSubscriptionsIt =
      ScAgentManager<TScAgent>::m_agentClassesToAgentImplementationSubscriptions.find(agentClassName);
  if (agentImplementationsToSubscriptionsIt
      == ScAgentManager<TScAgent>::m_agentClassesToAgentImplementationSubscriptions.cend())
    return std::nullopt;

  return std::ref(agentImplementationsToSubscriptionsIt->second);
}

template <class TScAgent>
typename ScAgentManager<TScAgent>::ScSubscriptionsRef ScAgentManager<TScAgent>::
    GenerateAgentImplementationSubscriptions(
        ScAddr const & agentImplementationAddr,
        ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptions & agentImplementationsToSubscriptions)
{
  agentImplementationsToSubscriptions.insert({agentImplementationAddr, {}});

  return *GetAgentImplementationSubscriptions(agentImplementationAddr, agentImplementationsToSubscriptions);
}

template <class TScAgent>
typename ScAgentManager<TScAgent>::ScSubscriptionsOptionalRef ScAgentManager<TScAgent>::
    GetAgentImplementationSubscriptions(
        ScAddr const & agentImplementationAddr,
        ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptions & agentImplementationsToSubscriptions)
{
  auto const & subscriptionsIt = agentImplementationsToSubscriptions.find(agentImplementationAddr);
  if (subscriptionsIt == agentImplementationsToSubscriptions.cend())
    return std::nullopt;

  return std::ref(subscriptionsIt->second);
}

template <class TScAgent>
ScEventSubscription * ScAgentManager<TScAgent>::GetSubscription(
    ScAddr const & subscriptionElementAddr,
    ScSubscriptions const & subscriptions)
{
  auto const & it = subscriptions.find(subscriptionElementAddr);
  if (it == subscriptions.cend())
    return nullptr;

  return it->second;
}

template <class TScAgent>
void ScAgentManager<TScAgent>::EraseSubscription(
    ScAddr const & subscriptionElementAddr,
    ScSubscriptions & subscriptions)
{
  subscriptions.erase(subscriptionElementAddr);
}

template <class TScAgent>
void ScAgentManager<TScAgent>::ClearEmptyAgentImplementationSubscriptions(
    std::string const & agentClassName,
    ScAddr const & agentImplementationAddr,
    ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptionsOptionalRef & agentImplementationsToSubscriptions,
    ScSubscriptionsOptionalRef const & subscriptions)
{
  if (subscriptions->get().empty())
    agentImplementationsToSubscriptions->get().erase(agentImplementationAddr);

  if (agentImplementationsToSubscriptions->get().empty())
    ScAgentManager<TScAgent>::m_agentClassesToAgentImplementationSubscriptions.erase(agentClassName);
}

template <class TScAgent>
std::function<void(void)> ScAgentManager<TScAgent>::GetPostEraseEventCallback(
    std::string const & agentClassName,
    std::string const & eventClassName,
    ScAddr const & agentImplementationAddr,
    ScAddr const & subscriptionElementAddr)
{
  return [=]() -> void
  {
    auto agentImplementationsToSubscriptions = GetAgentClassAgentImplementationSubscriptions(agentClassName);
    if (!agentImplementationsToSubscriptions)
    {
      // TODO(NikitaZotov): Implement transactions in sc-memory
      return;
    }

    auto subscriptions =
        GetAgentImplementationSubscriptions(agentImplementationAddr, *agentImplementationsToSubscriptions);
    if (!subscriptions)
    {
      // TODO(NikitaZotov): Implement transactions in sc-memory
      return;
    }

    SC_LOG_INFO(
        "Unsubscribe agent `" << agentClassName << "` from event `" << eventClassName
                              << "` with subscription sc-element `" << subscriptionElementAddr.Hash() << "`.");

    EraseSubscription(subscriptionElementAddr, *subscriptions);
    ClearEmptyAgentImplementationSubscriptions(
        agentClassName, agentImplementationAddr, agentImplementationsToSubscriptions, subscriptions);
  };
}

template <class TScAgent>
std::function<void(typename TScAgent::TEventType const &)> ScAgentManager<TScAgent>::GetCallback(
    ScAddr const & agentImplementationAddr,
    std::function<void(void)> const & postEraseEventCallback) noexcept
{
  static_assert(
      std::is_base_of<ScAgent<TScEvent, TScContext>, TScAgent>::value,
      "TScAgent type must be derived from ScAgent type.");

  static_assert(
      HasNoMoreThanOneOverride<TScAgent>::InitiationConditionMethod::value,
      "TScAgent must have no more than one override method from methods: `GetInitiationCondition(void)`, "
      "`GetInitiationCondition(event)` "
      "and `CheckInitiationCondition`.");

  static_assert(
      HasNoMoreThanOneOverride<TScAgent>::ResultConditionMethod::value,
      "TScAgent must have no more than one override method from methods: `GetResultCondition`, "
      "`GetResultConditionTemplate` and "
      "`CheckResultCondition`.");

  static_assert(
      HasOneOverride<TScAgent>::DoProgramMethod::value, "TScAgent must have one override `DoProgram` method.");

  return [agentImplementationAddr, postEraseEventCallback](TScEvent const & event) -> void
  {
    auto const & ResolveAction = [](TScEvent const & event, ScAgent<TScEvent, TScContext> & agent) -> ScAction
    {
      auto [subscriptionElementAddr, _, otherElementAddr] = event.GetTriple();
      if (subscriptionElementAddr == ScKeynodes::action_initiated)
      {
        ScAddr const & actionAddr = otherElementAddr;
        return agent.m_context.ConvertToAction(actionAddr);
      }

      return agent.m_context.GenerateAction(agent.GetActionClass()).Initiate();
    };

    auto const & PostCallback = [&postEraseEventCallback]() -> void
    {
      if (postEraseEventCallback)
        postEraseEventCallback();
    };

    TScAgent agent;
    agent.SetInitiator(event.GetUser());
    agent.SetImplementation(agentImplementationAddr);

    std::string const & agentName = GetAgentClassName(&agent.m_context, agent);
    // SC_LOG_INFO("Agent `" << agentName << "` reacted to primary initiation condition.");

    if (agent.IsActionClassDeactivated())
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was finished because actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return PostCallback();
    }

    // SC_LOG_INFO("Agent `" << agentName << "` started checking initiation condition.");
    bool isInitiationConditionCheckedSuccessfully = false;
    try
    {
      isInitiationConditionCheckedSuccessfully =
          agent.template ValidateInitiationCondition<TScAgent, HasOverride<TScAgent>>(event);
    }
    catch (utils::ScException const & exception)
    {
      SC_LOG_ERROR(
          "Not able to check initiation condition template, because error was occurred. " << exception.Message());
    }

    if (!isInitiationConditionCheckedSuccessfully)
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was finished because its initiation condition was checked unsuccessfully.");
      return PostCallback();
    }
    // SC_LOG_INFO("Agent `" << agentName << "` finished checking initiation condition.");

    ScAction action = ResolveAction(event, agent);
    ScResult result;

    try
    {
      SC_LOG_INFO("Agent `" << agentName << "` started performing action.");
      if constexpr (HasOverride<TScAgent>::DoProgramWithEventArgument::value)
        result = agent.DoProgram(event, action);
      else
        result = agent.DoProgram(action);
    }
    catch (utils::ScException const & exception)
    {
      try
      {
        action.FinishWithError();
        SC_LOG_ERROR(
            "Agent `" << agentName << "` was finished because error was occurred.\nError description:\n"
                      << exception.Description());
      }
      catch (utils::ScException const & finishingActionException)
      {
        SC_LOG_ERROR(
            "It was tried to finish agent `"
            << agentName << "` because error was occurred.\nError description:\n"
            << exception.Description() << "\nBut agent `" << agentName
            << "` can not be finished because error was occurred during its finishing.\nError description:\n"
            << finishingActionException.Description());
      }
      return PostCallback();
    }

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agentName << "` finished performing action successfully.");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agentName << "` finished performing action unsuccessfully.");
    else
      SC_LOG_INFO("Agent `" << agentName << "` finished performing action with error.");

    // SC_LOG_INFO("Agent `" << agentName << "` started checking result condition.");
    bool isResultConditionCheckedSuccessfully = false;
    try
    {
      isResultConditionCheckedSuccessfully =
          agent.template ValidateResultCondition<TScAgent, HasOverride<TScAgent>>(event, action);
    }
    catch (utils::ScException const & exception)
    {
      SC_LOG_ERROR("Not able to check result condition template, because error was occurred. " << exception.Message());
    }

    if (!isResultConditionCheckedSuccessfully)
    {
      SC_LOG_WARNING("Result condition of agent `" << agentName << "` checked unsuccessfully.");
      return PostCallback();
    }
    // SC_LOG_INFO("Agent `" << agentName << "` finished checking result condition.");

    return PostCallback();
  };
}
