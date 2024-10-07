/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "test_sc_specified_agent.hpp"

/// --------------------------------------

ScResult ATestSpecifiedAgent::DoProgram(ScElementaryEvent const &, ScAction & action)
{
  ScAddr const & nodeAddr2 = m_context.GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr =
      m_context.GenerateConnector(ScType::ConstPermPosArc, ATestSpecifiedAgent::concept_set, nodeAddr2);

  action.FormResult(ATestSpecifiedAgent::concept_set, nodeAddr2, arcAddr);

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------
