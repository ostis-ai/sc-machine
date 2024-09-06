/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_module.hpp"

#include "sc_agent_builder.hpp"

template <class TScAgent, class... TScAddr, typename>
ScModule * ScModule::Agent(TScAddr const &... subscriptionAddrs) noexcept
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  m_agents.push_back(
      {nullptr,
       GetAgentSubscribeCallback<TScAgent>(),
       GetAgentUnsubscribeCallback<TScAgent>(),
       {subscriptionAddrs...}});
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScModule::AgentBuilder(ScAddr const & agentImplementationAddr) noexcept
{
  auto * builder = new ScAgentBuilder<TScAgent>(this, agentImplementationAddr);
  m_agents.push_back({builder, GetAgentSubscribeCallback<TScAgent>(), GetAgentUnsubscribeCallback<TScAgent>(), {}});
  return builder;
}

template <class TScAgent, typename>
ScModule * ScModule::Agent() noexcept
{
  m_agents.push_back({nullptr, GetAgentSubscribeCallback<TScAgent>(), GetAgentUnsubscribeCallback<TScAgent>(), {}});
  return this;
}

template <class TScAgent>
ScModule::ScAgentSubscribeCallback ScModule::GetAgentSubscribeCallback() noexcept
{
  return [](ScMemoryContext * context, ScAddr const & agentImplementationAddr, ScAddrVector const & addrs) -> void
  {
    if (context->IsElement(agentImplementationAddr))
      ScAgentManager<TScAgent>::Subscribe(context, agentImplementationAddr);
    else if (!addrs.empty())
    {
      for (ScAddr const & addr : addrs)
        ScAgentManager<TScAgent>::Subscribe(context, agentImplementationAddr, addr);
    }
    else
    {
      ScAgentManager<TScAgent>::Subscribe(context, agentImplementationAddr);
    }
  };
}

template <class TScAgent>
ScModule::ScAgentUnsubscribeCallback ScModule::GetAgentUnsubscribeCallback() noexcept
{
  return [](ScMemoryContext * context, ScAddr const & agentImplementationAddr, ScAddrVector const & addrs) -> void
  {
    if (ScAgentManager<TScAgent>::WasAgentSubscribedToEventOfErasedElementErasing(
            context, TScAgent::template GetName<TScAgent>()))
      return;

    if (context->IsElement(agentImplementationAddr))
      ScAgentManager<TScAgent>::Unsubscribe(context, agentImplementationAddr);
    else if (!addrs.empty())
    {
      for (ScAddr const & addr : addrs)
        ScAgentManager<TScAgent>::Unsubscribe(context, agentImplementationAddr, addr);
    }
    else
    {
      ScAgentManager<TScAgent>::Unsubscribe(context, agentImplementationAddr);
    }
  };
}
