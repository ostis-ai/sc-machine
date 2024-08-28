/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_builder.hpp"

#include "sc_agent_context.hpp"

#include "sc_keynodes.hpp"

template <class TScAgent>
ScAgentBuilder<TScAgent>::ScAgentBuilder(ScAddr const & agentImplementationAddr) noexcept
  : ScAgentBuilder(nullptr, agentImplementationAddr)
{
}

template <class TScAgent>
ScAgentBuilder<TScAgent>::ScAgentBuilder(ScModule * module, ScAddr const & agentImplementationAddr) noexcept
  : m_module(module)
  , m_agentImplementationAddr(agentImplementationAddr)
{
}

template <class TScAgent>
ScAddr ScAgentBuilder<TScAgent>::GetAgentImplementation() const noexcept
{
  return m_agentImplementationAddr;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetAbstractAgent(ScAddr const & abstractAgentAddr) noexcept
{
  m_checkAbstractAgent = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_abstractAgentAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified abstract agent for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    ScIterator3Ptr it3 =
        context->Iterator3(ScKeynodes::abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_abstractAgentAddr);
    if (!it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-element for agent class `"
              << TScAgent::template GetName<TScAgent>()
              << "` is not abstract agent, because it does not belong to class `abstract_sc_agent`.");
  };

  m_abstractAgentAddr = abstractAgentAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetPrimaryInitiationCondition(
    std::tuple<ScAddr, ScAddr> const & primaryInitiationCondition) noexcept
{
  auto [eventClassAddr, eventSubscriptionElementAddr] = primaryInitiationCondition;

  m_checkPrimaryInitiationCondition = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_eventClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-event class for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    if (!context->IsElement(m_eventSubscriptionElementAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-event subscription sc-element for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                         << "` is not valid.");
  };

  m_eventClassAddr = eventClassAddr;
  m_eventSubscriptionElementAddr = eventSubscriptionElementAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetActionClass(ScAddr const & actionClassAddr) noexcept
{
  m_checkActionClass = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_actionClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified action class for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");
  };

  m_actionClassAddr = actionClassAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetInitiationConditionAndResult(
    std::tuple<ScAddr, ScAddr> const & initiationCondition) noexcept
{
  auto [initiationConditionAddr, resultConditionAddr] = initiationCondition;

  m_checkInitiationConditionAndResult = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_initiationConditionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified initiation condition template for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                      << "` is not valid.");

    if (!context->IsElement(m_resultConditionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified result condition template for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                  << "` is not valid.");
  };

  m_initiationConditionAddr = initiationConditionAddr;
  m_resultConditionAddr = resultConditionAddr;
  return this;
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveSpecification(ScMemoryContext * context) noexcept(false)
{
  std::string const & agentClassName = TScAgent::template GetName<TScAgent>();
  std::string agentImplementationName;

  ResolveAgentImplementation(context, agentImplementationName, agentClassName);
  ResolveAbstractAgent(context, agentImplementationName, agentClassName);

  std::string abstractAgentName = context->HelperGetSystemIdtf(m_abstractAgentAddr);
  if (abstractAgentName.empty())
    abstractAgentName = std::to_string(m_abstractAgentAddr.Hash());

  ResolvePrimaryInitiationCondition(context, abstractAgentName, agentClassName);
  ResolveActionClass(context, abstractAgentName, agentClassName);
  ResolveInitiationConditionAndResultCondition(context, abstractAgentName, agentClassName);
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveAgentImplementation(
    ScMemoryContext * context,
    std::string & agentImplementationName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_agentImplementationAddr))
  {
    agentImplementationName = context->HelperGetSystemIdtf(m_agentImplementationAddr);

    ScIterator3Ptr const it3 = context->Iterator3(
        ScKeynodes::platform_dependent_abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_agentImplementationAddr);
    if (!it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-element with system identifier `"
              << agentImplementationName << "` is not agent implementation for agent class `" << agentClassName
              << "`, because it does not belong to class `platform_dependent_abstract_sc_agent`.");

    if (it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent implementation with system identifier `"
              << agentImplementationName << "` for agent class `" << agentClassName
              << "` belongs to class `platform_dependent_abstract_sc_agent` twice.");

    SC_LOG_DEBUG("Agent implementation for agent class `" << agentClassName << "` was found.");
    return;
  }

  agentImplementationName = agentClassName;

  m_agentImplementationAddr = context->HelperFindBySystemIdtf(agentImplementationName);
  if (context->IsElement(m_agentImplementationAddr))
  {
    ScIterator3Ptr const it3 = context->Iterator3(
        ScKeynodes::platform_dependent_abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_agentImplementationAddr);
    if (!it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-element with system identifier `"
              << agentImplementationName << "` is not agent implementation for agent class `" << agentClassName
              << "`, because it does not belong to class `platform_dependent_abstract_sc_agent`.");

    if (it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent implementation with system identifier `"
              << agentImplementationName << "` for agent class `" << agentClassName
              << "` belongs to class `platform_dependent_abstract_sc_agent` twice.");

    SC_LOG_DEBUG(
        "Agent implementation for class `" << agentClassName << "` was not generated, because it already exists.");
  }
  else
  {
    m_agentImplementationAddr = context->CreateNode(ScType::NodeConst);
    context->HelperSetSystemIdtf(agentImplementationName, m_agentImplementationAddr);
    context->CreateEdge(
        ScType::EdgeAccessConstPosPerm, ScKeynodes::platform_dependent_abstract_sc_agent, m_agentImplementationAddr);
    SC_LOG_DEBUG("Agent implementation for class `" << agentClassName << "` was generated.");
  }
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveAbstractAgent(
    ScMemoryContext * context,
    std::string const & agentImplementationName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_abstractAgentAddr))
  {
    ScIterator5Ptr const it5 = context->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        m_agentImplementationAddr,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_inclusion);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and agent implementation for class `"
          << agentClassName << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & arcAddr =
          context->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, m_agentImplementationAddr);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and agent implementation for class `" << agentClassName
                                                                                             << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->Iterator5(
      ScType::Unknown,
      ScType::EdgeDCommonConst,
      m_agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_inclusion);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName
                                 << "` is not included to any abstract sc-agent. Check that agent implementation has "
                                    "specified relation `nrel_inclusion`.");

  m_abstractAgentAddr = it5->Get(0);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName << "` is included to two or more abstract agents.");

  ScType const & type = context->GetElementType(m_abstractAgentAddr);
  if (type.BitAnd(ScType::NodeConst) != ScType::NodeConst)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
            << agentImplementationName
            << "`, because sc-element does not have sc-type `ScType::NodeConst`, it has sc-type `" << type << "`.");

  ScIterator3Ptr const it3 =
      context->Iterator3(ScKeynodes::abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_abstractAgentAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
            << agentImplementationName << "`, because it does not belong to class `abstract_sc_agent`.");

  if (it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent for agent implementation `" << agentImplementationName
                                                       << "` belongs to class `abstract_sc_agent` twice.");

  SC_LOG_DEBUG("Abstract agent for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolvePrimaryInitiationCondition(
    ScMemoryContext * context,
    std::string const & abstractAgentName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_eventClassAddr) && context->IsElement(m_eventSubscriptionElementAddr))
  {
    ScIterator5Ptr const it5 = context->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        ScType::EdgeDCommonConst,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_primary_initiation_condition);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Primary initiation condition for class `" << agentClassName
                                                     << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & primaryConditionAddr =
          context->CreateEdge(ScType::EdgeDCommonConst, m_eventClassAddr, m_eventSubscriptionElementAddr);
      ScAddr const & arcAddr = context->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, primaryConditionAddr);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_primary_initiation_condition, arcAddr);
      SC_LOG_DEBUG("Primary initiation condition for class  `" << agentClassName << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->Iterator5(
      m_abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << abstractAgentName
            << "` does not have primary initiation condition. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  ScAddr const & primaryInitiationConditionAddr = it5->Get(2);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more primary initiation conditions.");

  ScType const & type = context->GetElementType(primaryInitiationConditionAddr);
  if (type.BitAnd(ScType::EdgeDCommonConst) != ScType::EdgeDCommonConst)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_primary_initiation_condition` is not primary initiation condition for "
        "abstract sc-agent `"
            << abstractAgentName
            << "`, because found sc-element does not have sc-type `ScType::EdgeDCommonConst`, it has sc-type `" << type
            << "`.");

  context->GetEdgeInfo(primaryInitiationConditionAddr, m_eventClassAddr, m_eventSubscriptionElementAddr);

  ScIterator3Ptr const it3 = context->Iterator3(ScKeynodes::sc_event, ScType::EdgeAccessConstPosPerm, m_eventClassAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "The first element of primary initiation condition for abstract sc-agent `"
            << abstractAgentName << "` is not sc-event class, because it doesn't belong to class `sc_event`.");

  if (it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-event class for abstract sc-agent `" << abstractAgentName << "` belongs to class `sc_event` twice.");

  SC_LOG_DEBUG("Primary initiation condition for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveActionClass(
    ScMemoryContext * context,
    std::string const & abstractAgentName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_actionClassAddr))
  {
    ScIterator5Ptr const it5 = context->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        m_actionClassAddr,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_sc_agent_action_class);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and action class for agent class `"
          << agentClassName << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & arcAddr = context->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, m_actionClassAddr);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_sc_agent_action_class, arcAddr);
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and action class for agent class `" << agentClassName
                                                                                           << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->Iterator5(
      m_abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName
                              << "` does not have action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  m_actionClassAddr = it5->Get(2);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more action classes.");

  ScType const & type = context->GetElementType(m_actionClassAddr);
  if (type.BitAnd(ScType::NodeConstClass) != ScType::NodeConstClass)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_sc_agent_action_class` is not action class for abstract sc-agent `"
            << abstractAgentName
            << "`, because sc-element does not have sc-type `ScType::NodeConstClass`, it has sc-type `" << type
            << "`.");

  SC_LOG_DEBUG("Action class for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveInitiationConditionAndResultCondition(
    ScMemoryContext * context,
    std::string const & abstractAgentName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_initiationConditionAddr) && context->IsElement(m_resultConditionAddr))
  {
    ScIterator5Ptr const it5 = context->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        ScType::EdgeDCommonConst,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_initiation_condition_and_result);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Initiation condition and result for class `" << agentClassName
                                                        << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & conditionAndResultAddr =
          context->CreateEdge(ScType::EdgeDCommonConst, m_initiationConditionAddr, m_resultConditionAddr);
      ScAddr const & arcAddr =
          context->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, conditionAndResultAddr);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_initiation_condition_and_result, arcAddr);
      SC_LOG_DEBUG("Initiation condition and result for class  `" << agentClassName << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->Iterator5(
      m_abstractAgentAddr,
      ScType::EdgeDCommonConst,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << abstractAgentName
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  ScAddr const & initiationConditionAndResultAddr = it5->Get(2);

  if (it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more initiation conditions and results.");

  ScType type = context->GetElementType(initiationConditionAndResultAddr);
  if (type.BitAnd(ScType::EdgeDCommonConst) != ScType::EdgeDCommonConst)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_initiation_condition_and_result` is initiation condition and result for "
        "abstract sc-agent `"
            << abstractAgentName
            << "`, because sc-element does not have sc-type `ScType::EdgeDCommonConst`, it has sc-type `" << type
            << "`.");

  context->GetEdgeInfo(initiationConditionAndResultAddr, m_initiationConditionAddr, m_resultConditionAddr);
  type = context->GetElementType(m_initiationConditionAddr);
  if (type.BitAnd(ScType::NodeConstStruct) != ScType::NodeConstStruct)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Initiation condition for abstract sc-agent `"
            << abstractAgentName
            << "` is not sc-template, because it does not have sc-type `ScType::NodeConstStruct`, it has sc-type `"
            << type << "`.");

  type = context->GetElementType(m_resultConditionAddr);
  if (type.BitAnd(ScType::NodeConstStruct) != ScType::NodeConstStruct)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Result condition for abstract sc-agent `"
            << abstractAgentName
            << "` is not sc-template, because it does not have sc-type `ScType::NodeConstStruct`, it has sc-type `"
            << type << "`.");

  SC_LOG_DEBUG("Initiation condition and result for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
ScModule * ScAgentBuilder<TScAgent>::FinishBuild() noexcept
{
  m_resolveSpecification = [this](ScMemoryContext * context)
  {
    ResolveSpecification(context);
  };

  return m_module;
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::Initialize(ScMemoryContext * context) noexcept(false)
{
  if (m_checkAbstractAgent)
    m_checkAbstractAgent(context);

  if (m_checkPrimaryInitiationCondition)
    m_checkPrimaryInitiationCondition(context);

  if (m_checkActionClass)
    m_checkActionClass(context);

  if (m_checkInitiationConditionAndResult)
    m_checkInitiationConditionAndResult(context);

  if (m_resolveSpecification)
    m_resolveSpecification(context);
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::Shutdown(ScMemoryContext *) noexcept(false)
{
}
