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

sc_result ATestCheckResult::OnEvent(ScActionEvent const & event)
{
  ScAction action = event.GetAction();
  auto [firstArgument, secondArgument] = action.GetArguments<2>();

  if (firstArgument.IsValid() == SC_FALSE)
  {
    msWaiter.Unlock();
    return action.FinishWithError();
  }

  if (secondArgument.IsValid() == SC_FALSE)
  {
    msWaiter.Unlock();
    return action.FinishUnsuccessfully();
  }

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

void ATestCheckResult::OnSuccess(ScActionEvent const & event)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, msAgentSet, event.GetSubscriptionElement());
}

void ATestCheckResult::OnUnsuccess(ScActionEvent const & event)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstFuzPerm, msAgentSet, event.GetSubscriptionElement());
}

void ATestCheckResult::OnError(ScActionEvent const & event, sc_result)
{
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstNegPerm, msAgentSet, event.GetSubscriptionElement());
}
