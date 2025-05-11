/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_agent.hpp"

#include "sc-memory/sc_action.hpp"
#include "sc-memory/sc_result.hpp"
#include "sc-memory/sc_event_subscription.hpp"
#include "sc-memory/sc_keynodes.hpp"

template <class TScEvent, class TScContext>
ScAgent<TScEvent, TScContext>::ScAgent() noexcept
  : m_context(nullptr)
{
}

template <class TScEvent, class TScContext>
ScAgent<TScEvent, TScContext>::~ScAgent() noexcept
{
  m_context.Destroy();
}

template <class TScEvent, class TScContext>
ScAddr ScAgent<TScEvent, TScContext>::GetAbstractAgent() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      ScType::ConstNode,
      ScType::ConstCommonArc,
      m_agentImplementationAddr,
      ScType::ConstPermPosArc,
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
ScAddr ScAgent<TScEvent, TScContext>::GetEventClass() const noexcept(false)
{
  if constexpr (!std::is_same<TScEvent, ScElementaryEvent>::value)
    return TScEvent::eventClassAddr;

  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      GetAbstractAgent(),
      ScType::ConstCommonArc,
      ScType::ConstCommonArc,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have primary initiation condition. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_context.GetArcSourceElement(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScAddr ScAgent<TScEvent, TScContext>::GetEventSubscriptionElement() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      GetAbstractAgent(),
      ScType::ConstCommonArc,
      ScType::ConstCommonArc,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have primary initiation condition. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_context.GetArcTargetElement(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScAddr ScAgent<TScEvent, TScContext>::GetActionClass() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      GetAbstractAgent(),
      ScType::ConstCommonArc,
      ScType::ConstNodeClass,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent `" << this->GetName()
                              << "` does not have action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  return it5->Get(2);
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::CheckInitiationCondition(TScEvent const & event)
{
  return true;
}

template <class TScEvent, class TScContext>
ScAddr ScAgent<TScEvent, TScContext>::GetInitiationCondition() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      GetAbstractAgent(),
      ScType::ConstCommonArc,
      ScType::ConstCommonArc,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_context.GetArcSourceElement(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::GetInitiationConditionTemplate(TScEvent const &) const
{
  return ScTemplate();
}

template <class TScEvent, class TScContext>
ScResult ScAgent<TScEvent, TScContext>::DoProgram(TScEvent const & event, ScAction & action)
{
  return SC_RESULT_OK;
}

template <class TScEvent, class TScContext>
ScResult ScAgent<TScEvent, TScContext>::DoProgram(ScAction & action)
{
  return SC_RESULT_OK;
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::CheckResultCondition(TScEvent const &, ScAction &)
{
  return true;
}

template <class TScEvent, class TScContext>
ScAddr ScAgent<TScEvent, TScContext>::GetResultCondition() const noexcept(false)
{
  ScIterator5Ptr const it5 = m_context.CreateIterator5(
      GetAbstractAgent(),
      ScType::ConstCommonArc,
      ScType::ConstCommonArc,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound,
        "Abstract sc-agent for agent implementation `"
            << this->GetName()
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_context.GetArcTargetElement(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::GetResultConditionTemplate(TScEvent const &, ScAction &) const
{
  return ScTemplate();
}

template <class TScEvent, class TScContext>
void ScAgent<TScEvent, TScContext>::SetInitiator(ScAddr const & userAddr) noexcept
{
  m_context = TScContext(userAddr);
}

template <class TScEvent, class TScContext>
void ScAgent<TScEvent, TScContext>::SetImplementation(ScAddr const & agentImplementationAddr) noexcept
{
  m_agentImplementationAddr = agentImplementationAddr;
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::IsActionClassDeactivated() noexcept
{
  return ScMemory::ms_globalContext->CheckConnector(
      ScKeynodes::action_deactivated, this->GetActionClass(), ScType::ConstPermPosArc);
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::MayBeSpecified() const noexcept
{
  return m_context.IsElement(m_agentImplementationAddr);
}

template <class TScEvent, class TScContext>
template <class TScAgent, typename HasOverride>
bool ScAgent<TScEvent, TScContext>::ValidateInitiationCondition(TScEvent const & event) noexcept(false)
{
  if constexpr (HasOverride::CheckInitiationCondition::value)
    return this->CheckInitiationCondition(event);

  ScTemplate initiationConditionTemplate;
  if constexpr (HasOverride::GetInitiationConditionTemplate::value)
  {
    initiationConditionTemplate = this->GetInitiationConditionTemplate(event);
  }
  else if constexpr (HasOverride::GetInitiationCondition::value)
  {
    ScAddr const & initiationConditionAddr = this->GetInitiationCondition();
    initiationConditionTemplate = BuildInitiationConditionTemplate(event, initiationConditionAddr);
  }
  else if (this->MayBeSpecified())
  {
    ScAddr const & initiationConditionAddr = this->GetInitiationCondition();
    initiationConditionTemplate = BuildInitiationConditionTemplate(event, initiationConditionAddr);
  }
  else if constexpr (std::is_base_of<ScActionInitiatedAgent, TScAgent>::value)
  {
    initiationConditionTemplate = this->GetInitiationConditionTemplate(event);
  }
  else
  {
    return true;
  }

  if (initiationConditionTemplate.IsEmpty())
    return false;

  bool isFound = false;
  ScTemplateSearchResult searchResult;
  try
  {
    isFound = this->m_context.SearchByTemplate(initiationConditionTemplate, searchResult);
  }
  catch (utils::ScException const & exception)
  {
    m_logger.Error("Initiation condition template is not valid. ", exception.Message());
  }
  return isFound;
}

template <class TScEvent, class TScContext>
template <class TScAgent, typename HasOverride>
bool ScAgent<TScEvent, TScContext>::ValidateResultCondition(TScEvent const & event, ScAction & action) noexcept(false)
{
  if constexpr (HasOverride::CheckResultCondition::value)
    return this->CheckResultCondition(event, action);

  ScTemplate resultConditionTemplate;
  if constexpr (HasOverride::GetResultConditionTemplate::value)
  {
    resultConditionTemplate = this->GetResultConditionTemplate(event, action);
  }
  else if constexpr (HasOverride::GetResultCondition::value)
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
  try
  {
    this->m_context.SearchByTemplateInterruptibly(
        resultConditionTemplate,
        [&isFound](ScTemplateResultItem const & item) -> ScTemplateSearchRequest
        {
          isFound = true;
          return ScTemplateSearchRequest::STOP;
        },
        [&](ScAddr const & elementAddr) -> bool
        {
          bool isBelong = result.HasElement(elementAddr);
          return isBelong;
        });
  }
  catch (utils::ScException const & exception)
  {
    m_logger.Error("Result condition template is not valid. ", exception.Message());
  }
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
    return this->m_context.CreateIterator5(
        eventSubscriptionElementAddr,
        ScType::Var,
        ScType::Unknown,
        ScType::ConstPermPosArc,
        initiationConditionTemplateAddr);
  };
  auto const & GetIteratorForEventTripleWithIncomingArc = [&]() -> ScIterator5Ptr
  {
    return this->m_context.CreateIterator5(
        ScType::Unknown,
        ScType::Var,
        eventSubscriptionElementAddr,
        ScType::ConstPermPosArc,
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
      {ScKeynodes::sc_event_before_erase_connector, {GetIteratorForEventTripleWithConnectorWithOutgoingDirection, 2u}},
      {ScKeynodes::sc_event_before_erase_element, {nullptr, 0u}},
      {ScKeynodes::sc_event_before_change_link_content, {nullptr, 0u}},
  };

  ScAddr const & eventClassAddr = event.GetEventClass();
  auto const & iteratorIt = eventToEventTripleIterators.find(eventClassAddr);
  if (iteratorIt == eventToEventTripleIterators.cend())
  {
    m_logger.Warning(
        "Event class for agent class `",
        this->GetName(),
        "` is unknown. It is impossible to check initiation condition template.");
    return ScTemplate();
  }

  auto [getIteratorForEventTriple, otherElementPosition] = iteratorIt->second;
  ScTemplateParams templateParams;
  if (getIteratorForEventTriple != nullptr)
  {
    ScIterator5Ptr eventTripleIterator = getIteratorForEventTriple();
    bool templateParamsIsGenerated = false;
    if (eventTripleIterator->IsValid())
      templateParamsIsGenerated = GenerateCheckTemplateParams(
          initiationConditionTemplateAddr, event, otherElementPosition, eventTripleIterator, templateParams);

    // If params were no generated then tries to find reverse iterator for connectors events.
    if (!templateParamsIsGenerated
        && (eventClassAddr == ScKeynodes::sc_event_after_generate_connector
            || eventClassAddr == ScKeynodes::sc_event_before_erase_connector))
    {
      eventTripleIterator = GetIteratorForEventTripleWithConnectorWithIncomingDirection();
      if (eventTripleIterator->IsValid())
      {
        otherElementPosition = 0u;
        templateParamsIsGenerated = GenerateCheckTemplateParams(
            initiationConditionTemplateAddr, event, otherElementPosition, eventTripleIterator, templateParams);
      }
    }

    if (!templateParamsIsGenerated)
      return ScTemplate();
  }

  ScTemplate initiationConditionTemplate;
  this->m_context.BuildTemplate(initiationConditionTemplate, initiationConditionTemplateAddr, templateParams);
  return initiationConditionTemplate;
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::BuildResultConditionTemplate(
    TScEvent const & event,
    ScAddr const & resultConditionTemplateAddr) noexcept
{
  ScTemplate resultConditionTemplate;
  this->m_context.BuildTemplate(resultConditionTemplate, resultConditionTemplateAddr);
  return resultConditionTemplate;
}

template <class TScEvent, class TScContext>
bool ScAgent<TScEvent, TScContext>::GenerateCheckTemplateParams(
    ScAddr const & initiationConditionTemplateAddr,
    TScEvent const & event,
    size_t otherElementPosition,
    ScIterator5Ptr const & eventTripleIterator,
    ScTemplateParams & checkTemplateParams) noexcept
{
  auto [_eventSubscriptionElementAddr, _connectorAddr, _otherElementAddr] = event.GetTriple();
  ScAddr const & eventSubscriptionElementAddr = _eventSubscriptionElementAddr;
  ScAddr const & connectorAddr = _connectorAddr;
  ScAddr const & otherElementAddr = _otherElementAddr;

  ScType const & connectorType = this->m_context.GetElementType(connectorAddr);
  ScType const & otherElementType = this->m_context.GetElementType(otherElementAddr);

  ScIterator3Ptr const subscriptionElementIterator = this->m_context.CreateIterator3(
      initiationConditionTemplateAddr, ScType::ConstPermPosArc, eventSubscriptionElementAddr);
  if (subscriptionElementIterator->Next())
  {
    if (eventTripleIterator->Next())
    {
      ScAddr const & connectorVarAddr = eventTripleIterator->Get(1);
      ScAddr const & otherVarAddr = eventTripleIterator->Get(otherElementPosition);

      ScType const & connectorVarType = this->m_context.GetElementType(connectorVarAddr);
      ScType const & otherVarType = this->m_context.GetElementType(otherVarAddr);

      ScType const & otherVarTypeAsConst = otherVarType.AsConst();
      bool const isOtherElementSubstitutable = otherElementAddr != otherVarAddr && otherVarType.IsVar()
                                               && otherElementType.BitAnd(otherVarTypeAsConst) == otherVarTypeAsConst;
      if (isOtherElementSubstitutable)
        checkTemplateParams.Add(otherVarAddr, otherElementAddr);

      ScType const & connectorVarTypeAsConst = connectorVarType.AsConst();
      bool const isConnectorSubstitutable =
          connectorVarType.IsVar() && connectorType.BitAnd(connectorVarTypeAsConst) == connectorVarTypeAsConst;
      if (isConnectorSubstitutable)
        checkTemplateParams.Add(connectorVarAddr, connectorAddr);
      else
      {
        m_logger.Warning(
            "Initiation condition template of agent class `",
            this->GetName(),
            "` checks initiated sc-event incorrectly. Maybe initiation condition template has triple with incorrect "
            "sc-element types to "
            "substitute sc-elements involved in initiated sc-event.");
        checkTemplateParams = ScTemplateParams();
        return false;
      }
    }
    else
    {
      m_logger.Warning(
          "Initiation condition template of agent class `",
          this->GetName(),
          "` checks initiated sc-event incorrectly. Maybe initiation condition template does not have triple to "
          "substitute sc-elements involved in initiated sc-event.");
      return false;
    }

    if (eventTripleIterator->Next())
    {
      m_logger.Warning(
          "Initiation condition template of agent class `",
          this->GetName(),
          "` checks initiated sc-event incorrectly. Maybe initiation condition template has triple to "
          "substitute sc-elements involved in initiated sc-event twice.");
      checkTemplateParams = ScTemplateParams();
      return false;
    }
  }
  else
  {
    m_logger.Warning(
        "Initiation condition template of agent class `",
        this->GetName(),
        "` doesn't check initiated sc-event. Maybe agent initiation condition template does not have sc-event "
        "subscription sc-element.");
    return false;
  }

  return true;
}
