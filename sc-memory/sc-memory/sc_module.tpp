/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_module.hpp"

template <class TKeynodesClass>
ScModule * ScModule::Keynodes()
{
  static_assert(
      std::is_base_of<ScKeynodes, TKeynodesClass>::value, "TKeynodesClass must be derivied from ScKeynodes class.");

  m_keynodes.push_back(new TKeynodesClass());
  return this;
}

template <class TScAgent, class... TScAddr>
ScModule * ScModule::Agent(TScAddr const &... addrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  m_agents.push_back({{GetAgentRegisterCallback<TScAgent>(), GetAgentUnregisterCallback<TScAgent>()}, {addrs...}});
  return this;
}

template <class TScAgent>
ScModule::ScAgentRegisterCallback ScModule::GetAgentRegisterCallback()
{
  return [](ScMemoryContext * ctx, ScAddrVector const & addrs)
  {
    for (ScAddr const & addr : addrs)
      TScAgent::template Register<TScAgent>(ctx, addr);
  };
}

template <class TScAgent>
ScModule::ScAgentUnregisterCallback ScModule::GetAgentUnregisterCallback()
{
  return [](ScMemoryContext * ctx, ScAddrVector const & addrs)
  {
    for (ScAddr const & addr : addrs)
      TScAgent::template Unregister<TScAgent>(ctx, addr);
  };
}
