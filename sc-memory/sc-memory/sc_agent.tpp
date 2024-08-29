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
ScAgentBase<TScEvent, TScContext>::ScAgentBase() noexcept
  : m_context(nullptr)
{
}

template <class TScEvent, class TScContext>
ScAgentBase<TScEvent, TScContext>::~ScAgentBase() noexcept
{
  m_context.Destroy();
}

template <class TScEvent, class TScContext>
ScAddr ScAgentBase<TScEvent, TScContext>::GetAbstractAgent() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.Iterator5(
      ScType::NodeConst,
      ScType::EdgeDCommonConst,
      m_agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_inclusion);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Agent implementation `" << this->GetName()
                                 << "` is not included to any abstract sc-agent. Check that agent implementation has "
                                    "specified relation `nrel_inclusion`.");

  return it5->Get(0);
}

template <class TScEvent, class TScContext>
ScAddr ScAgentBase<TScEvent, TScContext>::GetEventClass() const noexcept(false)
{
  if constexpr (!std::is_same<TScEvent, ScElementaryEvent>::value)
    return TScEvent::eventClassAddr;

  ScIterator5Ptr const it5 = m_context.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have primary initiation condition. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_context.GetEdgeSource(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScAddr ScAgentBase<TScEvent, TScContext>::GetEventSubscriptionElement() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have primary initiation condition. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_context.GetEdgeTarget(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScAddr ScAgentBase<TScEvent, TScContext>::GetActionClass() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::NodeConstClass,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent `" << this->GetName()
                              << "` does not have action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  return it5->Get(2);
}

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
bool ScAgentBase<TScEvent, TScContext>::CheckInitiationCondition(TScEvent const & event)
{
  return true;
}

// LCOV_EXCL_STOP

template <class TScEvent, class TScContext>
ScAddr ScAgentBase<TScEvent, TScContext>::GetInitiationCondition() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_context.GetEdgeSource(it5->Get(2));
}

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
ScTemplate ScAgentBase<TScEvent, TScContext>::GetInitiationConditionTemplate(TScEvent const &) const
{
  return ScTemplate();
}

// LCOV_EXCL_STOP

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
ScResult ScAgentBase<TScEvent, TScContext>::DoProgram(TScEvent const & event, ScAction & action)
{
  return SC_RESULT_OK;
}

template <class TScEvent, class TScContext>
ScResult ScAgentBase<TScEvent, TScContext>::DoProgram(ScAction & action)
{
  return SC_RESULT_OK;
}

// LCOV_EXCL_STOP

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
bool ScAgentBase<TScEvent, TScContext>::CheckResultCondition(TScEvent const &, ScAction &)
{
  return true;
}

// LCOV_EXCL_STOP

template <class TScEvent, class TScContext>
ScAddr ScAgentBase<TScEvent, TScContext>::GetResultCondition() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_context.GetEdgeTarget(it5->Get(2));
}

// LCOV_EXCL_START
template <class TScEvent, class TScContext>
ScTemplate ScAgentBase<TScEvent, TScContext>::GetResultConditionTemplate(TScEvent const &, ScAction &) const
{
  return ScTemplate();
}

// LCOV_EXCL_STOP

template <class TScEvent, class TScContext>
void ScAgentBase<TScEvent, TScContext>::SetInitiator(ScAddr const & userAddr) noexcept
{
  m_context = ScAgentContext(userAddr);
}

template <class TScEvent, class TScContext>
void ScAgentBase<TScEvent, TScContext>::SetImplementation(ScAddr const & agentImplementationAddr) noexcept
{
  m_agentImplementationAddr = agentImplementationAddr;
}

template <class TScEvent, class TScContext>
bool ScAgentBase<TScEvent, TScContext>::MayBeSpecified() const noexcept
{
  return m_context.IsElement(m_agentImplementationAddr);
}

template <class TScEvent, class TScContext>
std::function<void(TScEvent const &)> ScAgentBase<TScEvent, TScContext>::GetCallback(ScAddr const &) noexcept
{
  return {};
}

template <class TScEvent, class TScContext>
ScAgent<TScEvent, TScContext>::ScAgent() noexcept
  : ScAgentBase<TScEvent>(){};

