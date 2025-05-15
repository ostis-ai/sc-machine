/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_set.hpp"

#include "sc-memory/sc_memory.hpp"

template <typename Func>
_SC_EXTERN void ScSet::ForEach(Func func) const
{
  ScIterator3Ptr const iterator = m_context->CreateIterator3(*this, ScType::ConstPosArc, ScType::Unknown);
  while (iterator->Next())
  {
    ScAddr const arcToElementAddr = iterator->Get(1);
    ScAddr const elementAddr = iterator->Get(2);

    bool roleFound = false;
    ScIterator3Ptr const roleIterator =
        m_context->CreateIterator3(ScType::ConstNode, ScType::ConstPosArc, arcToElementAddr);
    while (roleIterator->Next())
    {
      ScAddr const arcFromRoleAddr = roleIterator->Get(1);
      ScAddr const roleAddr = roleIterator->Get(0);

      func(arcToElementAddr, elementAddr, arcFromRoleAddr, roleAddr);
      roleFound = true;
    }

    if (!roleFound)
      func(arcToElementAddr, elementAddr, ScAddr::Empty, ScAddr::Empty);
  }
}