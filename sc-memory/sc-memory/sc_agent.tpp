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
        "Abstract sc-agent `"
            << this->GetName()
            << "` has not primary initiation condition and result. Check that abstract sc-agent has specified "
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
        "Abstract sc-agent `"
            << this->GetName()
            << "` has not primary initiation condition and result. Check that abstract sc-agent has specified "
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
                              << "` has not action class. Check that abstract sc-agent has specified "
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
        "Abstract sc-agent `"
            << this->GetName()
            << "` has not initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_memoryCtx.GetEdgeSource(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScTemplate ScAgentAbstract<TScEvent, TScContext>::GetInitiationConditionTemplate() const
{
  return ScTemplate();
}

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
        "Abstract sc-agent `"
            << this->GetName()
            << "` has not initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_memoryCtx.GetEdgeTarget(it5->Get(2));
}

template <class TScEvent, class TScContext>
ScTemplate ScAgentAbstract<TScEvent, TScContext>::GetResultConditionTemplate() const
{
  return ScTemplate();
}

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
                                          << "due subscription sc-element with address `"
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
        utils::ExceptionInvalidState, "Agent `" << agentName << "` has not been subscribed to any events yet.");

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
                                              << "due subscription sc-element with address `"
                                              << subscriptionElementAddr.Hash() << "` is not valid.");
    }

    auto const & it = subscriptionsMap.find(subscriptionElementAddr);
    if (it == subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` has not been subscribed to event `" << eventClassName << "("
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
      should_be_no_more_than_one_override_initiation_condition_method_for<TScAgent>::value,
      "TScAgent must have no more than one override method from GetInitiationCondition, GetInitiationConditionTemplate "
      "and CheckInitiationCondition.");

  static_assert(
      should_be_no_more_than_one_override_result_condition_method_for<TScAgent>::value,
      "TScAgent must have no more than one override method from GetResultCondition, GetResultConditionTemplate and "
      "CheckResultCondition.");

  return [agentImplementationAddr](TScEvent const & event) -> void
  {
    auto const & CreateAction = [](TScEvent const & event, ScAgent & agent) -> ScAction
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
    if (ScMemory::ms_globalContext->HelperCheckEdge(
            ScKeynodes::action_deactivated, agent.GetActionClass(), ScType::EdgeAccessConstPosPerm))
    {
      SC_LOG_WARNING(
          "Agent `" << agentName << "` was not started due actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return;
    }

    ScAction action = CreateAction(event, agent);

    if constexpr (std::is_same<
                      decltype(&TScAgent::CheckInitiationCondition),
                      decltype(&ScAgent::CheckInitiationCondition)>::value)
    {
      ScTemplate initiationConditionTemplate;
      if constexpr (std::is_same<
                        decltype(&TScAgent::GetInitiationConditionTemplate),
                        decltype(&ScAgent::GetInitiationConditionTemplate)>::value)
      {
        if (agent.MayBeSpecified())
        {
          ScAddr const & initiationConditionAddr = agent.GetInitiationCondition();
          initiationConditionTemplate = agent.BuildCheckTemplate(event, initiationConditionAddr);
        }
      }
      else
      {
        initiationConditionTemplate = agent.GetInitiationConditionTemplate();
      }

      ScTemplateSearchResult searchResult;
      if (!initiationConditionTemplate.IsEmpty()
          && !agent.m_memoryCtx.HelperSearchTemplate(initiationConditionTemplate, searchResult))
      {
        SC_LOG_WARNING(
            "Agent `" << agentName << "` was finished due its initiation condition was tested unsuccessfully.");
        return;
      }
    }
    else
    {
      if (!agent.CheckInitiationCondition(event))
      {
        SC_LOG_WARNING(
            "Agent `" << agentName << "` was finished due its initiation condition was tested unsuccessfully.");
        return;
      }
    }

    ScResult result = agent.DoProgram(event, action);

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agentName << "` finished successfully");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agentName << "` finished unsuccessfully");
    else
      SC_LOG_INFO("Agent `" << agentName << "` finished with error");

    if constexpr (std::is_same<decltype(&TScAgent::CheckResultCondition), decltype(&ScAgent::CheckResultCondition)>::
                      value)
    {
      ScTemplate resultConditionTemplate;
      if constexpr (std::is_same<
                        decltype(&TScAgent::GetResultConditionTemplate),
                        decltype(&ScAgent::GetResultConditionTemplate)>::value)
      {
        if (agent.MayBeSpecified())
        {
          ScAddr const & resultConditionAddr = agent.GetResultCondition();
          resultConditionTemplate = agent.BuildCheckTemplate(event, resultConditionAddr);
        }
      }
      else
      {
        resultConditionTemplate = agent.GetResultConditionTemplate();
      }

      ScTemplateSearchResult searchResult;
      if (!resultConditionTemplate.IsEmpty()
          && !agent.m_memoryCtx.HelperSearchTemplate(resultConditionTemplate, searchResult))
      {
        SC_LOG_WARNING("Result condition of agent `" << agentName << "` tested unsuccessfully.");
        return;
      }
    }
    else
    {
      if (!agent.CheckResultCondition(event, action))
      {
        SC_LOG_WARNING("Result condition of agent `" << agentName << "` tested unsuccessfully.");
        return;
      }
    }

    return;
  };
}

