/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_agent.hpp"

#include "sc_action.hpp"
#include "sc_agent_context.hpp"
#include "sc_result.hpp"
#include "sc_event_subscription.hpp"
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
ScAddr ScAgentAbstract<TScEvent>::GetAbstractAgent() const
{
  return ScAddr::Empty;
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetSubscriptionElement() const
{
  return ScAddr::Empty;
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::CheckInitiationCondition(TScEvent const & event)
{
  return SC_TRUE;
}

template <class TScEvent>
ScTemplate ScAgentAbstract<TScEvent>::GetInitiationCondition() const
{
  return ScTemplate();
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::CheckResultCondition(TScEvent const &, ScAction &)
{
  return SC_TRUE;
}

template <class TScEvent>
ScTemplate ScAgentAbstract<TScEvent>::GetResultCondition() const
{
  return ScTemplate();
}

template <class TScEvent>
void ScAgentAbstract<TScEvent>::SetContext(ScAddr const & userAddr)
{
  m_memoryCtx = ScAgentContext(userAddr);
}

template <class TScEvent>
std::function<void(TScEvent const &)> ScAgentAbstract<TScEvent>::GetCallback()
{
  return {};
}

template <class TScEvent>
ScAgent<TScEvent>::ScAgent()
  : ScAgentAbstract<TScEvent>(){};

template <class TScEvent>
template <class TScAgent>
void ScAgent<TScEvent>::Initialize(ScMemoryContext * ctx)
{
  TScAgent agent;
  std::string const & agentImplementationName = agent.GetName();

  ScAddr const & abstractAgentAddr = agent.GetAbstractAgent();
  if (!abstractAgentAddr.IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation for class `" << agentImplementationName << "` has not specified abstract agent.");

  ScIterator3Ptr it3 = ctx->Iterator3(ScKeynodes::abstract_sc_agent, ScType::EdgeAccessConstPosPerm, abstractAgentAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Specified sc-element for class `"
            << agentImplementationName
            << "` is not abstract agent due it does not belong to class `abstract_sc_agent`.");

  ScAddr const & subscriptionElementAddr = agent.GetSubscriptionElement();
  if (subscriptionElementAddr.IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation for class `" << agentImplementationName
                                           << "` has not specified subscription sc-element.");

  ScAddr const & actionClassAddr = agent.GetActionClass();
  if (!actionClassAddr.IsValid())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation for class `" << agentImplementationName << "` has not specified action class.");

  ScTemplate && initiationConditionTemplate = agent.GetInitiationCondition();
  if (!initiationConditionTemplate.IsEmpty())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation for class `" << agentImplementationName
                                           << "` has not specified initiation condition template.");

  ScTemplate && resultConditionTemplate = agent.GetResultCondition();
  if (!resultConditionTemplate.IsEmpty())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation for class `" << agentImplementationName
                                           << "` has not specified result condition template.");

  ScAddr agentImplementationAddr = ctx->HelperFindBySystemIdtf(agentImplementationName);
  if (agentImplementationAddr.IsValid())
    SC_LOG_WARNING(
        "Agent implementation for class `" << agentImplementationName << "` was not generated due it already exists.");
  else
  {
    agentImplementationAddr = ctx->CreateNode(ScType::NodeConst);
    ctx->HelperSetSystemIdtf(agentImplementationName, agentImplementationAddr);
    SC_LOG_WARNING("Agent implementation for class `" << agentImplementationName << "` was generated.");
  }

  ScIterator5Ptr it5 = ctx->Iterator5(
      abstractAgentAddr,
      ScType::EdgeDCommonConst,
      agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_inclusion);
  if (it5->Next())
    SC_LOG_WARNING(
        "Connection between specified abstract agent and agent implementation for class `"
        << agentImplementationName << "` was not generated due it already exists.");
  else
  {
    ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, abstractAgentAddr, agentImplementationAddr);
    ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
    SC_LOG_WARNING(
        "Connection between specified abstract agent and agent implementation for class `" << agentImplementationName
                                                                                           << "` was generated.");
  }

  it5 = ctx->Iterator5(
      abstractAgentAddr,
      ScType::EdgeDCommonConst,
      actionClassAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_action_class);
  if (it5->Next())
    SC_LOG_WARNING(
        "Connection between specified abstract agent and action class for agent class `"
        << agentImplementationName << "` was not generated due it already exists.");
  else
  {
    ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, abstractAgentAddr, actionClassAddr);
    ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_sc_agent_action_class, arcAddr);
    SC_LOG_WARNING(
        "Connection between specified abstract agent and action class for agent class `" << agentImplementationName
                                                                                         << "` was generated.");
  }

  it5 = ctx->Iterator5(
      abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (it5->Next())
    SC_LOG_WARNING(
        "Primary initiation condition for class `" << agentImplementationName
                                                   << "` was not generated due it already exists.");
  else
  {
    ScAddr const & primaryConditionAddr =
        ctx->CreateEdge(ScType::EdgeDCommonConst, TScEvent::eventClassAddr, subscriptionElementAddr);
    ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, abstractAgentAddr, primaryConditionAddr);
    ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_primary_initiation_condition, arcAddr);
    SC_LOG_WARNING("Primary initiation condition for class  `" << agentImplementationName << "` was generated.");
  }

  it5 = ctx->Iterator5(
      abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (it5->Next())
    SC_LOG_WARNING(
        "Initiation condition and result for class `" << agentImplementationName
                                                      << "` was not generated due it already exists.");
  else
  {
    ScAddr initiationConditionAddr;
    ctx->HelperLoadTemplate(initiationConditionTemplate, initiationConditionAddr);

    ScAddr resultConditionTemplate;
    ctx->HelperLoadTemplate(initiationConditionTemplate, initiationConditionAddr);

    ScAddr const & conditionAndResultAddr =
        ctx->CreateEdge(ScType::EdgeDCommonConst, initiationConditionAddr, resultConditionTemplate);
    ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, abstractAgentAddr, conditionAndResultAddr);
    ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_initiation_condition_and_result, arcAddr);
    SC_LOG_WARNING("Initiation condition and result for class  `" << agentImplementationName << "` was generated.");
  }
}

template <class TScEvent>
template <class TScAgent>
void ScAgent<TScEvent>::Shutdown(ScMemoryContext * ctx)
{
}

template <class TScEvent>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent>::Subscribe(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  ScAgent<TScAgent>::Initialize<TScAgent>(ctx);

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
          "Not able to subscribe agent `" << agentName << "` to event `" << eventName
                                          << "due subscription sc-element with address `" << subscriptionAddr.Hash()
                                          << "` is not valid.");
    if (subscriptionsMap.find(subscriptionAddr) != subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` has already been subscribed to event `" << eventName << "("
                    << subscriptionAddr.Hash() << ")`.");

    SC_LOG_INFO(
        "Subscribe agent `" << agentName << "` to event `" << eventName << "(" << subscriptionAddr.Hash() << ")`.");

    subscriptionsMap.insert(
        {subscriptionAddr,
         new TScElementaryEventSubscription<TScEvent>(
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

  ScAgent<TScAgent>::Shutdown<TScAgent>(ctx);

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  auto const & agentsMapIt = ScAgentAbstract<TScEvent>::m_events.find(agentName);
  if (agentsMapIt == ScAgentAbstract<TScEvent>::m_events.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Agent `" << agentName << "` has not been subscribed to any events yet.");

  std::string const & eventClassName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = agentsMapIt->second;
  for (ScAddr const & subscriptionAddr : {subscriptionAddrs...})
  {
    if constexpr (!std::is_same<TScEvent, ScEventRemoveElement>::value)
    {
      if (!ctx->IsElement(subscriptionAddr))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Not able to unsubscribe agent `" << agentName << "` from event `" << eventClassName
                                              << "due subscription sc-element with address `" << subscriptionAddr.Hash()
                                              << "` is not valid.");
    }

    auto const & it = subscriptionsMap.find(subscriptionAddr);
    if (it == subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` has not been subscribed to event `" << eventClassName << "("
                    << subscriptionAddr.Hash() << ")` yet.");

    SC_LOG_INFO(
        "Unsubscribe agent `" << agentName << "` from event `" << eventClassName << "(" << subscriptionAddr.Hash()
                              << ")`.");

    delete it->second;
    subscriptionsMap.erase(subscriptionAddr);
  }
}

template <class TScEvent>
template <class TScAgent>
std::function<void(TScEvent const &)> ScAgent<TScEvent>::GetCallback()
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  return [](TScEvent const & event) -> void
  {
    auto const & CreateAction = [](TScEvent const & event, ScAgent & agent) -> ScAction
    {
      if constexpr (std::is_base_of<class ScActionAgent, TScAgent>::value)
        return ScAction(&agent.m_memoryCtx, event.GetArcTargetElement(), agent.GetActionClass());

      return ScAction(&agent.m_memoryCtx, agent.GetActionClass()).Initiate();
    };

    TScAgent agent;
    SC_LOG_INFO("Agent `" << agent.GetName() << "` started");

    agent.SetContext(event.GetUser());
    if (ScMemory::ms_globalContext->HelperCheckEdge(
            ScKeynodes::action_deactivated, agent.GetActionClass(), ScType::EdgeAccessConstPosPerm))
    {
      SC_LOG_WARNING(
          "Agent `" << agent.GetName() << "` was not started due actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return;
    }

    ScAction action = CreateAction(event, agent);

    ScTemplate && initiationConditionTemplate = agent.GetInitiationCondition();
    ScTemplateSearchResult searchResult;
    if (!initiationConditionTemplate.IsEmpty()
        && !agent.m_memoryCtx.HelperSearchTemplate(initiationConditionTemplate, searchResult))
    {
      SC_LOG_WARNING(
          "Agent `" << agent.GetName() << "` was finished due its initiation condition was tested unsuccessfully.");
      return;
    }

    ScResult result = agent.DoProgram(event, action);

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished successfully");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished unsuccessfully");
    else
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished with error");

    ScTemplate && resultConditionTemplate = agent.GetResultCondition();
    if (!resultConditionTemplate.IsEmpty()
        && !agent.m_memoryCtx.HelperSearchTemplate(resultConditionTemplate, searchResult))
    {
      SC_LOG_WARNING("Result condition of agent `" << agent.GetName() << "` tested unsuccessfully.");
      return;
    }

    return;
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

template <ScKeynode const & agentImplementationAddr>
ScSpecificatedAgent<agentImplementationAddr>::ScSpecificatedAgent()
  : ScAgent<ScElementaryEvent>(){};

template <ScKeynode const & agentImplementationAddr>
template <class TScAgent>
void ScSpecificatedAgent<agentImplementationAddr>::Initialize(ScMemoryContext * ctx)
{
  std::string const & agentClassName = TScAgent::template GetName<TScAgent>();
  std::string const & agentImplementationName = ctx->HelperGetSystemIdtf(agentImplementationAddr);

  ScIterator3Ptr it3 = ctx->Iterator3(
      ScKeynodes::platform_dependent_abstract_sc_agent, ScType::EdgeAccessConstPosPerm, agentImplementationAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Specified sc-element with system identifier `"
            << agentImplementationName << "` is not agent implementation for agent class `" << agentClassName
            << "` because it does not belong to class `platform_dependent_abstract_sc_agent`.");

  if (it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation with system identifier `"
            << agentImplementationName << "` for agent class `" << agentClassName
            << "` belongs to class `platform_dependent_abstract_sc_agent` twice.");

  ScIterator5Ptr it5 = ctx->Iterator5(
      ScType::Unknown,
      ScType::EdgeDCommonConst,
      agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_program);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName
                                 << "` has not sc-agent program. Check that agent implementation `"
                                 << agentImplementationName << " has specified relation `nrel_sc_agent_program`.");

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName << "` has two or more sc-agent programs.");

  it5 = ctx->Iterator5(
      ScType::Unknown,
      ScType::EdgeDCommonConst,
      agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_inclusion);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName
                                 << "` is not included to any abstract sc-agent. Check that agent implementation has "
                                    "specified relation `nrel_inclusion`.");

  ms_abstractAgentAddr = it5->Get(0);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName << "` is included to two or more abstract sc-agents.");

  if (ctx->GetElementType(ms_abstractAgentAddr).BitAnd(ScType::NodeConst) != ScType::NodeConst)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
            << agentImplementationName << "` because it has not sc-type `ScType::NodeConst`.");

  it3 = ctx->Iterator3(ScKeynodes::abstract_sc_agent, ScType::EdgeAccessConstPosPerm, ms_abstractAgentAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
            << agentImplementationName << "` because it does not belong to class `abstract_sc_agent`.");

  if (it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent or agent implementation `" << agentImplementationName
                                                      << "` belongs to clas `abstract_sc_agent` twice.");

  std::string abstractAgentName = ctx->HelperGetSystemIdtf(ms_abstractAgentAddr);
  if (abstractAgentName.empty())
    abstractAgentName = std::to_string(ms_abstractAgentAddr.Hash());

  it5 = ctx->Iterator5(
      ms_abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName
                              << "` has not primary initiation condition. Check that abstract sc-agent has specified "
                                 "relation `nrel_primary_initiation_condition`.");

  ScAddr const & primaryInitiationConditionAddr = it5->Get(2);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more primary initiation conditions.");

  if (ctx->GetElementType(primaryInitiationConditionAddr).BitAnd(ScType::EdgeDCommonConst) != ScType::EdgeDCommonConst)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_primary_initiation_condition` is not primary initiation condition for "
        "abstract sc-agent `"
            << abstractAgentName << "` beacuse it has not sc-type `ScType::EdgeDCommonConst`.");

  ctx->GetEdgeInfo(primaryInitiationConditionAddr, ms_eventClassAddr, ms_eventSubscriptionAddr);

  it3 = ctx->Iterator3(ScKeynodes::sc_event, ScType::EdgeAccessConstPosPerm, ms_eventClassAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "The first element of primary initiation condition for abstract sc-agent `"
            << abstractAgentName << "` is not sc-event class because it doesn't belong to class `sc_event`.");

  if (it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-event class for abstract sc-agent `" << abstractAgentName << "` belongs to class `sc_event` twice.");

  it5 = ctx->Iterator5(
      ms_abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName
                              << "` has not action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  ms_actionClassAddr = it5->Get(2);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more action classes.");

  if (ctx->GetElementType(ms_actionClassAddr).BitAnd(ScType::NodeConstClass) != ScType::NodeConstClass)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_sc_agent_action_class` is not action class for "
        "abstract sc-agent `"
            << abstractAgentName << "` beacuse it has not sc-type `ScType::NodeConstClass`.");

  it5 = ctx->Iterator5(
      ms_abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << abstractAgentName
            << "` has not initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  ScAddr const & initiationConditionAndResultAddr = it5->Get(2);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more initiation conditions and results.");

  if (ctx->GetElementType(initiationConditionAndResultAddr).BitAnd(ScType::EdgeDCommonConst)
      != ScType::EdgeDCommonConst)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_initiation_condition_and_result` is initiation condition and result for "
        "abstract sc-agent `"
            << abstractAgentName << "` beacuse it has not sc-type `ScType::EdgeDCommonConst`.");

  ctx->GetEdgeInfo(initiationConditionAndResultAddr, ms_initiationConditionAddr, ms_resultConditionAddr);
  if (ctx->GetElementType(ms_initiationConditionAddr).BitAnd(ScType::NodeConstStruct) != ScType::NodeConstStruct)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Initiation condition for "
        "abstract sc-agent `"
            << abstractAgentName << "` is not sc-template because it has not sc-type `ScType::NodeConstStruct`.");

  if (ctx->GetElementType(ms_resultConditionAddr).BitAnd(ScType::NodeConstStruct) != ScType::NodeConstStruct)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Result condition for "
        "abstract sc-agent `"
            << abstractAgentName << "` is not sc-template because it has not sc-type `ScType::NodeConstStruct`.");
}

