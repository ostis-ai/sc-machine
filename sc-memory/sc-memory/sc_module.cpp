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

void ScModule::Register(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  SC_LOG_INFO("Initialize " << this->GetName());
  Initialize(ctx, initMemoryGeneratedStructureAddr);

  for (auto * keynodes : m_keynodes)
    keynodes->Initialize(ctx, initMemoryGeneratedStructureAddr);

  for (auto const & agentInfo : m_agents)
  {
    auto [builder, registerCallback, _, addrs] = agentInfo;
    if (builder != nullptr)
      builder->Initialize(ctx, initMemoryGeneratedStructureAddr);
    ScAddr const & agentImplementationAddr = builder ? builder->GetAgentImplementation() : ScAddr::Empty;
    registerCallback(ctx, agentImplementationAddr, addrs);
  }
}

void ScModule::Unregister(ScMemoryContext * ctx)
{
  for (auto * keynodes : m_keynodes)
  {
    keynodes->Shutdown(ctx);
    delete keynodes;
  }
  m_keynodes.clear();

  for (auto const & agentInfo : m_agents)
  {
    auto [builder, _, unregisterCallback, addrs] = agentInfo;
    ScAddr const & agentImplementationAddr = builder ? builder->GetAgentImplementation() : ScAddr::Empty;
    unregisterCallback(ctx, agentImplementationAddr, addrs);
    if (builder != nullptr)
      builder->Shutdown(ctx);

    delete builder;
  }
  m_agents.clear();

  SC_LOG_INFO("Shutdown " << this->GetName());
  Shutdown(ctx);
}

void ScModule::Initialize(ScMemoryContext *, ScAddr const &) {}

void ScModule::Shutdown(ScMemoryContext *) {}
