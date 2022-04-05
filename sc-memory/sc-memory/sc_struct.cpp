/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_struct.hpp"
#include "sc_memory.hpp"
#include "sc_template.hpp"

ScSet::ScSet(ScMemoryContext & ctx, ScAddr const & structAddr)
  : m_addr(structAddr)
  , m_context(ctx)
{
}

bool ScSet::Append(ScAddr const & elAddr)
{
  if (!HasElement(elAddr))
    return m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, m_addr, elAddr).IsValid();

  return false;
}

bool ScSet::Append(ScAddr const & elAddr, ScAddr const & attrAddr)
{
  if (!HasElement(elAddr))
  {
    ScAddr const edge = m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, m_addr, elAddr);
    if (edge.IsValid())
    {
      ScAddr const edge2 = m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, attrAddr, edge);
      if (edge2.IsValid())
        return true;

      // cleanup
      m_context.EraseElement(edge);
    }
  }

  return false;
}

bool ScSet::Remove(ScAddr const & elAddr)
{
  bool found = false;
  ScIterator3Ptr iter = m_context.Iterator3(m_addr, ScType::EdgeAccessConstPosPerm, elAddr);
  while (iter->Next())
  {
    m_context.EraseElement(iter->Get(1));
    found = true;
  }

  return found;
}

bool ScSet::HasElement(ScAddr const & elAddr) const
{
  return m_context.HelperCheckEdge(m_addr, elAddr, ScType::EdgeAccessConstPosPerm);
}

ScSet & ScSet::operator<<(ScAddr const & elAddr)
{
  Append(elAddr);
  return *this;
}

ScSet & ScSet::operator<<(ScTemplateGenResult const & res)
{
  size_t const res_num = res.Size();
  for (size_t i = 0; i < res_num; ++i)
    Append(res.m_result[i]);

  return *this;
}

ScSet & ScSet::operator>>(ScAddr const & elAddr)
{
  Remove(elAddr);
  return *this;
}

ScAddr const & ScSet::operator*() const
{
  return m_addr;
}

bool ScSet::IsEmpty() const
{
  ScIterator3Ptr const iter = m_context.Iterator3(m_addr, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  return !iter->Next();
}
