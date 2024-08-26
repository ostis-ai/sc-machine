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

template <class TScEvent, class TScContext>
ScAgentAbstract<TScEvent, TScContext>::ScAgentAbstract()
  : m_memoryCtx(nullptr)
{
}

template <class TScEvent, class TScContext>
ScAgentAbstract<TScEvent, TScContext>::~ScAgentAbstract()
{
  m_memoryCtx.Destroy();
}

template <class TScEvent, class TScContext>
ScAddr ScAgentAbstract<TScEvent, TScContext>::GetAbstractAgent() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      ScType::NodeConst,
      ScType::EdgeDCommonConst,
      m_agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_inclusion);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << this->GetName()
                                 << "` is not included to any abstract sc-agent. Check that agent implementation has "
                                    "specified relation `nrel_inclusion`.");

  return it5->Get(0);
}

template <class TScEvent, class TScContext>
ScAddr ScAgentAbstract<TScEvent, TScContext>::GetEventClass() const
{
  if constexpr (!std::is_same<TScEvent, ScElementaryEvent>::value)
    return TScEvent::eventClassAddr;

  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have primary initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_memoryCtx.GetEdgeSource(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScAddr ScAgentAbstract<TScEvent, TScContext>::GetEventSubscriptionElement() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have primary initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_memoryCtx.GetEdgeTarget(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScAddr ScAgentAbstract<TScEvent, TScContext>::GetActionClass() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::NodeConstClass,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << this->GetName()
                              << "` does not have action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  return it5->Get(2);
}

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
sc_bool ScAgentAbstract<TScEvent, TScContext>::CheckInitiationCondition(TScEvent const & event)
{
  return SC_TRUE;
}

// LCOV_EXCL_STOP

template <class TScEvent, class TScContext>
ScAddr ScAgentAbstract<TScEvent, TScContext>::GetInitiationCondition() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_memoryCtx.GetEdgeSource(it5->Get(2));
}

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
ScTemplate ScAgentAbstract<TScEvent, TScContext>::GetInitiationConditionTemplate() const
{
  return ScTemplate();
}

// LCOV_EXCL_STOP

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
ScResult ScAgentAbstract<TScEvent, TScContext>::DoProgram(TScEvent const & event, ScAction & action)
{
  return ScResult(SC_RESULT_OK);
}

template <class TScEvent, class TScContext>
ScResult ScAgentAbstract<TScEvent, TScContext>::DoProgram(ScAction & action)
{
  return ScResult(SC_RESULT_OK);
}

// LCOV_EXCL_STOP

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
sc_bool ScAgentAbstract<TScEvent, TScContext>::CheckResultCondition(TScEvent const &, ScAction &)
{
  return SC_TRUE;
}

// LCOV_EXCL_STOP

template <class TScEvent, class TScContext>
ScAddr ScAgentAbstract<TScEvent, TScContext>::GetResultCondition() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_memoryCtx.GetEdgeTarget(it5->Get(2));
}

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
ScTemplate ScAgentAbstract<TScEvent, TScContext>::GetResultConditionTemplate() const
{
  return ScTemplate();
}

// LCOV_EXCL_STOP

template <class TScEvent, class TScContext>
void ScAgentAbstract<TScEvent, TScContext>::SetInitiator(ScAddr const & userAddr)
{
  m_memoryCtx = ScAgentContext(userAddr);
}

template <class TScEvent, class TScContext>
void ScAgentAbstract<TScEvent, TScContext>::SetImplementation(ScAddr const & agentImplementationAddr)
{
  m_agentImplementationAddr = agentImplementationAddr;
}

template <class TScEvent, class TScContext>
sc_bool ScAgentAbstract<TScEvent, TScContext>::MayBeSpecified() const
{
  return m_agentImplementationAddr.IsValid();
}

template <class TScEvent, class TScContext>
std::function<void(TScEvent const &)> ScAgentAbstract<TScEvent, TScContext>::GetCallback(ScAddr const &)
{
  return {};
}

template <class TScEvent, class TScContext>
ScAgent<TScEvent, TScContext>::ScAgent()
  : ScAgentAbstract<TScEvent>(){};