template <class TScEvent, class TScContext>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent, TScContext>::Subscribe(
    ScMemoryContext * context,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs) noexcept(false)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetInitiator(context->GetUser());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentName = agent.GetName();
  if (!ScAgentBase<TScEvent>::m_events.count(agentName))
    ScAgentBase<TScEvent>::m_events.insert({agentName, {}});

  ScAddrVector subscriptionVector{subscriptionAddrs...};
  // Check that user specify agent implementation only and find subscription sc-element in knowledge base.
  if (agent.MayBeSpecified() || subscriptionVector.empty())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = ScAgentBase<TScEvent>::m_events.find(agentName)->second;
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
               TScAgent::template GetCallback<TScAgent>(agentImplementationAddr))});
    }
    else
    {
      subscriptionsMap.insert(
          {subscriptionElementAddr,
           new ScElementaryEventSubscription<TScEvent>(
               *context, subscriptionElementAddr, TScAgent::template GetCallback<TScAgent>(agentImplementationAddr))});
    }
  }
}

template <class TScEvent, class TScContext>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent, TScContext>::Unsubscribe(
    ScMemoryContext * context,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs) noexcept(false)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetInitiator(context->GetUser());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentName = agent.GetName();
  auto const & agentsMapIt = ScAgentBase<TScEvent>::m_events.find(agentName);
  if (agentsMapIt == ScAgentBase<TScEvent>::m_events.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Agent `" << agentName << "` does not have been subscribed to any events yet.");

  ScAddrVector subscriptionVector{subscriptionAddrs...};
  // Check that user specify agent implementation only and find subscription sc-element in knowledge base.
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

template <class TScEvent, class TScContext>
template <class TScAgent>
std::function<void(TScEvent const &)> ScAgent<TScEvent, TScContext>::GetCallback(
    ScAddr const & agentImplementationAddr) noexcept
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

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
    auto const & ResolveAction = [](TScEvent const & event, ScAgent & agent) -> ScAction
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
    if (!agent.template ValidateInitiationCondition<TScAgent>(event))
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
    if (!agent.template ValidateResultCondition<TScAgent>(event, action))
    {
      SC_LOG_WARNING("Result condition of agent `" << agentName << "` checked unsuccessfully.");
      return;
    }
    // SC_LOG_INFO("Agent `" << agentName << "` finished checking result condition.");
  };
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::IsActionClassDeactivated() noexcept
{
  return ScMemory::ms_globalContext->HelperCheckEdge(
      ScKeynodes::action_deactivated, this->GetActionClass(), ScType::EdgeAccessConstPosPerm);
}

template <class TScEvent, class TScContext>
template <class TScAgent>
bool ScAgent<TScEvent, TScContext>::ValidateInitiationCondition(TScEvent const & event) noexcept
{
  if constexpr (HasOverride<TScAgent>::CheckInitiationCondition::value)
    return this->CheckInitiationCondition(event);

  ScTemplate initiationConditionTemplate;
  if constexpr (HasOverride<TScAgent>::GetInitiationConditionTemplate::value)
  {
    initiationConditionTemplate = this->GetInitiationConditionTemplate(event);
  }
  else if constexpr (HasOverride<TScAgent>::GetInitiationCondition::value)
  {
    ScAddr const & initiationConditionAddr = this->GetInitiationCondition();
    initiationConditionTemplate = BuildInitiationConditionTemplate(event, initiationConditionAddr);
  }
  else if (this->MayBeSpecified())
  {
    ScAddr const & initiationConditionAddr = this->GetInitiationCondition();
    initiationConditionTemplate = BuildInitiationConditionTemplate(event, initiationConditionAddr);
  }
  else
  {
    return true;
  }

  if (initiationConditionTemplate.IsEmpty())
    return false;

  ScTemplateSearchResult searchResult;
  return this->m_context.HelperSearchTemplate(initiationConditionTemplate, searchResult);
}