template <class TScEvent, class TScContext>
ScTemplate ScAgent<TScEvent, TScContext>::BuildCheckTemplate(TScEvent const & event, ScAddr const & checkTemplateAddr)
{
  ScAddr const & eventClassAddr = event.GetEventClass();
  auto [eventSubscriptionElement, _, otherElementAddr] = event.GetTriple();

  ScIterator5Ptr it5;
  size_t otherVarPosition = 0u;
  if (eventClassAddr == ScKeynodes::sc_event_generate_incoming_arc
      || eventClassAddr == ScKeynodes::sc_event_erase_incoming_arc)
  {
    it5 = this->m_memoryCtx.Iterator5(
        ScType::Unknown, ScType::Var, eventSubscriptionElement, ScType::EdgeAccessConstPosPerm, checkTemplateAddr);
  }
  else if (
      eventClassAddr == ScKeynodes::sc_event_generate_outgoing_arc
      || eventClassAddr == ScKeynodes::sc_event_erase_outgoing_arc
      || eventClassAddr == ScKeynodes::sc_event_generate_edge || eventClassAddr == ScKeynodes::sc_event_erase_edge)
  {
    it5 = this->m_memoryCtx.Iterator5(
        eventSubscriptionElement, ScType::Var, ScType::Unknown, ScType::EdgeAccessConstPosPerm, checkTemplateAddr);

    otherVarPosition = 2u;
  }

  ScTemplateParams params;
  if (it5 && it5->IsValid())
  {
    ScIterator3Ptr const it3 =
        this->m_memoryCtx.Iterator3(checkTemplateAddr, ScType::EdgeAccessConstPosPerm, eventSubscriptionElement);
    if (it3->Next())
    {
      if (it5->Next())
      {
        ScAddr const & otherVarAddr = it5->Get(otherVarPosition);
        if (this->m_memoryCtx.GetElementType(otherVarAddr).IsVar() && otherVarAddr != otherElementAddr)
          params.Add(it5->Get(otherVarPosition), otherElementAddr);
      }
      else
      {
        SC_LOG_WARNING(
            "Initiation condition (or result) template of agent class `"
            << this->GetName()
            << "` checks initiated sc-event incorrectly. May be types of sc-elements "
               "in initiation condition or result template are not correct. Check it.");
      }
    }
    else
    {
      SC_LOG_WARNING(
          "Initiation condition (or result) template of agent class `"
          << this->GetName()
          << "` doesn't check initiated sc-event. Check that agent initiation "
             "condition and result templates are correct.");
    }
  }

  ScTemplate templ;
  this->m_memoryCtx.HelperBuildTemplate(templ, checkTemplateAddr, params);
  return templ;
}
