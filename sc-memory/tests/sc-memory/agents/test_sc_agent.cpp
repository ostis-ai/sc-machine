#include "test_sc_agent.hpp"

/// --------------------------------------

ScAddr ATestAddInputEdge::GetActionClass() const
{
  return ATestAddInputEdge::add_input_edge_action;
}

sc_result ATestAddInputEdge::DoProgram(ScEventAddInputEdge const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestAddOutputEdge::GetActionClass() const
{
  return ATestAddOutputEdge::add_output_edge_action;
}

sc_result ATestAddOutputEdge::DoProgram(ScEventAddOutputEdge const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestRemoveInputEdge::GetActionClass() const
{
  return ATestRemoveInputEdge::remove_input_edge_action;
}

sc_result ATestRemoveInputEdge::DoProgram(ScEventRemoveInputEdge const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestRemoveOutputEdge::GetActionClass() const
{
  return ATestRemoveOutputEdge::remove_output_edge_action;
}

sc_result ATestRemoveOutputEdge::DoProgram(ScEventRemoveOutputEdge const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestRemoveElement::GetActionClass() const
{
  return ATestRemoveElement::remove_element_action;
}

sc_result ATestRemoveElement::DoProgram(ScEventRemoveElement const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestContentChanged::GetActionClass() const
{
  return ATestContentChanged::content_change_action;
}

sc_result ATestContentChanged::DoProgram(ScEventChangeContent const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestAddMultipleOutputEdge::GetActionClass() const
{
  return ATestAddOutputEdge::add_output_edge_action;
}

sc_result ATestAddMultipleOutputEdge::DoProgram(ScEventAddOutputEdge const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestCheckResult::GetActionClass() const
{
  return ATestAddOutputEdge::add_output_edge_action;
}

sc_result ATestCheckResult::DoProgram(ScActionEvent const &, ScAction & action)
{
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
