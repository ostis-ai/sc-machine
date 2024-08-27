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

void ScModule::Register(ScMemoryContext * ctx)
{
  SC_LOG_INFO("Initialize " << this->GetName());
  Initialize(ctx);

  for (auto const & agentInfo : m_agents)
  {
    auto [builder, subscribeCallback, _, addrs] = agentInfo;
    if (builder != nullptr)
      builder->Initialize(ctx);
    ScAddr const & agentImplementationAddr = builder ? builder->GetAgentImplementation() : ScAddr::Empty;
    subscribeCallback(ctx, agentImplementationAddr, addrs);
  }
}

void ScModule::Unregister(ScMemoryContext * ctx)
{
  for (auto const & agentInfo : m_agents)
  {
    auto [builder, _, unsubscribeCallback, addrs] = agentInfo;
    ScAddr const & agentImplementationAddr = builder ? builder->GetAgentImplementation() : ScAddr::Empty;
    unsubscribeCallback(ctx, agentImplementationAddr, addrs);
    if (builder != nullptr)
      builder->Shutdown(ctx);

    delete builder;
  }
  m_agents.clear();

  SC_LOG_INFO("Shutdown " << this->GetName());
  Shutdown(ctx);
}

void ScModule::Initialize(ScMemoryContext *) {}

void ScModule::Shutdown(ScMemoryContext *) {}
