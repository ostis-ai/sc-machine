/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_builder.hpp"

#include "sc_agent_context.hpp"

#include "sc_keynodes.hpp"

template <class TScAgent>
ScAgentBuilder<TScAgent>::ScAgentBuilder(ScAddr const & agentImplementationAddr)
  : m_module(nullptr)
  , m_agentImplementationAddr(agentImplementationAddr)
{
}

template <class TScAgent>
ScAgentBuilder<TScAgent>::ScAgentBuilder(ScModule * module, ScAddr const & agentImplementationAddr)
  : m_module(module)
  , m_agentImplementationAddr(agentImplementationAddr)
{
}

template <class TScAgent>
ScAddr ScAgentBuilder<TScAgent>::GetAgentImplementation() const
{
  return m_agentImplementationAddr;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetAbstractAgent(ScAddr const & abstractAgentAddr)
{
  m_initializeAbstractAgent = [this](ScMemoryContext * ctx)
  {
    if (!ctx->IsElement(m_abstractAgentAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified abstract agent for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    ScIterator3Ptr it3 =
        ctx->Iterator3(ScKeynodes::abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_abstractAgentAddr);
    if (!it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-element for agent class `"
              << TScAgent::template GetName<TScAgent>()
              << "` is not abstract agent due it does not belong to class `abstract_sc_agent`.");
  };

  m_abstractAgentAddr = abstractAgentAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetPrimaryInitiationCondition(
    std::tuple<ScAddr, ScAddr> const & primaryInitiationCondition)
{
  auto [eventClassAddr, eventSubscriptionElementAddr] = primaryInitiationCondition;

  m_initializePrimaryInitiationCondition = [this](ScMemoryContext * ctx)
  {
    if (!ctx->IsElement(m_eventClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified sc-event class for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    if (!ctx->IsElement(m_eventSubscriptionElementAddr))
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
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetActionClass(ScAddr const & actionClassAddr)
{
  m_initializeActionClass = [this](ScMemoryContext * ctx)
  {
    if (!ctx->IsElement(m_actionClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified action class for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");
  };

  m_actionClassAddr = actionClassAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetInitiationConditionAndResult(
    std::tuple<ScAddr, ScAddr> const & initiationCondition)
{
  auto [initiationConditionAddr, resultConditionAddr] = initiationCondition;

  m_initializeInitiationConditionAndResult = [this](ScMemoryContext * ctx)
  {
    if (!ctx->IsElement(m_initiationConditionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Specified initiation condition template for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                      << "` is not valid.");

    if (!ctx->IsElement(m_resultConditionAddr))
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
void ScAgentBuilder<TScAgent>::LoadSpecification(ScMemoryContext * ctx)
{
  std::string const & agentClassName = TScAgent::template GetName<TScAgent>();
  std::string agentImplementationName;

  if (m_agentImplementationAddr.IsValid())
  {
    agentImplementationName = ctx->HelperGetSystemIdtf(m_agentImplementationAddr);

    ScIterator3Ptr const it3 = ctx->Iterator3(
        ScKeynodes::platform_dependent_abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_agentImplementationAddr);
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

    SC_LOG_WARNING("Agent implementation for agent class `" << agentClassName << "` was found.");
  }
  else
  {
    agentImplementationName = agentClassName;

    m_agentImplementationAddr = ctx->HelperFindBySystemIdtf(agentClassName);
    if (m_agentImplementationAddr.IsValid())
    {
      ScIterator3Ptr const it3 = ctx->Iterator3(
          ScKeynodes::platform_dependent_abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_agentImplementationAddr);
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

      SC_LOG_WARNING(
          "Agent implementation for class `" << agentClassName << "` was not generated due it already exists.");
    }
    else
    {
      m_agentImplementationAddr = ctx->CreateNode(ScType::NodeConst);
      ctx->HelperSetSystemIdtf(agentClassName, m_agentImplementationAddr);
      ctx->CreateEdge(
          ScType::EdgeAccessConstPosPerm, ScKeynodes::platform_dependent_abstract_sc_agent, m_agentImplementationAddr);
      SC_LOG_WARNING("Agent implementation for class `" << agentClassName << "` was generated.");
    }
  }

  if (m_abstractAgentAddr.IsValid())
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        m_agentImplementationAddr,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_inclusion);
    if (it5->Next())
      SC_LOG_WARNING(
          "Connection between specified abstract agent and agent implementation for class `"
          << agentClassName << "` was not generated due it already exists.");
    else
    {
      ScAddr const & arcAddr =
          ctx->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, m_agentImplementationAddr);
      ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
      SC_LOG_WARNING(
          "Connection between specified abstract agent and agent implementation for class `" << agentClassName
                                                                                             << "` was generated.");
    }
  }
  else
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
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

    if (ctx->GetElementType(m_abstractAgentAddr).BitAnd(ScType::NodeConst) != ScType::NodeConst)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
              << agentImplementationName << "` because it has not sc-type `ScType::NodeConst`.");

    ScIterator3Ptr const it3 =
        ctx->Iterator3(ScKeynodes::abstract_sc_agent, ScType::EdgeAccessConstPosPerm, m_abstractAgentAddr);
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

    SC_LOG_WARNING("Abstract agent for agent class `" << agentClassName << "` was found.");
  }

  std::string abstractAgentName = ctx->HelperGetSystemIdtf(m_abstractAgentAddr);
  if (abstractAgentName.empty())
    abstractAgentName = std::to_string(m_abstractAgentAddr.Hash());

  if (m_eventClassAddr.IsValid() && m_eventSubscriptionElementAddr.IsValid())
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        ScType::EdgeDCommonConst,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_primary_initiation_condition);
    if (it5->Next())
      SC_LOG_WARNING(
          "Primary initiation condition for class `" << agentClassName << "` was not generated due it already exists.");
    else
    {
      ScAddr const & primaryConditionAddr =
          ctx->CreateEdge(ScType::EdgeDCommonConst, m_eventClassAddr, m_eventSubscriptionElementAddr);
      ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, primaryConditionAddr);
      ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_primary_initiation_condition, arcAddr);
      SC_LOG_WARNING("Primary initiation condition for class  `" << agentClassName << "` was generated.");
    }
  }
  else
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
        m_abstractAgentAddr,
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

    if (ctx->GetElementType(primaryInitiationConditionAddr).BitAnd(ScType::EdgeDCommonConst)
        != ScType::EdgeDCommonConst)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Found sc-element by relation `nrel_primary_initiation_condition` is not primary initiation condition for "
          "abstract sc-agent `"
              << abstractAgentName << "` beacuse it has not sc-type `ScType::EdgeDCommonConst`.");

    ctx->GetEdgeInfo(primaryInitiationConditionAddr, m_eventClassAddr, m_eventSubscriptionElementAddr);

    ScIterator3Ptr const it3 = ctx->Iterator3(ScKeynodes::sc_event, ScType::EdgeAccessConstPosPerm, m_eventClassAddr);
    if (!it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "The first element of primary initiation condition for abstract sc-agent `"
              << abstractAgentName << "` is not sc-event class because it doesn't belong to class `sc_event`.");

    if (it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Found sc-event class for abstract sc-agent `" << abstractAgentName
                                                         << "` belongs to class `sc_event` twice.");

    SC_LOG_WARNING("Primary initiation condition for agent class `" << agentClassName << "` was found.");
  }

  if (m_actionClassAddr.IsValid())
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        m_actionClassAddr,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_sc_agent_action_class);
    if (it5->Next())
      SC_LOG_WARNING(
          "Connection between specified abstract agent and action class for agent class `"
          << agentClassName << "` was not generated due it already exists.");
    else
    {
      ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, m_actionClassAddr);
      ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_sc_agent_action_class, arcAddr);
      SC_LOG_WARNING(
          "Connection between specified abstract agent and action class for agent class `" << agentClassName
                                                                                           << "` was generated.");
    }
  }
  else
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
        m_abstractAgentAddr,
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

    m_actionClassAddr = it5->Get(2);

    if (it5->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Abstract sc-agent `" << abstractAgentName << "` has two or more action classes.");

    if (ctx->GetElementType(m_actionClassAddr).BitAnd(ScType::NodeConstClass) != ScType::NodeConstClass)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Found sc-element by relation `nrel_sc_agent_action_class` is not action class for abstract sc-agent `"
              << abstractAgentName << "` beacuse it has not sc-type `ScType::NodeConstClass`.");

    SC_LOG_WARNING("Action class for agent class `" << agentClassName << "` was found.");
  }

  if (m_initiationConditionAddr.IsValid() && m_resultConditionAddr.IsValid())
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
        m_abstractAgentAddr,
        ScType::EdgeDCommonConst,
        ScType::EdgeDCommonConst,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::nrel_initiation_condition_and_result);
    if (it5->Next())
      SC_LOG_WARNING(
          "Initiation condition and result for class `" << agentClassName
                                                        << "` was not generated due it already exists.");
    else
    {
      ScAddr const & conditionAndResultAddr =
          ctx->CreateEdge(ScType::EdgeDCommonConst, m_initiationConditionAddr, m_resultConditionAddr);
      ScAddr const & arcAddr = ctx->CreateEdge(ScType::EdgeDCommonConst, m_abstractAgentAddr, conditionAndResultAddr);
      ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_initiation_condition_and_result, arcAddr);
      SC_LOG_WARNING("Initiation condition and result for class  `" << agentClassName << "` was generated.");
    }
  }
  else
  {
    ScIterator5Ptr const it5 = ctx->Iterator5(
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

    ctx->GetEdgeInfo(initiationConditionAndResultAddr, m_initiationConditionAddr, m_resultConditionAddr);
    if (ctx->GetElementType(m_initiationConditionAddr).BitAnd(ScType::NodeConstStruct) != ScType::NodeConstStruct)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Initiation condition for abstract sc-agent `"
              << abstractAgentName << "` is not sc-template because it has not sc-type `ScType::NodeConstStruct`.");

    if (ctx->GetElementType(m_resultConditionAddr).BitAnd(ScType::NodeConstStruct) != ScType::NodeConstStruct)
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Result condition for abstract sc-agent `"
              << abstractAgentName << "` is not sc-template because it has not sc-type `ScType::NodeConstStruct`.");

    SC_LOG_WARNING("Initiation condition and result for agent class `" << agentClassName << "` was found.");
  }
}

template <class TScAgent>
ScModule * ScAgentBuilder<TScAgent>::FinishBuild()
{
  m_initializeSpecification = [this](ScMemoryContext * ctx)
  {
    LoadSpecification(ctx);
  };

  return m_module;
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::Initialize(ScMemoryContext * ctx)
{
  if (m_initializeAbstractAgent)
    m_initializeAbstractAgent(ctx);

  if (m_initializePrimaryInitiationCondition)
    m_initializePrimaryInitiationCondition(ctx);

  if (m_initializeActionClass)
    m_initializeActionClass(ctx);

  if (m_initializeInitiationConditionAndResult)
    m_initializeInitiationConditionAndResult(ctx);

  if (m_initializeSpecification)
    m_initializeSpecification(ctx);
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::Shutdown(ScMemoryContext *)
{
}
