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

bool ScOrientedSet::Remove(ScAddr const & elementAddr, ScType const & arcType)
{
  bool removed = false;

  ScIterator3Ptr arcIt = m_context->CreateIterator3(*this, arcType, elementAddr);
  while (arcIt->Next())
  {
    ScAddr arcToElement = arcIt->Get(1);

    // Find previous and next arcs in the sequence
    ScAddr prevArc, nextArc;

    // Find previous arc: (prevArc -- nrel_basic_sequence --> arcToElement)
    ScIterator5Ptr prevIt = m_context->CreateIterator5(
        ScType::Unknown, ScType::ConstCommonArc, arcToElement, ScType::ConstPosArc, ScKeynodes::nrel_basic_sequence);
    if (prevIt->Next())
      prevArc = prevIt->Get(0);

    // Find next arc: (arcToElement -- nrel_basic_sequence --> nextArc)
    ScIterator5Ptr nextIt = m_context->CreateIterator5(
        arcToElement, ScType::ConstCommonArc, ScType::Unknown, ScType::ConstPosArc, ScKeynodes::nrel_basic_sequence);
    if (nextIt->Next())
      nextArc = nextIt->Get(2);

    // Remove all arcs connecting roles to arcToElement
    ScIterator3Ptr roleIt = m_context->CreateIterator3(ScType::ConstNode, ScType::ConstPosArc, arcToElement);
    while (roleIt->Next())
      m_context->EraseElement(roleIt->Get(1));

    ScIterator3Ptr rel1It = m_context->CreateIterator3(ScKeynodes::rrel_1, ScType::ConstPosArc, arcToElement);
    while (rel1It->Next())
      m_context->EraseElement(rel1It->Get(1));
    ScIterator3Ptr relLastIt = m_context->CreateIterator3(ScKeynodes::rrel_last, ScType::ConstPosArc, arcToElement);
    while (relLastIt->Next())
      m_context->EraseElement(relLastIt->Get(1));

    ScIterator5Ptr seqIt = m_context->CreateIterator5(
        arcToElement, ScType::ConstCommonArc, ScType::Unknown, ScType::ConstPosArc, ScKeynodes::nrel_basic_sequence);
    while (seqIt->Next())
    {
      m_context->EraseElement(seqIt->Get(1));  // Remove common arc
      m_context->EraseElement(seqIt->Get(3));  // Remove nrel_basic_sequence arc
    }
    ScIterator5Ptr seqIt2 = m_context->CreateIterator5(
        ScType::Unknown, ScType::ConstCommonArc, arcToElement, ScType::ConstPosArc, ScKeynodes::nrel_basic_sequence);
    while (seqIt2->Next())
    {
      m_context->EraseElement(seqIt2->Get(1));  // Remove common arc
      m_context->EraseElement(seqIt2->Get(3));  // Remove nrel_basic_sequence arc
    }

    // Remove the arc from set to element
    m_context->EraseElement(arcToElement);

    // Restore sequence: connect prevArc and nextArc if both exist
    if (prevArc.IsValid() && nextArc.IsValid())
    {
      ScAddr commonArc = m_context->GenerateConnector(ScType::ConstCommonArc, prevArc, nextArc);
      m_context->GenerateConnector(arcType, ScKeynodes::nrel_basic_sequence, commonArc);
    }

    // Update rrel_1 (first) and rrel_last (last)
    if (!prevArc.IsValid() && nextArc.IsValid())
    {
      // nextArc is now first
      m_context->GenerateConnector(arcType, ScKeynodes::rrel_1, nextArc);
    }
    if (!nextArc.IsValid() && prevArc.IsValid())
    {
      // prevArc is now last
      m_context->GenerateConnector(ScType::ConstActualTempPosArc, ScKeynodes::rrel_last, prevArc);
    }
    if (!prevArc.IsValid() && !nextArc.IsValid())
    {
      // Set is now empty; nothing to do
    }

    removed = true;
    break;
  }
  return removed;
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
