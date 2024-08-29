/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_agent_manager.hpp"

#include "sc_action.hpp"
#include "sc_structure.hpp"
#include "sc_result.hpp"
#include "sc_event_subscription.hpp"
#include "sc_keynodes.hpp"

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

  std::string const & agentName = agent.GetName();
  if (!ScAgentManager<TScAgent>::m_agentSubscriptions.count(agentName))
    ScAgentManager<TScAgent>::m_agentSubscriptions.insert({agentName, {}});

  // If agent is specified in knowledge base, then subscriptionAddrs will be empty and subscriptionVector will contain
  // only subscription sc-element from knowledge base, otherwise (when agent is not specified in knowledge base)
  // subscriptionAddrs will be not empty and subscriptionVector will equal to subscriptionAddrs.
  ScAddrVector subscriptionVector{subscriptionAddrs...};
  if (agent.MayBeSpecified() || subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = ScAgentManager<TScAgent>::m_agentSubscriptions.find(agentName)->second;
  for (ScAddr const & subscriptionElementAddr : subscriptionVector)
  {
    if (!context->IsElement(subscriptionElementAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to subscribe agent `" << agentName << "` to event `" << eventName
                                          << "because subscription sc-element with address hash `"
                                          << subscriptionElementAddr.Hash() << "` is not valid.");
    if (subscriptionsMap.find(subscriptionElementAddr) != subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` has already been subscribed to event `" << eventName << "("
                    << subscriptionElementAddr.Hash() << ")`.");

    SC_LOG_INFO(
        "Subscribe agent `" << agentName << "` to event `" << eventName << "(" << subscriptionElementAddr.Hash()
                            << ")`.");

    if constexpr (std::is_same<ScElementaryEvent, TScEvent>::value)
    {
      subscriptionsMap.insert(
          {subscriptionElementAddr,
           new ScElementaryEventSubscription(
               *context,
               agent.GetEventClass(),
               subscriptionElementAddr,
               ScAgentManager<TScAgent>::GetCallback(agentImplementationAddr))});
    }
    else
    {
      subscriptionsMap.insert(
          {subscriptionElementAddr,
           new ScElementaryEventSubscription<TScEvent>(
               *context, subscriptionElementAddr, ScAgentManager<TScAgent>::GetCallback(agentImplementationAddr))});
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

  std::string const & agentName = agent.GetName();
  auto const & agentsMapIt = ScAgentManager<TScAgent>::m_agentSubscriptions.find(agentName);
  if (agentsMapIt == ScAgentManager<TScAgent>::m_agentSubscriptions.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Agent `" << agentName << "` does not have been subscribed to any events yet.");

  // If agent is specified in knowledge base, then subscriptionAddrs will be empty and subscriptionVector will contain
  // only subscription sc-element from knowledge base, otherwise (when agent is not specified in knowledge base)
  // subscriptionAddrs will be not empty and subscriptionVector will equal to subscriptionAddrs.
  ScAddrVector subscriptionVector{subscriptionAddrs...};
  if (agent.MayBeSpecified() || subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventClassName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = agentsMapIt->second;
  for (ScAddr const & subscriptionElementAddr : subscriptionVector)
  {
    if constexpr (!std::is_same<TScEvent, ScEventBeforeEraseElement>::value)
    {
      if (!context->IsElement(subscriptionElementAddr))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Not able to unsubscribe agent `" << agentName << "` from event `" << eventClassName
                                              << "because subscription sc-element with address hash `"
                                              << subscriptionElementAddr.Hash() << "` is not valid.");
    }

    auto const & it = subscriptionsMap.find(subscriptionElementAddr);
    if (it == subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` does not have been subscribed to event `" << eventClassName << "("
                    << subscriptionElementAddr.Hash() << ")` yet.");

    SC_LOG_INFO(
        "Unsubscribe agent `" << agentName << "` from event `" << eventClassName << "("
                              << subscriptionElementAddr.Hash() << ")`.");

    delete it->second;
    subscriptionsMap.erase(subscriptionElementAddr);
  }
}

template <class TScAgent>
std::function<void(typename TScAgent::TEventType const &)> ScAgentManager<TScAgent>::GetCallback(
    ScAddr const & agentImplementationAddr) noexcept
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

  return [agentImplementationAddr](TScEvent const & event) -> void
  {
    auto const & ResolveAction = [](TScEvent const & event, ScAgent<TScEvent, TScContext> & agent) -> ScAction
    {
      auto [subscriptionElementAddr, _, otherElementAddr] = event.GetTriple();
      if (subscriptionElementAddr == ScKeynodes::action_initiated)
      {
        ScAddr const & actionAddr = otherElementAddr;
        return agent.m_context.ConvertToAction(actionAddr);
      }

      return agent.m_context.CreateAction(agent.GetActionClass()).Initiate();
    };

    TScAgent agent;
    std::string const & agentName = agent.GetName();
    // SC_LOG_INFO("Agent `" << agentName << "` reacted to primary initiation condition.");

    agent.SetInitiator(event.GetUser());
    agent.SetImplementation(agentImplementationAddr);

    if (agent.IsActionClassDeactivated())
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was finished because actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return;
    }

    // SC_LOG_INFO("Agent `" << agentName << "` started checking initiation condition.");
    if (!agent.template ValidateInitiationCondition<TScAgent, HasOverride<TScAgent>>(event))
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was finished because its initiation condition was checked unsuccessfully.");
      return;
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
      action.FinishWithError();
      SC_LOG_ERROR(
          "Agent `" << agentName << "` was finished because error was occurred.\nError description:\n"
                    << exception.Description());
      return;
    }

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agentName << "` finished performing action successfully.");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agentName << "` finished performing action unsuccessfully.");
    else
      SC_LOG_INFO("Agent `" << agentName << "` finished performing action with error.");

    // SC_LOG_INFO("Agent `" << agentName << "` started checking result condition.");
    if (!agent.template ValidateResultCondition<TScAgent, HasOverride<TScAgent>>(event, action))
    {
      SC_LOG_WARNING("Result condition of agent `" << agentName << "` checked unsuccessfully.");
      return;
    }
    // SC_LOG_INFO("Agent `" << agentName << "` finished checking result condition.");
  };
}
