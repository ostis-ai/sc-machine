/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_agent.hpp"

#include "sc_keynodes.hpp"

template <ScEventClass TScEvent>
ScAgentAbstract<TScEvent>::ScAgentAbstract()
  : m_memoryCtx(nullptr)
{
}

template <ScEventClass TScEvent>
ScAgentAbstract<TScEvent>::~ScAgentAbstract()
{
  m_memoryCtx.Destroy();
}

template <ScEventClass TScEvent>
void ScAgentAbstract<TScEvent>::SetContext(ScAddr const & userAddr)
{
  m_memoryCtx = ScAgentContext(userAddr);
}

template <ScEventClass TScEvent>
std::function<sc_result(TScEvent const &)> ScAgentAbstract<TScEvent>::GetCallback()
{
  return {};
}

template <ScEventClass TScEvent>
sc_result ScAgentAbstract<TScEvent>::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

template <ScEventClass TScEvent>
sc_result ScAgentAbstract<TScEvent>::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

template <ScEventClass TScEvent>
ScAgent<TScEvent>::ScAgent()
  : ScAgentAbstract<TScEvent>(){};

template <ScEventClass TScEvent>
template <ScAgentClass<TScEvent> TScAgent, ScAddrClass... TScAddr>
void ScAgent<TScEvent>::Register(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  SC_LOG_INFO("Register " << ScAgent::template GetName<TScAgent>());

  for (ScAddr const & subscriptionAddr : ScAddrVector{subscriptionAddrs...})
    ScAgentAbstract<TScEvent>::m_events.insert(
        {subscriptionAddr,
         new ScElementaryEventSubscription<TScEvent>(
             *ctx, subscriptionAddr, TScAgent::template GetCallback<TScAgent>())});
}

template <ScEventClass TScEvent>
template <ScAgentClass<TScEvent> TScAgent, ScAddrClass... TScAddr>
void ScAgent<TScEvent>::Unregister(ScMemoryContext *, TScAddr const &... subscriptionAddrs)
{
  SC_LOG_INFO("Unregister " << ScAgent::template GetName<TScAgent>());

  for (ScAddr const & subscriptionAddr : ScAddrVector{subscriptionAddrs...})
  {
    auto const & it = ScAgentAbstract<TScEvent>::m_events.find(subscriptionAddr);
    if (it != ScAgentAbstract<TScEvent>::m_events.cend())
      delete it->second;
  }
}

template <ScEventClass TScEvent>
template <ScAgentClass<TScEvent> TScAgent>
std::function<sc_result(TScEvent const &)> ScAgent<TScEvent>::GetCallback()
{
  return [](TScEvent const & event) -> sc_result
  {
    TScAgent agent;
    agent.SetContext(event.GetUser());

    SC_LOG_INFO(agent.GetName() << " started");
    sc_result const result = agent.OnEvent(event);

    // finish agent
    if (result == SC_RESULT_OK)
    {
      SC_LOG_INFO(agent.GetName() << " finished successfully");
      agent.OnSuccess(event);
    }
    else if (result == SC_RESULT_NO)
    {
      SC_LOG_INFO(agent.GetName() << " finished unsuccessfully");
      agent.OnUnsuccess(event);
    }
    else
    {
      SC_LOG_INFO(agent.GetName() << " finished with error");
      agent.OnError(event, result);
    }

    return result;
  };
}

template <ScAgentClass<ScEventAddOutputEdge> TScAgent>
void ScActionAgent::Register(ScMemoryContext * ctx, ScAddr const & actionClassAddr)
{
  SC_LOG_INFO("Register " << ScAgent::template GetName<TScAgent>());

  ScActionAgent::m_events.insert(
      {actionClassAddr,
       new ScEventSubscriptionAddOutputEdge(
           *ctx, ScKeynodes::action_initiated, TScAgent::template GetCallback<TScAgent>(actionClassAddr))});
}

template <ScAgentClass<ScEventAddOutputEdge> TScAgent>
void ScActionAgent::Unregister(ScMemoryContext *, ScAddr const & actionClassAddr)
{
  SC_LOG_INFO("Unregister " << ScAgent::template GetName<TScAgent>());

  auto const & it = ScActionAgent::m_events.find(actionClassAddr);
  if (it != ScActionAgent::m_events.cend())
    delete it->second;
}

template <class TScAgent>
std::function<sc_result(ScEventAddOutputEdge const &)> ScActionAgent::GetCallback(ScAddr const & actionClassAddr)
{
  return [actionClassAddr](ScEventAddOutputEdge const & event) -> sc_result
  {
    ScAddr const & actionAddr = event.GetOtherElement();

    sc_result result = SC_RESULT_ERROR;
    if (!(event.GetAddedConnectorType().BitAnd(ScType::EdgeAccess) == ScType::EdgeAccess)
        || (!ScMemory::ms_globalContext->HelperCheckEdge(actionClassAddr, actionAddr, ScType::EdgeAccessConstPosPerm)))
      return result;

    TScAgent agent;
    agent.SetContext(event.GetUser());
    SC_LOG_INFO(agent.GetName() << " started");
    result = agent.OnEvent(event);

    // finish agent
    if (result == SC_RESULT_OK)
    {
      SC_LOG_INFO(agent.GetName() << " finished successfully");
      agent.OnSuccess(event);
      ScMemory::ms_globalContext->CreateEdge(
          ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_successfully, actionAddr);
    }
    else if (result == SC_RESULT_NO)
    {
      SC_LOG_INFO(agent.GetName() << " finished unsuccessfully");
      agent.OnUnsuccess(event);
      ScMemory::ms_globalContext->CreateEdge(
          ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_unsuccessfully, actionAddr);
    }
    else
    {
      SC_LOG_INFO(agent.GetName() << " finished with error");
      agent.OnError(event, result);
      ScMemory::ms_globalContext->CreateEdge(
          ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_with_error, actionAddr);
    }
    ScMemory::ms_globalContext->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, actionAddr);

    return result;
  };
}
