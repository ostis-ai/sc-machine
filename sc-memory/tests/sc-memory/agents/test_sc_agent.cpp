#include "test_sc_agent.hpp"

/// --------------------------------------

ScAddr ATestAddInputArc::GetActionClass() const
{
  return ATestAddInputArc::add_input_arc_action;
}

ScResult ATestAddInputArc::DoProgram(ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestAddOutputArc::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScResult ATestAddOutputArc::DoProgram(ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestAddEdge::GetActionClass() const
{
  return ATestAddEdge::add_edge_action;
}

ScResult ATestAddEdge::DoProgram(ScEventAddEdge<ScType::EdgeUCommonConst> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestRemoveInputArc::GetActionClass() const
{
  return ATestRemoveInputArc::remove_input_arc_action;
}

ScResult ATestRemoveInputArc::DoProgram(
    ScEventRemoveInputArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestRemoveOutputArc::GetActionClass() const
{
  return ATestRemoveOutputArc::remove_output_arc_action;
}

ScResult ATestRemoveOutputArc::DoProgram(
    ScEventRemoveOutputArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestRemoveEdge::GetActionClass() const
{
  return ATestRemoveEdge::remove_edge_action;
}

ScResult ATestRemoveEdge::DoProgram(ScEventRemoveEdge<ScType::EdgeUCommonConst> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseElement::GetActionClass() const
{
  return ATestEraseElement::erase_element_action;
}

ScResult ATestEraseElement::DoProgram(ScEventEraseElement const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestChangeContent::GetActionClass() const
{
  return ATestChangeContent::content_change_action;
}

ScResult ATestChangeContent::DoProgram(ScEventChangeContent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestAddMultipleOutputArc::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScResult ATestAddMultipleOutputArc::DoProgram(
    ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestCheckResult::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScResult ATestCheckResult::DoProgram(ScActionEvent const &, ScAction & action)
{
  auto [firstArgument, secondArgument] = action.GetArguments<2>();

  if (!firstArgument.IsValid())
  {
    msWaiter.Unlock();
    return action.FinishWithError();
  }

  if (!secondArgument.IsValid())
  {
    msWaiter.Unlock();
    return action.FinishUnsuccessfully();
  }

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}
