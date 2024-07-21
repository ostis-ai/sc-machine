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
void ScAgent<TScEvent>::Register(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  SC_LOG_INFO("Register " << ScAgent::template GetName<TScAgent>());

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  for (ScAddr const & subscriptionAddr : ScAddrVector{subscriptionAddrs...})
    ScAgentAbstract<TScEvent>::m_events.insert(
        {agentName,
         new ScElementaryEventSubscription<TScEvent>(
             *ctx, subscriptionAddr, TScAgent::template GetCallback<TScAgent>())});
}

template <class TScEvent>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent>::Unregister(ScMemoryContext *, TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  SC_LOG_INFO("Unregister " << ScAgent::template GetName<TScAgent>());

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  for (ScAddr const & _ : ScAddrVector{subscriptionAddrs...})
  {
    SC_UNUSED(_);

    auto const & it = ScAgentAbstract<TScEvent>::m_events.find(agentName);
    if (it != ScAgentAbstract<TScEvent>::m_events.cend())
    {
      delete it->second;
      it->second = nullptr;
    }
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

    // finish agent
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
void ScActionAgent::Register(ScMemoryContext * ctx)
{
  static_assert(
      std::is_base_of<ScActionAgent, TScAgent>::value, "TScAgent type must be derived from ScActionAgent type.");

  SC_LOG_INFO("Register " << TScAgent::template GetName<TScAgent>());

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  ScActionAgent::m_events.insert(
      {agentName,
       new ScElementaryEventSubscription<ScActionEvent>(
           *ctx, ScKeynodes::action_initiated, TScAgent::template GetCallback<TScAgent>())});
}

template <class TScAgent>
void ScActionAgent::Unregister(ScMemoryContext *)
{
  static_assert(
      std::is_base_of<ScActionAgent, TScAgent>::value, "TScAgent type must be derived from ScActionAgent type.");

  SC_LOG_INFO("Unregister " << TScAgent::template GetName<TScAgent>());

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  auto const & it = ScActionAgent::m_events.find(agentName);
  if (it != ScActionAgent::m_events.cend())
  {
    delete it->second;
    it->second = nullptr;
  }
}
