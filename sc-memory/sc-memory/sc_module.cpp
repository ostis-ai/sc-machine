/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_module.hpp"

#include "sc-memory/sc_keynodes.hpp"

ScModule::~ScModule() noexcept = default;

ScModule * ScModule::Create(ScModule * module) noexcept
{
  return module;
}

void ScModule::Register(ScMemoryContext * context) noexcept(false)
{
  SC_LOG_INFO("Initialize " << this->GetName());

  internal::ScKeynodesRegister::Register(context);

  Initialize(context);

  for (auto const & agentInfo : m_agents)
  {
    auto [builder, subscribeCallback, _, addrs] = agentInfo;
    if (builder != nullptr)
      builder->Initialize(context);
    ScAddr const & agentImplementationAddr = builder ? builder->GetAgentImplementation() : ScAddr::Empty;
    subscribeCallback(context, agentImplementationAddr, addrs);
  }
}

void ScModule::Unregister(ScMemoryContext * context) noexcept(false)
{
  for (auto const & agentInfo : m_agents)
  {
    auto [builder, _, unsubscribeCallback, addrs] = agentInfo;
    ScAddr const & agentImplementationAddr = builder ? builder->GetAgentImplementation() : ScAddr::Empty;
    unsubscribeCallback(context, agentImplementationAddr, addrs);
    if (builder != nullptr)
    {
      builder->Shutdown(context);
      delete builder;
      builder = nullptr;
    }
  }
  m_agents.clear();

  SC_LOG_INFO("Shutdown " << this->GetName());
  Shutdown(context);
}

void ScModule::Initialize(ScMemoryContext *) {}

void ScModule::Shutdown(ScMemoryContext *) {}
