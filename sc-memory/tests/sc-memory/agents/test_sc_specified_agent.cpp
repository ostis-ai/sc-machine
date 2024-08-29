#include "test_sc_specified_agent.hpp"

/// --------------------------------------

ScResult ATestSpecifiedAgent::DoProgram(ScElementaryEvent const &, ScAction & action)
{
  ScAddr const & nodeAddr2 = m_context.CreateNode(ScType::NodeConst);
  ScAddr const & arcAddr =
      m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, ATestSpecifiedAgent::concept_set, nodeAddr2);

  action.FormResult(ATestSpecifiedAgent::concept_set, nodeAddr2, arcAddr);

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------
