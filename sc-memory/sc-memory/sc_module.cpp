/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_module.hpp"

ScModule::~ScModule() = default;

ScModule * ScModule::Create(ScModule * module)
{
  return module;
}

sc_result ScModule::Register(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  SC_LOG_INFO("Initialize " << this->GetName());
  Initialize(ctx, initMemoryGeneratedStructureAddr);

  for (auto * keynodes : m_keynodes)
    keynodes->Initialize(ctx, initMemoryGeneratedStructureAddr);

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
  for (auto * keynodes : m_keynodes)
  {
    keynodes->Shutdown(ctx);
    delete keynodes;
  }
  m_keynodes.clear();

  for (auto const & agentInfo : m_agents)
  {
    ScAgentUnregisterCallback const & unregisterCallback = agentInfo.first.second;
    ScAddrVector const & addrs = agentInfo.second;
    unregisterCallback(ctx, addrs);
  }
  m_agents.clear();

  SC_LOG_INFO("Shutdown " << this->GetName());
  Shutdown(ctx);

  return SC_RESULT_OK;
}

sc_result ScModule::Initialize(ScMemoryContext *, ScAddr const &)
{
  return SC_RESULT_OK;
}

sc_result ScModule::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}
