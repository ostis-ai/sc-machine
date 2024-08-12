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

ScAddr ATestRemoveElement::GetActionClass() const
{
  return ATestRemoveElement::erase_element_action;
}

ScResult ATestRemoveElement::DoProgram(ScEventRemoveElement const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestChangeLinkContent::GetActionClass() const
{
  return ATestChangeLinkContent::content_change_action;
}

ScResult ATestChangeLinkContent::DoProgram(ScEventChangeLinkContent const &, ScAction & action)
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

/// --------------------------------------

ScAddr ATestGetInitiationConditionTemplate::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScTemplate ATestGetInitiationConditionTemplate::GetInitiationConditionTemplate() const
{
  ScTemplate initiationCondition;
  initiationCondition.Triple(GetActionClass(), ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action");
  initiationCondition.Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);
  return initiationCondition;
}

ScResult ATestGetInitiationConditionTemplate::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestCheckInitiationCondition::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

sc_bool ATestCheckInitiationCondition::CheckInitiationCondition(ScActionEvent const & event)
{
  return m_memoryCtx.HelperCheckEdge(
             ATestAddOutputArc::add_output_arc_action, event.GetArcTargetElement(), ScType::EdgeAccessConstPosPerm)
         && m_memoryCtx.Iterator3(event.GetArcTargetElement(), ScType::EdgeAccessConstPosPerm, ScType::NodeConst)
                ->Next();
}

ScResult ATestCheckInitiationCondition::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetResultConditionTemplate::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScResult ATestGetResultConditionTemplate::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

ScTemplate ATestGetResultConditionTemplate::GetResultConditionTemplate() const
{
  ScTemplate initiationCondition;
  initiationCondition.Triple(GetActionClass(), ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action");
  initiationCondition.Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);
  return initiationCondition;
}

/// --------------------------------------

ScAddr ATestCheckResultCondition::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScResult ATestCheckResultCondition::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

sc_bool ATestCheckResultCondition::CheckResultCondition(ScActionEvent const & event, ScAction & action)
{
  return m_memoryCtx.HelperCheckEdge(ScKeynodes::action_finished_successfully, action, ScType::EdgeAccessConstPosPerm)
         && m_memoryCtx.Iterator3(event.GetArcTargetElement(), ScType::EdgeAccessConstPosPerm, ScType::NodeConst)
                ->Next();
}

/// --------------------------------------

ScAddr ATestActionDeactivated::GetActionClass() const
{
  return ATestAddOutputArc::add_output_arc_action;
}

ScResult ATestActionDeactivated::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}
