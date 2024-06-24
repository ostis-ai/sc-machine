/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/kpm/sc_module.hpp"

ScModule ScModule::ms_coreModule;

ScModule * ScModule::Keynodes(ScKeynodes * keynodes)
{
  m_keynodes.push_back(keynodes);
  return this;
}

ScModule * ScModule::Agent(std::pair<ScAgentAbstract *, ScAddrVector> const & agentInfo)
{
  m_agents.push_back(agentInfo);
  return this;
}

ScModule * ScModule::Agent(std::pair<ScAgentAbstract *, ScAddr> const & agentInfo)
{
  m_agents.push_back({agentInfo.first, {agentInfo.second}});
  return this;
}

sc_result ScModule::Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  SC_LOG_INFO("Initialize " + GetName());

  for (auto * keynodes : m_keynodes)
  {
    SC_LOG_INFO("Initialize " + keynodes->GetName());
    keynodes->Initialize(ctx, initMemoryGeneratedStructureAddr);
  }

  for (auto const & agentInfo : m_agents)
  {
    ScAgentAbstract * agent = agentInfo.first;
    ScAddrVector const & addrs = agentInfo.second;

    SC_LOG_INFO("Register " + agent->GetName());
    agent->Register(ctx, addrs);
  }

  return SC_RESULT_OK;
}

sc_result ScModule::Initialize(ScMemoryContext * ctx)
{
  return Initialize(ctx, ScAddr::Empty);
}

sc_result ScModule::Shutdown(ScMemoryContext * ctx)
{
  SC_LOG_INFO("Shutdown " + GetName());

  for (auto * keynodes : m_keynodes)
  {
    SC_LOG_INFO("Shutdown " + keynodes->GetName());
    keynodes->Shutdown(ctx);
    delete keynodes;
  }
  m_keynodes.clear();

  for (auto const & agentInfo : m_agents)
  {
    ScAgentAbstract * agent = agentInfo.first;

    SC_LOG_INFO("Unregister " + agent->GetName());
    agent->Unregister(ctx);
    delete agent;
  }
  m_agents.clear();

  return SC_RESULT_OK;
}
