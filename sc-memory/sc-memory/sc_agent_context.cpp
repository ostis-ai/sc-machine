/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <algorithm>

#include "sc_agent_context.hpp"

#include "sc-memory/sc_keynodes.hpp"

ScAgentContext::ScAgentContext() noexcept
  : ScAgentContext(ScAddr::Empty)
{
}

ScAgentContext::ScAgentContext(sc_memory_context * context) noexcept
  : ScMemoryContext(context)
  , m_cache(*this)
{
}

ScAgentContext::ScAgentContext(ScAddr const & userAddr) noexcept
  : ScMemoryContext(userAddr)
  , m_cache(*this)
{
}

ScAgentContext::ScAgentContext(ScAgentContext && other) noexcept
  : ScMemoryContext(std::move(other))
  , m_cache(*this) {};

ScAgentContext & ScAgentContext::operator=(ScAgentContext && other) noexcept
{
  if (this == &other)
    return *this;

  ScMemoryContext::operator=(std::move(other));
  return *this;
}

ScAddr ScAgentContext::GetActionArgument(ScAddr const & actionAddr, sc_uint16 number)
{
  std::stringstream stream;
  stream << "rrel_" << number;

  ScIterator5Ptr const it = Iterator5(
      actionAddr,
      ScType::EdgeAccessConstPosPerm,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      m_cache.GetKeynode(stream.str()));

  if (it->Next())
    return it->Get(2);

  return ScAddr::Empty;
}

ScAddr ScAgentContext::SetActionArgument(ScAddr const & actionAddr, ScAddr const & argumentAddr, sc_uint16 number)
{
  std::stringstream stream;
  stream << "rrel_" << number;
  ScAddr const & rrelAddr = m_cache.GetKeynode(stream.str());

  ScIterator5Ptr const it =
      Iterator5(actionAddr, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, rrelAddr);

  if (it->Next())
    EraseElement(it->Get(2));

  ScAddr const & edge = CreateEdge(ScType::EdgeAccessConstPosPerm, actionAddr, argumentAddr);
  return CreateEdge(ScType::EdgeAccessConstPosPerm, rrelAddr, edge);
}

void ScAgentContext::FormActionAnswer(ScAddr const & actionAddr, ScAddr const & answerAddr)
{
  ScAddr const edge = CreateEdge(ScType::EdgeDCommonConst, actionAddr, answerAddr);
  CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kNrelAnswer, edge);
}
