/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_addr.hpp"

class ScModule;
class ScMemoryContext;

class ScAgentBuilderAbstract : public ScObject
{
public:
  _SC_EXTERN virtual void Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) = 0;

  _SC_EXTERN virtual void Shutdown(ScMemoryContext * ctx) = 0;
};

template <class TScAgent>
class ScAgentBuilder : public ScAgentBuilderAbstract
{
public:
  _SC_EXTERN ScAgentBuilder * SetAbstractAgent(ScAddr const & abstractAgentAddr);

  _SC_EXTERN ScAgentBuilder * SetEventClass(ScAddr const & eventClassAddr);

  _SC_EXTERN ScAgentBuilder * SetPrimaryInitiationCondition(
      std::tuple<ScAddr, ScAddr> const & primaryInitiationCondition);

  _SC_EXTERN ScAgentBuilder * SetActionClass(ScAddr const & actionClassAddr);

  _SC_EXTERN ScAgentBuilder * SetInitiationConditionAndResult(
      std::tuple<ScAddr, ScAddr> const & initiationConditionAndResult);

  _SC_EXTERN ScModule * FinishBuild();

protected:
  using ScInitializeCallback = std::function<void(ScMemoryContext *)>;
  friend class ScModule;
  template <class ScAgent>
  friend void BuildAndSubscribeAgent(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr);

  ScModule * m_module;

  ScAddr m_agentImplementationAddr;

  ScAddr m_abstractAgentAddr;
  ScInitializeCallback m_initializeAbstractAgent;

  ScAddr m_eventClassAddr;
  ScAddr m_eventSubscriptionElementAddr;
  ScInitializeCallback m_initializePrimaryInitiationCondition;

  ScAddr m_actionClassAddr;
  ScInitializeCallback m_initializeActionClass;

  ScAddr m_initiationConditionAddr;
  ScAddr m_resultConditionAddr;
  ScInitializeCallback m_initializeInitiationConditionAndResult;

  ScInitializeCallback m_initializeSpecification;

  _SC_EXTERN ScAgentBuilder(ScAddr const & agentImplementationAddr);

  _SC_EXTERN ScAgentBuilder(ScModule * module, ScAddr const & agentImplementationAddr);

  _SC_EXTERN void LoadSpecification(ScMemoryContext * ctx);

  _SC_EXTERN void Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;

  _SC_EXTERN void Shutdown(ScMemoryContext * ctx) override;
};

#include "sc_agent_builder.tpp"
