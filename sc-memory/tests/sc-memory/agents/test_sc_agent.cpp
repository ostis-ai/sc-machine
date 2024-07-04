#include "test_sc_agent.hpp"

/// --------------------------------------
ScKeynodeClass const ATestAddInputEdge::msAgentKeynode("ATestAddInputEdge");
TestWaiter ATestAddInputEdge::msWaiter;

sc_result ATestAddInputEdge::OnEvent(ScAddr const & sourceAddr, ScAddr const & connectorAddr, ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestAddOutputEdge::msAgentKeynode("ATestAddOutputEdge");
TestWaiter ATestAddOutputEdge::msWaiter;

sc_result ATestAddOutputEdge::OnEvent(
    ScAddr const & sourceAddr,
    ScAddr const & connectorAddr,
    ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestRemoveInputEdge::msAgentKeynode("ATestRemoveInputEdge");
TestWaiter ATestRemoveInputEdge::msWaiter;

sc_result ATestRemoveInputEdge::OnEvent(
    ScAddr const & sourceAddr,
    ScAddr const & connectorAddr,
    ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestRemoveOutputEdge::msAgentKeynode("ATestRemoveOutputEdge");
TestWaiter ATestRemoveOutputEdge::msWaiter;

sc_result ATestRemoveOutputEdge::OnEvent(
    ScAddr const & sourceAddr,
    ScAddr const & connectorAddr,
    ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestRemoveElement::msAgentKeynode("ATestRemoveElement");
TestWaiter ATestRemoveElement::msWaiter;

sc_result ATestRemoveElement::OnEvent(
    ScAddr const & sourceAddr,
    ScAddr const & connectorAddr,
    ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeLink const ATestContentChanged::msAgentKeynode("ATestContentChanged");
TestWaiter ATestContentChanged::msWaiter;

sc_result ATestContentChanged::OnEvent(
    ScAddr const & sourceAddr,
    ScAddr const & connectorAddr,
    ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestAddMultipleOutputEdge::msAgentKeynode1("ATestAddMultipleOutputEdge1");
ScKeynodeClass const ATestAddMultipleOutputEdge::msAgentKeynode2("ATestAddMultipleOutputEdge2");
TestWaiter ATestAddMultipleOutputEdge::msWaiter;

sc_result ATestAddMultipleOutputEdge::OnEvent(
    ScAddr const & sourceAddr,
    ScAddr const & connectorAddr,
    ScAddr const & targetAddr)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestCheckResult::msAgentKeynode("ATestCheckResult");
ScKeynodeClass const ATestCheckResult::msAgentSet("ATestCheckResultSet");
TestWaiter ATestCheckResult::msWaiter;

sc_result ATestCheckResult::OnEvent(ScAddr const & actionAddr)
{
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

void ATestCheckResult::OnSuccess(ScAddr const & actionAddr)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, msAgentSet, msAgentKeynode);
}

void ATestCheckResult::OnUnsuccess(ScAddr const & actionAddr)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstFuzPerm, msAgentSet, msAgentKeynode);
}

void ATestCheckResult::OnError(ScAddr const & actionAddr, sc_result errorCode)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstNegPerm, msAgentSet, msAgentKeynode);
}
