/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_module.hpp"

sc_result ScModule::Register(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  SC_LOG_INFO("Initialize " + this->GetName());

  Initialize(ctx, initMemoryGeneratedStructureAddr);

  for (auto * keynodes : m_keynodes)
  {
    SC_LOG_INFO("Initialize " + keynodes->GetName());
    keynodes->Initialize(ctx, initMemoryGeneratedStructureAddr);
  }

  for (auto const & agentInfo : m_agents)
  {
    ScAgentRegisterCallback const & registerCallback = agentInfo.first.first;
    ScAddrVector const & addrs = agentInfo.second;
    registerCallback(ctx, addrs);
  }

  return SC_RESULT_OK;
}

sc_result ScModule::Unregister(ScMemoryContext * ctx)
{
  SC_LOG_INFO("Shutdown " + this->GetName());

  Shutdown(ctx);

  for (auto * keynodes : m_keynodes)
  {
    SC_LOG_INFO("Shutdown " + keynodes->GetName());
    keynodes->Shutdown(ctx);
    delete keynodes;
  }
  m_keynodes.clear();

  for (auto const & agentInfo : m_agents)
  {
    ScAgentUnregisterCallback const & unregisterCallback = agentInfo.first.second;
    unregisterCallback(ctx);
  }
  m_agents.clear();

  return SC_RESULT_OK;
}
