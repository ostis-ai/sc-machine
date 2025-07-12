/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_oriented_set.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_keynodes.hpp"

template <typename Func>
void ScOrientedSet::ForEach(Func func) const
{
  ScAddr arcToElementAddr;
  ScIterator3Ptr firstArcIterator = m_context->CreateIterator3(*this, ScType::ConstPosArc, ScType::Unknown);
  while (firstArcIterator->Next())
  {
    ScAddr const & arcAddr = firstArcIterator->Get(1);
    if (m_context->CheckConnector(ScKeynodes::rrel_1, arcAddr, ScType::ConstPermPosArc))
    {
      arcToElementAddr = arcAddr;
      break;
    }
  }

  while (arcToElementAddr.IsValid())
  {
    ScAddr const elementAddr = m_context->GetArcTargetElement(arcToElementAddr);

    ScAddr arcFromRoleAddr;
    ScAddr roleAddr;
    ScIterator3Ptr const roleIt = m_context->CreateIterator3(ScType::ConstNode, ScType::ConstPosArc, arcToElementAddr);
    if (roleIt->Next())
    {
      arcFromRoleAddr = roleIt->Get(1);
      roleAddr = roleIt->Get(0);
    }

    func(arcToElementAddr, elementAddr, arcFromRoleAddr, roleAddr);

    ScIterator5Ptr const nextArcIt = m_context->CreateIterator5(
        arcToElementAddr,
        ScType::ConstCommonArc,
        ScType::ConstPosArc,
        ScType::ConstPosArc,
        ScKeynodes::nrel_basic_sequence);
    if (nextArcIt->Next())
      arcToElementAddr = nextArcIt->Get(2);
    else
      arcToElementAddr = ScAddr::Empty;
  }
}
