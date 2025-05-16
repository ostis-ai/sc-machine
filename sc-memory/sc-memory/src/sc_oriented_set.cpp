#include "sc-memory/sc_oriented_set.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_keynodes.hpp"

ScOrientedSet::ScOrientedSet(ScMemoryContext * context, ScAddr const & setAddr)
  : ScSet(context, setAddr)
{
  Reset();
}

ScOrientedSet::~ScOrientedSet() = default;

bool ScOrientedSet::Append(ScAddr const & elementAddr, ScType const & arcType)
{
  AppendElement(elementAddr, arcType);
  return true;
}

bool ScOrientedSet::Append(ScAddr const & elementAddr, ScAddr const & roleAddr, ScType const & arcType)
{
  ScAddr const arcToNewElementAddr = AppendElement(elementAddr, arcType);
  m_context->GenerateConnector(arcType, roleAddr, arcToNewElementAddr);
  return true;
}

ScAddr ScOrientedSet::AppendElement(ScAddr const & elementAddr, ScType const & arcType)
{
  ScIterator5Ptr const elementsIterator = m_context->CreateIterator5(
      *this, ScType::ConstPosArc, ScType::Unknown, ScType::ConstPosArc, ScKeynodes::rrel_last);
  ScAddr arcToPreviousElementAddr;
  if (elementsIterator->Next())
  {
    arcToPreviousElementAddr = elementsIterator->Get(1);
    m_context->EraseElement(elementsIterator->Get(3));
  }

  ScAddr const & arcToNewElementAddr = m_context->GenerateConnector(arcType, *this, elementAddr);
  m_context->GenerateConnector(ScType::ConstActualTempPosArc, ScKeynodes::rrel_last, arcToNewElementAddr);
  if (arcToPreviousElementAddr.IsValid())
  {
    ScAddr const & arcAddr =
        m_context->GenerateConnector(ScType::ConstCommonArc, arcToPreviousElementAddr, arcToNewElementAddr);
    m_context->GenerateConnector(arcType, ScKeynodes::nrel_basic_sequence, arcAddr);
  }
  else
  {
    m_context->GenerateConnector(arcType, ScKeynodes::rrel_1, arcToNewElementAddr);
  }

  return arcToNewElementAddr;
}

ScAddr ScOrientedSet::Next(ScAddrSet & roles) const
{
  if (!m_currentArcAddr.IsValid())
    return ScAddr::Empty;

  ScAddr const & elementAddr = m_context->GetArcTargetElement(m_currentArcAddr);

  ScIterator3Ptr const rolesIterator =
      m_context->CreateIterator3(ScType::ConstNode, ScType::ConstPosArc, m_currentArcAddr);
  while (rolesIterator->Next())
  {
    roles.insert(rolesIterator->Get(0));
  }

  ScIterator5Ptr const nextArcIterator = m_context->CreateIterator5(
      m_currentArcAddr,
      ScType::ConstCommonArc,
      ScType::ConstPosArc,
      ScType::ConstPosArc,
      ScKeynodes::nrel_basic_sequence);
  m_currentArcAddr = nextArcIterator->Next() ? nextArcIterator->Get(2) : ScAddr::Empty;
  return elementAddr;
}

ScAddr ScOrientedSet::Next() const
{
  ScAddrSet roles;
  return Next(roles);
}

void ScOrientedSet::Reset()
{
  m_currentArcAddr = ScAddr::Empty;
  ScIterator3Ptr firstArcIterator = m_context->CreateIterator3(*this, ScType::ConstPosArc, ScType::Unknown);
  while (firstArcIterator->Next())
  {
    ScAddr const & arcAddr = firstArcIterator->Get(1);
    if (m_context->CheckConnector(ScKeynodes::rrel_1, arcAddr, ScType::ConstPermPosArc))
    {
      m_currentArcAddr = arcAddr;
      break;
    }
  }
}

ScAddr ScOrientedSet::GetLast() const
{
  ScIterator5Ptr const it = m_context->CreateIterator5(
      *this, ScType::ConstPosArc, ScType::Unknown, ScType::ConstPosArc, ScKeynodes::rrel_last);
  if (it->Next())
    return it->Get(2);

  return ScAddr::Empty;
}
