#include "test_sc_agent.hpp"

/// --------------------------------------

ScAddr ATestAddInputArc::GetActionClass() const
{
  return ATestAddInputArc::add_input_arc_action;
}

sc_result ATestAddInputArc::DoProgram(ScEventAddInputArc const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestAddOutputArc::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

sc_result ATestAddOutputArc::DoProgram(ScEventAddOutputArc const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestRemoveInputArc::GetActionClass() const
{
  return ATestRemoveInputArc::remove_input_arc_action;
}

sc_result ATestRemoveInputArc::DoProgram(ScEventRemoveInputArc const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestRemoveOutputArc::GetActionClass() const
{
  return ATestRemoveOutputArc::remove_output_arc_action;
}

sc_result ATestRemoveOutputArc::DoProgram(ScEventRemoveOutputArc const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestEraseElement::GetActionClass() const
{
  return ATestEraseElement::erase_element_action;
}

sc_result ATestEraseElement::DoProgram(ScEventEraseElement const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestChangeContent::GetActionClass() const
{
  return ATestChangeContent::content_change_action;
}

sc_result ATestChangeContent::DoProgram(ScEventChangeContent const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestAddMultipleOutputArc::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

sc_result ATestAddMultipleOutputArc::DoProgram(ScEventAddOutputArc const &, ScAction &)
{
  msWaiter.Unlock();
  return SC_RESULT_OK;
}

/// --------------------------------------

ScAddr ATestCheckResult::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
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