template <ScKeynode const & agentImplementationAddr>
template <class TScAgent>
void ScSpecificatedAgent<agentImplementationAddr>::Shutdown(ScMemoryContext *)
{
}

template <ScKeynode const & agentImplementationAddr>
template <class TScAgent>
void ScSpecificatedAgent<agentImplementationAddr>::Subscribe(ScMemoryContext * ctx)
{
  static_assert(
      std::is_base_of<ScSpecificatedAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  ScSpecificatedAgent<agentImplementationAddr>::Initialize<TScAgent>(ctx);

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  if (!ScAgentAbstract<ScElementaryEvent>::m_events.count(agentName))
    ScAgentAbstract<ScElementaryEvent>::m_events.insert({agentName, {}});

  std::string const & eventClassName = ctx->HelperGetSystemIdtf(ms_eventClassAddr);
  auto & subscriptionsMap = ScAgentAbstract<ScElementaryEvent>::m_events.find(agentName)->second;
  if (subscriptionsMap.find(ms_eventSubscriptionAddr) != subscriptionsMap.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent `" << agentName << "` has already been subscribed to event `" << eventClassName << "("
                  << ms_eventSubscriptionAddr.Hash() << ")`.");

  SC_LOG_INFO(
      "Subscribe agent `" << agentName << "` to event `" << eventClassName << "(" << ms_eventSubscriptionAddr.Hash()
                          << ")`.");

  subscriptionsMap.insert(
      {ms_eventSubscriptionAddr,
       new ScElementaryEventSubscription(
           *ctx, ms_eventClassAddr, ms_eventSubscriptionAddr, TScAgent::template GetCallback<TScAgent>())});
}

template <ScKeynode const & agentImplementationAddr>
template <class TScAgent>
void ScSpecificatedAgent<agentImplementationAddr>::Unsubscribe(ScMemoryContext * ctx)
{
  static_assert(
      std::is_base_of<ScSpecificatedAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  ScSpecificatedAgent<agentImplementationAddr>::Shutdown<TScAgent>(ctx);

  std::string const & agentName = TScAgent::template GetName<TScAgent>();
  auto const & agentsMapIt = ScAgentAbstract<ScElementaryEvent>::m_events.find(agentName);
  if (agentsMapIt == ScAgentAbstract<ScElementaryEvent>::m_events.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Agent `" << agentName << "` has not been subscribed to any events yet.");

  std::string const & eventClassName = ctx->HelperGetSystemIdtf(ms_eventClassAddr);
  auto & subscriptionsMap = agentsMapIt->second;
  auto const & it = subscriptionsMap.find(ms_eventSubscriptionAddr);
  if (it == subscriptionsMap.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent `" << agentName << "` has not been subscribed to event `" << eventClassName << "("
                  << ms_eventSubscriptionAddr.Hash() << ")` yet.");

  SC_LOG_INFO(
      "Unsubscribe agent `" << agentName << "` from event `" << eventClassName << "(" << ms_eventSubscriptionAddr.Hash()
                            << ")`.");

  delete it->second;
  subscriptionsMap.erase(ms_eventSubscriptionAddr);
}

template <ScKeynode const & agentImplementationAddr>
std::tuple<ScAddr, ScAddr> ScSpecificatedAgent<agentImplementationAddr>::GetPrimaryInitiationCondition()
{
  return {ms_eventClassAddr, ms_eventSubscriptionAddr};
}

template <ScKeynode const & agentImplementationAddr>
ScAddr ScSpecificatedAgent<agentImplementationAddr>::GetActionClass() const
{
  return ms_actionClassAddr;
}

//! This method is private in ScSpecificatedAgent class. You can not use it.
// LCOV_EXCL_START
template <ScKeynode const & agentImplementationAddr>
ScTemplate ScSpecificatedAgent<agentImplementationAddr>::GetInitiationCondition() const
{
  return ScTemplate();
}

// LCOV_EXCL_END

//! This method is private in ScSpecificatedAgent class. You can not use it.
// LCOV_EXCL_START
template <ScKeynode const & agentImplementationAddr>
ScTemplate ScSpecificatedAgent<agentImplementationAddr>::GetResultCondition() const
{
  return ScTemplate();
}

// LCOV_EXCL_END

template <ScKeynode const & agentImplementationAddr>
template <class TScAgent>
std::function<void(ScElementaryEvent const &)> ScSpecificatedAgent<agentImplementationAddr>::GetCallback()
{
  static_assert(
      std::is_base_of<ScSpecificatedAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  return [](ScElementaryEvent const & event) -> void
  {
    auto const & CreateAction = [](ScElementaryEvent const & event, ScSpecificatedAgent & agent) -> ScAction
    {
      if (TScAgent::ms_eventSubscriptionAddr == ScKeynodes::action_initiated)
      {
        auto [_1, _2, actionAddr] = event.GetTriple();
        return ScAction(&agent.m_memoryCtx, actionAddr, agent.GetActionClass());
      }

      return ScAction(&agent.m_memoryCtx, agent.GetActionClass()).Initiate();
    };

    TScAgent agent;
    SC_LOG_INFO("Agent `" << agent.GetName() << "` started");

    agent.SetContext(event.GetUser());
    if (ScMemory::ms_globalContext->HelperCheckEdge(
            ScKeynodes::action_deactivated, agent.GetActionClass(), ScType::EdgeAccessConstPosPerm))
    {
      SC_LOG_WARNING(
          "Agent `" << agent.GetName() << "` was not started due actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return;
    }

    ScAction action = CreateAction(event, agent);

    ScTemplate initiationConditionTemplate;
    agent.m_memoryCtx.HelperBuildTemplate(initiationConditionTemplate, TScAgent::ms_initiationConditionAddr);
    ScTemplateSearchResult searchResult;
    if (!initiationConditionTemplate.IsEmpty()
        && !agent.m_memoryCtx.HelperSearchTemplate(initiationConditionTemplate, searchResult))
    {
      SC_LOG_WARNING(
          "Agent `" << agent.GetName() << "` was finished due its initiation condition was tested unsuccessfully.");
      return;
    }

    ScResult result = agent.DoProgram(event, action);

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished successfully");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished unsuccessfully");
    else
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished with error");

    ScTemplate resultConditionTemplate;
    agent.m_memoryCtx.HelperBuildTemplate(resultConditionTemplate, TScAgent::ms_resultConditionAddr);
    if (!resultConditionTemplate.IsEmpty()
        && !agent.m_memoryCtx.HelperSearchTemplate(resultConditionTemplate, searchResult))
    {
      SC_LOG_WARNING("Result condition of agent `" << agent.GetName() << "` tested unsuccessfully.");
      return;
    }

    return;
  };
}
