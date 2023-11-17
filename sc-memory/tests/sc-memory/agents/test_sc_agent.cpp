#include "test_sc_agent.hpp"

/// --------------------------------------
ScKeynodeClass const ATestAddInputEdge::msAgentKeynode("ATestAddInputEdge");
TestWaiter ATestAddInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestAddOutputEdge::msAgentKeynode("ATestAddOutputEdge");
TestWaiter ATestAddOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestRemoveInputEdge::msAgentKeynode("ATestRemoveInputEdge");
TestWaiter ATestRemoveInputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveInputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestRemoveOutputEdge::msAgentKeynode("ATestRemoveOutputEdge");
TestWaiter ATestRemoveOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestRemoveElement::msAgentKeynode("ATestRemoveElement");
TestWaiter ATestRemoveElement::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestRemoveElement)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeLink const ATestContentChanged::msAgentKeynode("ATestContentChanged");
TestWaiter ATestContentChanged::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestContentChanged)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestAddMultipleOutputEdge::msAgentKeynode1("ATestAddMultipleOutputEdge1");
ScKeynodeClass const ATestAddMultipleOutputEdge::msAgentKeynode2("ATestAddMultipleOutputEdge2");
TestWaiter ATestAddMultipleOutputEdge::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestAddMultipleOutputEdge)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------
ScKeynodeClass const ATestCheckResult::msAgentKeynode("ATestCheckResult");
ScKeynodeClass const ATestCheckResult::msAgentSet("ATestCheckResultSet");
TestWaiter ATestCheckResult::msWaiter;

SC_AGENT_IMPLEMENTATION(ATestCheckResult)
{
  ScAddr const & firstArgument = m_memoryCtx.GetActionArgument(otherAddr, 1);
  ScAddr const & secondArgument = m_memoryCtx.GetActionArgument(otherAddr, 2);

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

void ATestCheckResult::OnSuccess(const ScAddr & listenAddr, const ScAddr & edgeAddr, const ScAddr & otherAddr)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, msAgentSet, msAgentKeynode);
}

void ATestCheckResult::OnUnsuccess(const ScAddr & listenAddr, const ScAddr & edgeAddr, const ScAddr & otherAddr)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstFuzPerm, msAgentSet, msAgentKeynode);
}

void ATestCheckResult::OnError(
    const ScAddr & listenAddr,
    const ScAddr & edgeAddr,
    const ScAddr & otherAddr,
    sc_result errorCode)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstNegPerm, msAgentSet, msAgentKeynode);
}
