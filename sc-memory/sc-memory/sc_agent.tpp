/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_agent.hpp"

#include "sc_keynodes.hpp"

template <class TScEvent>
ScAgentAbstract<TScEvent>::ScAgentAbstract()
  : m_memoryCtx(nullptr)
{
}

template <class TScEvent>
ScAgentAbstract<TScEvent>::~ScAgentAbstract()
{
  m_memoryCtx.Destroy();
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::CheckInitiationCondition(TScEvent const &)
{
  return SC_TRUE;
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::CheckResult(TScEvent const &, ScAction &)
{
  return SC_TRUE;
}

template <class TScEvent>
void ScAgentAbstract<TScEvent>::SetContext(ScAddr const & userAddr)
{
  m_memoryCtx = ScAgentContext(userAddr);
}

template <class TScEvent>
std::function<sc_result(TScEvent const &)> ScAgentAbstract<TScEvent>::GetCallback()
{
  return {};
}

template <class TScEvent>
sc_result ScAgentAbstract<TScEvent>::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

template <class TScEvent>
sc_result ScAgentAbstract<TScEvent>::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

template <class TScEvent>
ScAgent<TScEvent>::ScAgent()
  : ScAgentAbstract<TScEvent>(){};

template <class TScEvent>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent>::Subscribe(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  if (!ScAgentAbstract<TScEvent>::m_events.count(agentName))
    ScAgentAbstract<TScEvent>::m_events.insert({agentName, {}});

  std::string const & eventName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = ScAgentAbstract<TScEvent>::m_events.find(agentName)->second;
  for (ScAddr const & subscriptionAddr : {subscriptionAddrs...})
  {
    if (!ctx->IsElement(subscriptionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to subscribe `" << agentName << "` to event `" << eventName
                                    << "due subscription sc-element with address `" << subscriptionAddr.Hash()
                                    << "` is not valid.");
    if (subscriptionsMap.find(subscriptionAddr) != subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "`" << agentName << "` has already been subscribed to event `" << eventName << "(" << subscriptionAddr.Hash()
              << ")`.");

    SC_LOG_INFO("Subscribe `" << agentName << "` to event `" << eventName << "(" << subscriptionAddr.Hash() << ")`.");

    subscriptionsMap.insert(
        {subscriptionAddr,
         new ScElementaryEventSubscription<TScEvent>(
             *ctx, subscriptionAddr, TScAgent::template GetCallback<TScAgent>())});
  }
}

template <class TScEvent>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent>::Unsubscribe(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  auto const & agentsMapIt = ScAgentAbstract<TScEvent>::m_events.find(agentName);
  if (agentsMapIt == ScAgentAbstract<TScEvent>::m_events.cend())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, agentName << " has not been subscribed to any events yet.");

  std::string const & eventName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = agentsMapIt->second;
  for (ScAddr const & subscriptionAddr : {subscriptionAddrs...})
  {
    if constexpr (!std::is_same<TScEvent, ScEventRemoveElement>::value)
    {
      if (!ctx->IsElement(subscriptionAddr))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Not able to unsubscribe `" << agentName << "` from event `" << eventName
                                        << "due subscription sc-element with address `" << subscriptionAddr.Hash()
                                        << "` is not valid.");
    }

    auto const & it = subscriptionsMap.find(subscriptionAddr);
    if (it == subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "`" << agentName << "` has not been subscribed to event `" << eventName << "(" << subscriptionAddr.Hash()
              << ")` yet.");

    SC_LOG_INFO(
        "Unsubscribe `" << agentName << "` from event `" << eventName << "(" << subscriptionAddr.Hash() << ")`.");

    delete it->second;
    subscriptionsMap.erase(subscriptionAddr);
  }
}

template <class TScEvent>
template <class TScAgent>
std::function<sc_result(TScEvent const &)> ScAgent<TScEvent>::GetCallback()
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  return [](TScEvent const & event) -> sc_result
  {
    auto const & CreateAction = [](TScEvent const & event, ScAgent & agent) -> ScAction
    {
      if constexpr (std::is_base_of<class ScActionAgent, TScAgent>::value)
        return ScAction(&agent.m_memoryCtx, event.GetOtherElement(), agent.GetActionClass());

      return ScAction(&agent.m_memoryCtx, agent.GetActionClass());
      ;
    };

    TScAgent agent;
    agent.SetContext(event.GetUser());

    ScAction action = CreateAction(event, agent);
    if (!agent.CheckInitiationCondition(event))
      return SC_RESULT_OK;

    SC_LOG_INFO(agent.GetName() << " started");
    sc_result result = agent.DoProgram(event, action);

    if (result == SC_RESULT_OK)
      SC_LOG_INFO(agent.GetName() << " finished successfully");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO(agent.GetName() << " finished unsuccessfully");
    else
      SC_LOG_INFO(agent.GetName() << " finished with error");

    if (!agent.CheckResult(event, action))
      return SC_RESULT_OK;

    return result;
  };
}

template <class TScAgent>
void ScActionAgent::Subscribe(ScMemoryContext * ctx)
{
  ScAgent<ScActionEvent>::Subscribe<TScAgent>(ctx, ScKeynodes::action_initiated);
}

template <class TScAgent>
void ScActionAgent::Unsubscribe(ScMemoryContext * ctx)
{
  ScAgent<ScActionEvent>::Unsubscribe<TScAgent>(ctx, ScKeynodes::action_initiated);
}