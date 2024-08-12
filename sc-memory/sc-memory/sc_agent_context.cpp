/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include <algorithm>

#include "sc_action.hpp"
#include "sc_keynodes.hpp"

ScAgentContext::ScAgentContext() noexcept
  : ScAgentContext(ScAddr::Empty)
{
}

ScAgentContext::ScAgentContext(sc_memory_context * context) noexcept
  : ScMemoryContext(context)
{
}

ScAgentContext::ScAgentContext(ScAddr const & userAddr) noexcept
  : ScMemoryContext(userAddr)
{
}

ScAgentContext::ScAgentContext(ScAgentContext && other) noexcept
  : ScMemoryContext(std::move(other)) {};

ScAgentContext & ScAgentContext::operator=(ScAgentContext && other) noexcept
{
  if (this == &other)
    return *this;

  ScMemoryContext::operator=(std::move(other));
  return *this;
}

ScAction ScAgentContext::CreateAction(ScAddr const & actionClassAddr)
{
  ScAction action{this, actionClassAddr};
  return action;
}

ScAction ScAgentContext::UseAction(ScAddr const & actionAddr)
{
  ScAction action{this, actionAddr, ScAddr::Empty};
  return action;
}

ScSet ScAgentContext::CreateSet()
{
  ScSet set{this};
  return set;
}

ScSet ScAgentContext::UseSet(ScAddr const & setAddr)
{
  ScSet set{this, setAddr};
  return set;
}

ScStruct ScAgentContext::CreateStructure()
{
  ScStruct structure{this};
  return structure;
}

ScStruct ScAgentContext::UseStructure(ScAddr const & structureAddr)
{
  ScStruct structure{this, structureAddr};
  return structure;
}