template <class TScEvent, class TScContext>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent, TScContext>::Subscribe(
    ScMemoryContext * ctx,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetInitiator(ctx->GetUser());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentName = agent.GetName();
  if (!ScAgentAbstract<TScEvent>::m_events.count(agentName))
    ScAgentAbstract<TScEvent>::m_events.insert({agentName, {}});

  ScAddrVector subscriptionVector{subscriptionAddrs...};
  // Check that user specify agent implementation only and find subscription sc-element in knowledge base.
  if (agent.MayBeSpecified() && subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = ScAgentAbstract<TScEvent>::m_events.find(agentName)->second;
  for (ScAddr const & subscriptionElementAddr : subscriptionVector)
  {
    if (!ctx->IsElement(subscriptionElementAddr))
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
               *ctx,
               agent.GetEventClass(),
               subscriptionElementAddr,
               TScAgent::template GetCallback<TScAgent>(agentImplementationAddr))});
    }
    else
    {
      subscriptionsMap.insert(
          {subscriptionElementAddr,
           new ScElementaryEventSubscription<TScEvent>(
               *ctx, subscriptionElementAddr, TScAgent::template GetCallback<TScAgent>(agentImplementationAddr))});
    }
  }
}

template <class TScEvent, class TScContext>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent, TScContext>::Unsubscribe(
    ScMemoryContext * ctx,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetInitiator(ctx->GetUser());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentName = agent.GetName();
  auto const & agentsMapIt = ScAgentAbstract<TScEvent>::m_events.find(agentName);
  if (agentsMapIt == ScAgentAbstract<TScEvent>::m_events.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Agent `" << agentName << "` does not have been subscribed to any events yet.");

  ScAddrVector subscriptionVector{subscriptionAddrs...};
  // Check that user specify agent implementation only and find subscription sc-element in knowledge base.
  if (agent.MayBeSpecified() && subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventClassName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = agentsMapIt->second;
  for (ScAddr const & subscriptionElementAddr : subscriptionVector)
  {
    if constexpr (!std::is_same<TScEvent, ScEventEraseElement>::value)
    {
      if (!ctx->IsElement(subscriptionElementAddr))
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

template <class TScEvent, class TScContext>
template <class TScAgent>
std::function<void(TScEvent const &)> ScAgent<TScEvent, TScContext>::GetCallback(ScAddr const & agentImplementationAddr)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  static_assert(
      HasNoMoreThanOneOverride<TScAgent>::InitiationConditionMethod::value,
      "TScAgent must have no more than one override method from methods: `GetInitiationCondition`, "
      "`GetInitiationConditionTemplate` "
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
    auto const & ResolveAction = [](TScEvent const & event, ScAgent & agent) -> ScAction
    {
      auto [subscriptionElementAddr, _, otherElementAddr] = event.GetTriple();
      if (subscriptionElementAddr == ScKeynodes::action_initiated)
      {
        ScAddr const & actionAddr = otherElementAddr;
        return agent.m_memoryCtx.UseAction(actionAddr);
      }

      return agent.m_memoryCtx.CreateAction(agent.GetActionClass()).Initiate();
    };

    TScAgent agent;
    std::string const & agentName = agent.GetName();
    SC_LOG_INFO("Agent `" << agentName << "` started");

    agent.SetInitiator(event.GetUser());
    agent.SetImplementation(agentImplementationAddr);
    if (agent.IsActionClassDeactivated())
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was finished because actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return;
    }

    ScAction action = ResolveAction(event, agent);
    if (!agent.template ValidateInitiationCondition<TScAgent>(event))
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was finished because its initiation condition was checked unsuccessfully.");
      return;
    }

    ScResult result;
    if constexpr (HasOverride<TScAgent>::DoProgramWithEventArgument::value)
      result = agent.DoProgram(event, action);
    else
      result = agent.DoProgram(action);

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agentName << "` finished successfully");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agentName << "` finished unsuccessfully");
    else
      SC_LOG_INFO("Agent `" << agentName << "` finished with error");

    if (!agent.template ValidateResultCondition<TScAgent>(event, action))
    {
      SC_LOG_WARNING("Result condition of agent `" << agentName << "` checked unsuccessfully.");
      return;
    }
  };
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::IsActionClassDeactivated()
{
  return ScMemory::ms_globalContext->HelperCheckEdge(
      ScKeynodes::action_deactivated, this->GetActionClass(), ScType::EdgeAccessConstPosPerm);
}

template <class TScEvent, class TScContext>
template <class TScAgent>
bool ScAgent<TScEvent, TScContext>::ValidateInitiationCondition(TScEvent const & event)
{
  if constexpr (HasOverride<TScAgent>::CheckInitiationCondition::value)
    return this->CheckInitiationCondition(event);

  ScTemplate initiationConditionTemplate;
  if constexpr (HasOverride<TScAgent>::GetInitiationConditionTemplate::value)
  {
    initiationConditionTemplate = this->GetInitiationConditionTemplate();
  }
  else if constexpr (HasOverride<TScAgent>::GetInitiationCondition::value)
  {
    ScAddr const & initiationConditionAddr = this->GetInitiationCondition();
    initiationConditionTemplate = BuildCheckTemplate(event, initiationConditionAddr);
  }
  else if (this->MayBeSpecified())
  {
    ScAddr const & initiationConditionAddr = this->GetInitiationCondition();
    initiationConditionTemplate = BuildCheckTemplate(event, initiationConditionAddr);
  }

  if (initiationConditionTemplate.IsEmpty())
    return true;

  ScTemplateSearchResult searchResult;
  return this->m_memoryCtx.HelperSearchTemplate(initiationConditionTemplate, searchResult);
}

template <class TScEvent, class TScContext>
template <class TScAgent>
bool ScAgent<TScEvent, TScContext>::ValidateResultCondition(TScEvent const & event, ScAction & action)
{
  if constexpr (HasOverride<TScAgent>::CheckResultCondition::value)
    return this->CheckResultCondition(event, action);

  ScTemplate resultConditionTemplate;
  if constexpr (HasOverride<TScAgent>::GetResultConditionTemplate::value)
  {
    resultConditionTemplate = this->GetResultConditionTemplate();
  }
  else if constexpr (HasOverride<TScAgent>::GetResultCondition::value)
  {
    ScAddr const & resultConditionAddr = this->GetResultCondition();
    resultConditionTemplate = BuildCheckTemplate(event, resultConditionAddr);
  }
  else if (this->MayBeSpecified())
  {
    ScAddr const & resultConditionAddr = this->GetResultCondition();
    resultConditionTemplate = BuildCheckTemplate(event, resultConditionAddr);
  }

  if (resultConditionTemplate.IsEmpty())
    return true;

  ScTemplateSearchResult searchResult;
  return this->m_memoryCtx.HelperSearchTemplate(resultConditionTemplate, searchResult);
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::BuildCheckTemplate(TScEvent const & event, ScAddr const & checkTemplateAddr)
{
  auto [_eventSubscriptionElementAddr, _, _otherElementAddr] = event.GetTriple();
  ScAddr const & eventSubscriptionElementAddr = _eventSubscriptionElementAddr;
  ScAddr const & otherElementAddr = _otherElementAddr;

  auto const & GetIteratorForEventTripleWithOutgoingArc = [&]() -> ScIterator5Ptr
  {
    return this->m_memoryCtx.Iterator5(
        eventSubscriptionElementAddr, ScType::Var, ScType::Unknown, ScType::EdgeAccessConstPosPerm, checkTemplateAddr);
  };
  auto const & GetIteratorForEventTripleWithIncomingArc = [&]() -> ScIterator5Ptr
  {
    return this->m_memoryCtx.Iterator5(
        ScType::Unknown, ScType::Var, eventSubscriptionElementAddr, ScType::EdgeAccessConstPosPerm, checkTemplateAddr);
  };

  auto const & GetIteratorForEventTripleWithEdge = GetIteratorForEventTripleWithOutgoingArc;

  auto const & GetIteratorForEventTripleWithConnectorWithOutgoingDirection = GetIteratorForEventTripleWithOutgoingArc;
  auto const & GetIteratorForEventTripleWithConnectorWithIncomingDirection = GetIteratorForEventTripleWithOutgoingArc;

  ScAddrToValueUnorderedMap<std::tuple<std::function<ScIterator5Ptr()>, size_t>> eventToEventTripleIterators = {
      {ScKeynodes::sc_event_generate_incoming_arc, {GetIteratorForEventTripleWithIncomingArc, 0u}},
      {ScKeynodes::sc_event_erase_incoming_arc, {GetIteratorForEventTripleWithIncomingArc, 0u}},
      {ScKeynodes::sc_event_generate_outgoing_arc, {GetIteratorForEventTripleWithOutgoingArc, 2u}},
      {ScKeynodes::sc_event_erase_outgoing_arc, {GetIteratorForEventTripleWithOutgoingArc, 2u}},
      {ScKeynodes::sc_event_generate_edge, {GetIteratorForEventTripleWithEdge, 2u}},
      {ScKeynodes::sc_event_erase_edge, {GetIteratorForEventTripleWithEdge, 2u}},
      {ScKeynodes::sc_event_generate_connector, {GetIteratorForEventTripleWithConnectorWithOutgoingDirection, 2u}},
      {ScKeynodes::sc_event_erase_connector, {GetIteratorForEventTripleWithConnectorWithOutgoingDirection, 2u}}};

  ScAddr const & eventClassAddr = event.GetEventClass();
  auto const & iteratorIt = eventToEventTripleIterators.find(eventClassAddr);
  if (iteratorIt == eventToEventTripleIterators.cend())
  {
    SC_LOG_WARNING(
        "Event class for agent class `"
        << this->GetName()
        << "` is unknown. It is impossible to check initiation condition (or result condition) template.");
    return ScTemplate();
  }

  auto [getIteratorForEventTriple, otherElementPosition] = iteratorIt->second;
  ScIterator5Ptr eventTripleIterator = getIteratorForEventTriple();
  bool checkTemplateParamsIsGenerated = false;
  ScTemplateParams checkTemplateParams;
  if (eventTripleIterator->IsValid())
    checkTemplateParamsIsGenerated = GenerateCheckTemplateParams(
        checkTemplateAddr,
        eventSubscriptionElementAddr,
        otherElementAddr,
        otherElementPosition,
        eventTripleIterator,
        checkTemplateParams);

  if (checkTemplateParamsIsGenerated
      && (eventClassAddr == ScKeynodes::sc_event_generate_connector
          || eventClassAddr == ScKeynodes::sc_event_erase_connector))
  {
    eventTripleIterator = GetIteratorForEventTripleWithConnectorWithIncomingDirection();
    if (eventTripleIterator->IsValid())
      GenerateCheckTemplateParams(
          checkTemplateAddr,
          eventSubscriptionElementAddr,
          otherElementAddr,
          otherElementPosition,
          eventTripleIterator,
          checkTemplateParams);
  }

  ScTemplate checkTemplate;
  this->m_memoryCtx.HelperBuildTemplate(checkTemplate, checkTemplateAddr, checkTemplateParams);
  return checkTemplate;
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::GenerateCheckTemplateParams(
    ScAddr const & checkTemplateAddr,
    ScAddr const & eventSubscriptionElementAddr,
    ScAddr const & otherElementAddr,
    size_t otherElementPosition,
    ScIterator5Ptr const eventTripleIterator,
    ScTemplateParams & checkTemplateParams)
{
  ScTemplateParams params;
  ScIterator3Ptr const subscriptionElementIterator =
      this->m_memoryCtx.Iterator3(checkTemplateAddr, ScType::EdgeAccessConstPosPerm, eventSubscriptionElementAddr);
  if (subscriptionElementIterator->Next())
  {
    if (eventTripleIterator->Next())
    {
      ScAddr const & otherVarAddr = eventTripleIterator->Get(otherElementPosition);
      if (this->m_memoryCtx.GetElementType(otherVarAddr).IsVar() && otherVarAddr != otherElementAddr)
        params.Add(otherElementPosition, otherElementAddr);
    }
    else
    {
      SC_LOG_WARNING(
          "Initiation condition (or result condition) template of agent class `"
          << this->GetName()
          << "` checks initiated sc-event incorrectly. May be types of sc-elements "
             "in initiation condition or result template are not correct.");
      return false;
    }
  }
  else
  {
    SC_LOG_WARNING(
        "Initiation condition (or result condition) template of agent class `"
        << this->GetName()
        << "` doesn't check initiated sc-event. Check that agent initiation "
           "condition and result templates are correct.");

    return false;
  }

  return true;
}
