#include "test_sc_agent.hpp"

/// --------------------------------------

sc_result ATestAddInputEdge::OnEvent(ScEventAddInputEdge const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestAddOutputEdge::OnEvent(ScEventAddOutputEdge const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestRemoveInputEdge::OnEvent(ScEventRemoveInputEdge const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestRemoveOutputEdge::OnEvent(ScEventRemoveOutputEdge const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestRemoveElement::OnEvent(ScEventRemoveElement const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestContentChanged::OnEvent(ScEventChangeContent const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestAddMultipleOutputEdge::OnEvent(ScEventAddOutputEdge const &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

sc_result ATestCheckResult::OnEvent(ScEventAddOutputEdge const & event)
{
  ScAddr const & actionAddr = event.GetOtherElement();

  ScAddr const & firstArgument = m_memoryCtx.GetActionArgument(actionAddr, 1);
  ScAddr const & secondArgument = m_memoryCtx.GetActionArgument(actionAddr, 2);

  if (firstArgument.IsValid() == SC_FALSE)
  {
    msWaiter.Unlock();
    return SC_RESULT_ERROR;
  }

  if (secondArgument.IsValid() == SC_FALSE)
  {
    msWaiter.Unlock();
    return SC_RESULT_NO;
  }

  msWaiter.Unlock();
  return SC_RESULT_OK;
}

void ATestCheckResult::OnSuccess(ScEventAddOutputEdge const &)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, msAgentSet, ATestAddOutputEdge::add_output_edge_action);
}

void ATestCheckResult::OnUnsuccess(ScEventAddOutputEdge const &)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstFuzPerm, msAgentSet, ATestAddOutputEdge::add_output_edge_action);
}

void ATestCheckResult::OnError(ScEventAddOutputEdge const &, sc_result)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstNegPerm, msAgentSet, ATestAddOutputEdge::add_output_edge_action);
}
