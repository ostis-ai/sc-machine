#include "test_sc_agent.hpp"

/// --------------------------------------

ScAddr ATestAddIncomingArc::GetActionClass() const
{
  return ATestAddIncomingArc::add_incoming_arc_action;
}

ScResult ATestAddIncomingArc::DoProgram(
    ScEventAddIncomingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestAddOutgoingArc::GetActionClass() const
{
  return ATestAddOutgoingArc::add_outgoing_arc_action;
}

ScResult ATestAddOutgoingArc::DoProgram(
    ScEventAddOutgoingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
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

ScAddr ATestEraseIncomingArc::GetActionClass() const
{
  return ATestEraseIncomingArc::remove_incoming_arc_action;
}

ScResult ATestEraseIncomingArc::DoProgram(
    ScEventEraseIncomingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseOutgoingArc::GetActionClass() const
{
  return ATestEraseOutgoingArc::remove_outgoing_arc_action;
}

ScResult ATestEraseOutgoingArc::DoProgram(
    ScEventEraseOutgoingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseEdge::GetActionClass() const
{
  return ATestEraseEdge::remove_edge_action;
}

ScResult ATestEraseEdge::DoProgram(ScEventEraseEdge<ScType::EdgeUCommonConst> const &, ScAction & action)
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
  return ATestAddOutgoingArc::add_outgoing_arc_action;
}

ScResult ATestAddMultipleOutputArc::DoProgram(
    ScEventAddOutgoingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestCheckResult::GetActionClass() const
{
  return ATestAddOutgoingArc::add_outgoing_arc_action;
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
  return ATestAddOutgoingArc::add_outgoing_arc_action;
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
  return ATestAddOutgoingArc::add_outgoing_arc_action;
}

sc_bool ATestCheckInitiationCondition::CheckInitiationCondition(ScActionEvent const & event)
{
  return m_memoryCtx.HelperCheckEdge(
             ATestAddOutgoingArc::add_outgoing_arc_action, event.GetArcTargetElement(), ScType::EdgeAccessConstPosPerm)
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
  return ATestAddOutgoingArc::add_outgoing_arc_action;
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
  return ATestAddOutgoingArc::add_outgoing_arc_action;
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
  return ATestAddOutgoingArc::add_outgoing_arc_action;
}

ScResult ATestActionDeactivated::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}
