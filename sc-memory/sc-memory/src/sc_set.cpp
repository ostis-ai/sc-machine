/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_set.hpp"

#include "sc-memory/sc_memory.hpp"

#include "sc-memory/sc_keynodes.hpp"

ScSet::ScSet(ScMemoryContext * context, ScAddr const & setAddr)
  : ScAddr(setAddr)
  , m_context(context)
{
}

ScSet::~ScSet() = default;

bool ScSet::Append(ScAddr const & elementAddr, ScType const & arcType)
{
  if (!Has(elementAddr, ScType::ConstPosArc))
    return m_context->GenerateConnector(arcType, *this, elementAddr).IsValid();

  return false;
}

bool ScSet::Append(ScAddr const & elementAddr, ScAddr const & roleAddr, ScType const & arcType)
{
  if (Has(elementAddr, ScType::ConstPosArc))
    return false;

  ScAddr const & arcAddr = m_context->GenerateConnector(arcType, *this, elementAddr);
  m_context->GenerateConnector(arcType, roleAddr, arcAddr);

  return true;
}

bool ScSet::Remove(ScAddr const & elementAddr, ScType const & arcType)
{
  bool isFound = false;
  ScIterator3Ptr const iterator3 = m_context->CreateIterator3(*this, arcType, elementAddr);
  while (iterator3->Next())
  {
    m_context->EraseElement(iterator3->Get(1));
    isFound = true;
  }

  return isFound;
}

bool ScSet::Has(ScAddr const & elementAddr, ScType const & arcType) const
{
  return m_context->CheckConnector(*this, elementAddr, arcType);
}

ScSet & ScSet::operator<<(ScAddr const & elementAddr)
{
  Append(elementAddr);
  return *this;
}

ScSet & ScSet::operator<<(ScTemplateResultItem const & searchResultItem)
{
  size_t const size = searchResultItem.Size();
  for (size_t i = 0; i < size; ++i)
    Append(searchResultItem.m_replacementConstruction[i]);

  return *this;
}

ScSet & ScSet::operator>>(ScAddr const & elementAddr)
{
  Remove(elementAddr);
  return *this;
}

bool ScSet::IsEmpty() const
{
  ScIterator3Ptr const iterator3 = m_context->CreateIterator3(*this, ScType::ConstPosArc, ScType::Unknown);
  return !iterator3->Next();
}

size_t ScSet::GetPower() const
{
  size_t power = 0;
  ScIterator3Ptr const iterator3 = m_context->CreateIterator3(*this, ScType::ConstPosArc, ScType::Unknown);
  while (iterator3->Next())
    power++;
  return power;
}

ScAddr ScSet::Next(ScAddrSet & roles) const
{
  if (!m_elementsIterator || !m_elementsIterator->IsValid())
    m_elementsIterator = m_context->CreateIterator3(*this, ScType::ConstPosArc, ScType::Unknown);

  if (!m_elementsIterator->Next())
    return ScAddr::Empty;

  ScIterator3Ptr const rolesIterator =
      m_context->CreateIterator3(ScType::ConstNode, ScType::ConstPosArc, m_elementsIterator->Get(1));
  while (rolesIterator->Next())
  {
    roles.insert(rolesIterator->Get(0));
  }

  return m_elementsIterator->Get(2);
}

ScAddr ScSet::Next() const
{
  ScAddrSet roles;
  return Next(roles);
}

void ScSet::Reset()
{
  m_elementsIterator = nullptr;
}