template <class TScEvent, class TScContext>
template <class TScAgent>
bool ScAgent<TScEvent, TScContext>::ValidateResultCondition(TScEvent const & event, ScAction & action) noexcept
{
  if constexpr (HasOverride<TScAgent>::CheckResultCondition::value)
    return this->CheckResultCondition(event, action);

  ScTemplate resultConditionTemplate;
  if constexpr (HasOverride<TScAgent>::GetResultConditionTemplate::value)
  {
    resultConditionTemplate = this->GetResultConditionTemplate(event, action);
  }
  else if constexpr (HasOverride<TScAgent>::GetResultCondition::value)
  {
    ScAddr const & resultConditionAddr = this->GetResultCondition();
    resultConditionTemplate = BuildResultConditionTemplate(event, resultConditionAddr);
  }
  else if (this->MayBeSpecified())
  {
    ScAddr const & resultConditionAddr = this->GetResultCondition();
    resultConditionTemplate = BuildResultConditionTemplate(event, resultConditionAddr);
  }

  if (resultConditionTemplate.IsEmpty())
    return true;

  ScStructure const & result = action.GetResult();
  bool isFound = false;
  this->m_context.HelperSearchTemplate(
      resultConditionTemplate,
      [&isFound](ScTemplateResultItem const & item) -> void
      {
        isFound = true;
      },
      [&](ScAddr const & elementAddr) -> bool
      {
        bool isBelong = result.HasElement(elementAddr);
        return isBelong;
      });
  return isFound;
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::BuildInitiationConditionTemplate(
    TScEvent const & event,
    ScAddr const & initiationConditionTemplateAddr) noexcept
{
  ScAddr const & eventSubscriptionElementAddr = event.GetSubscriptionElement();

  auto const & GetIteratorForEventTripleWithOutgoingArc = [&]() -> ScIterator5Ptr
  {
    return this->m_context.Iterator5(
        eventSubscriptionElementAddr,
        ScType::Var,
        ScType::Unknown,
        ScType::EdgeAccessConstPosPerm,
        initiationConditionTemplateAddr);
  };
  auto const & GetIteratorForEventTripleWithIncomingArc = [&]() -> ScIterator5Ptr
  {
    return this->m_context.Iterator5(
        ScType::Unknown,
        ScType::Var,
        eventSubscriptionElementAddr,
        ScType::EdgeAccessConstPosPerm,
        initiationConditionTemplateAddr);
  };

  auto const & GetIteratorForEventTripleWithEdge = GetIteratorForEventTripleWithOutgoingArc;

  auto const & GetIteratorForEventTripleWithConnectorWithOutgoingDirection = GetIteratorForEventTripleWithOutgoingArc;
  auto const & GetIteratorForEventTripleWithConnectorWithIncomingDirection = GetIteratorForEventTripleWithIncomingArc;

  ScAddrToValueUnorderedMap<std::tuple<std::function<ScIterator5Ptr()>, size_t>> eventToEventTripleIterators = {
      {ScKeynodes::sc_event_after_generate_incoming_arc, {GetIteratorForEventTripleWithIncomingArc, 0u}},
      {ScKeynodes::sc_event_before_erase_incoming_arc, {GetIteratorForEventTripleWithIncomingArc, 0u}},
      {ScKeynodes::sc_event_after_generate_outgoing_arc, {GetIteratorForEventTripleWithOutgoingArc, 2u}},
      {ScKeynodes::sc_event_before_erase_outgoing_arc, {GetIteratorForEventTripleWithOutgoingArc, 2u}},
      {ScKeynodes::sc_event_after_generate_edge, {GetIteratorForEventTripleWithEdge, 2u}},
      {ScKeynodes::sc_event_before_erase_edge, {GetIteratorForEventTripleWithEdge, 2u}},
      {ScKeynodes::sc_event_after_generate_connector,
       {GetIteratorForEventTripleWithConnectorWithOutgoingDirection, 2u}},
      {ScKeynodes::sc_event_before_erase_connector, {GetIteratorForEventTripleWithConnectorWithOutgoingDirection, 2u}}};

  ScAddr const & eventClassAddr = event.GetEventClass();
  auto const & iteratorIt = eventToEventTripleIterators.find(eventClassAddr);
  if (iteratorIt == eventToEventTripleIterators.cend())
  {
    SC_LOG_WARNING(
        "Event class for agent class `" << this->GetName()
                                        << "` is unknown. It is impossible to check initiation condition template.");
    return ScTemplate();
  }

  auto [getIteratorForEventTriple, otherElementPosition] = iteratorIt->second;
  ScIterator5Ptr eventTripleIterator = getIteratorForEventTriple();
  bool templateParamsIsGenerated = false;
  ScTemplateParams templateParams;
  if (eventTripleIterator->IsValid())
    templateParamsIsGenerated = GenerateCheckTemplateParams(
        initiationConditionTemplateAddr, event, otherElementPosition, eventTripleIterator, templateParams);

  if (templateParamsIsGenerated
      && (eventClassAddr == ScKeynodes::sc_event_after_generate_connector
          || eventClassAddr == ScKeynodes::sc_event_before_erase_connector))
  {
    eventTripleIterator = GetIteratorForEventTripleWithConnectorWithIncomingDirection();
    if (eventTripleIterator->IsValid())
    {
      otherElementPosition = 0u;
      GenerateCheckTemplateParams(
          initiationConditionTemplateAddr, event, otherElementPosition, eventTripleIterator, templateParams);
    }
  }

  if (!templateParamsIsGenerated)
    return ScTemplate();

  ScTemplate initiationConditionTemplate;
  this->m_context.HelperBuildTemplate(initiationConditionTemplate, initiationConditionTemplateAddr, templateParams);
  return initiationConditionTemplate;
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::BuildResultConditionTemplate(
    TScEvent const & event,
    ScAddr const & resultConditionTemplateAddr) noexcept
{
  ScTemplate resultConditionTemplate;
  this->m_context.HelperBuildTemplate(resultConditionTemplate, resultConditionTemplateAddr);
  return resultConditionTemplate;
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::GenerateCheckTemplateParams(
    ScAddr const & initiationConditionTemplateAddr,
    TScEvent const & event,
    size_t otherElementPosition,
    ScIterator5Ptr const eventTripleIterator,
    ScTemplateParams & checkTemplateParams) noexcept
{
  auto [_eventSubscriptionElementAddr, _connectorAddr, _otherElementAddr] = event.GetTriple();
  ScAddr const & eventSubscriptionElementAddr = _eventSubscriptionElementAddr;
  ScAddr const & connectorAddr = _connectorAddr;
  ScAddr const & otherElementAddr = _otherElementAddr;

  ScType const & connectorType = this->m_context.GetElementType(connectorAddr);
  ScType const & otherElementType = this->m_context.GetElementType(otherElementAddr);

  ScTemplateParams params;
  ScIterator3Ptr const subscriptionElementIterator = this->m_context.Iterator3(
      initiationConditionTemplateAddr, ScType::EdgeAccessConstPosPerm, eventSubscriptionElementAddr);
  if (subscriptionElementIterator->Next())
  {
    if (eventTripleIterator->Next())
    {
      ScAddr const & connectorVarAddr = eventTripleIterator->Get(1);
      ScAddr const & otherVarAddr = eventTripleIterator->Get(otherElementPosition);

      ScType const & connectorVarType = this->m_context.GetElementType(connectorVarAddr);
      ScType const & otherVarType = this->m_context.GetElementType(otherVarAddr);

      ScType const & isOtherElementSubstitutable =
          otherElementAddr != otherVarAddr
              ? otherVarType.IsVar() && otherElementType.BitAnd(otherVarType.AsConst()) == otherElementType
              : false;
      if (isOtherElementSubstitutable)
        params.Add(otherVarAddr, otherElementAddr);

      ScType const & isConnectorSubstitutable =
          connectorVarType.IsVar() ? connectorType.BitAnd(connectorVarType.AsConst()) == connectorType : false;
      if (isConnectorSubstitutable)
        params.Add(connectorVarAddr, connectorAddr);
      else
      {
        SC_LOG_WARNING(
            "Initiation condition template of agent class `"
            << this->GetName()
            << "` checks initiated sc-event incorrectly. Maybe initiation condition template has triple with incorrect "
               "sc-element types to "
               "substitute sc-elements involved in initiated sc-event.");
        return false;
      }
    }
    else
    {
      SC_LOG_WARNING(
          "Initiation condition template of agent class `"
          << this->GetName()
          << "` checks initiated sc-event incorrectly. Maybe initiation condition template does not have triple to "
             "substitute sc-elements involved in initiated sc-event.");
      return false;
    }

    if (eventTripleIterator->Next())
    {
      SC_LOG_WARNING(
          "Initiation condition template of agent class `"
          << this->GetName()
          << "` checks initiated sc-event incorrectly. Maybe initiation condition template has triple to "
             "substitute sc-elements involved in initiated sc-event twice.");
      return false;
    }
  }
  else
  {
    SC_LOG_WARNING(
        "Initiation condition template of agent class `"
        << this->GetName()
        << "` doesn't check initiated sc-event. Maybe agent initiation condition template does not have sc-event "
           "subscription sc-element.");
    return false;
  }

  return true;
}
