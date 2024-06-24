/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent_context.hpp"

#include <algorithm>

#include "sc-memory/sc_keynodes.hpp"

ScAgentContext::ScAgentContext(ScAgentContext const & other)
  : m_cache(*this)
{
  this->m_context = other.m_context;
  this->m_name = other.m_name;
}

ScAgentContext & ScAgentContext::operator=(ScAgentContext const & other)
{
  this->m_context = other.m_context;
  this->m_name = other.m_name;
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

ScAddr ScAgentContext::FormStructure(ScAddrVector const & addrVector, ScAddr answerAddr)
{
  if (answerAddr.IsValid() == SC_FALSE)
    answerAddr = CreateNode(ScType::NodeConstStruct);

  std::for_each(
      addrVector.begin(),
      addrVector.end(),
      [this, &answerAddr](auto const & addr)
      {
        CreateEdge(ScType::EdgeAccessConstPosPerm, answerAddr, addr);
      });

  return answerAddr;
}

void ScAgentContext::FormActionAnswer(ScAddr const & actionAddr, ScAddr const & answerAddr)
{
  ScAddr const edge = CreateEdge(ScType::EdgeDCommonConst, actionAddr, answerAddr);
  CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kNrelAnswer, edge);
}
