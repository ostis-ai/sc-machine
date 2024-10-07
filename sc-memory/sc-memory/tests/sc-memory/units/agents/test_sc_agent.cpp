/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include "test_sc_agent.hpp"

/// --------------------------------------

TestScEvent::TestScEvent()
  : ScElementaryEvent(ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScAddr::Empty, ScType::Unknown, ScAddr::Empty)
{
}

/// --------------------------------------

ScAddr ATestGenerateConnector::GetActionClass() const
{
  return ATestGenerateConnector::generate_connector_action;
}

ScResult ATestGenerateConnector::DoProgram(
    ScEventAfterGenerateConnector<ScType::ConstPermPosArc> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateIncomingArc::GetActionClass() const
{
  return ATestGenerateIncomingArc::generate_incoming_arc_action;
}

ScResult ATestGenerateIncomingArc::DoProgram(
    ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateOutgoingArc::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGenerateOutgoingArc::DoProgram(
    ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateEdge::GetActionClass() const
{
  return ATestGenerateEdge::add_edge_action;
}

ScResult ATestGenerateEdge::DoProgram(ScEventAfterGenerateEdge<ScType::ConstCommonEdge> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateEdgeAsConnector::GetActionClass() const
{
  return ATestGenerateEdge::add_edge_action;
}

ScAddr ATestGenerateEdgeAsConnector::GetInitiationCondition() const
{
  return initiationCondition;
}

ScResult ATestGenerateEdgeAsConnector::DoProgram(
    ScEventAfterGenerateConnector<ScType::ConstCommonEdge> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseConnector::GetActionClass() const
{
  return ATestEraseConnector::erase_connector_action;
}

ScResult ATestEraseConnector::DoProgram(ScEventBeforeEraseConnector<ScType::ConstPermPosArc> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseIncomingArc::GetActionClass() const
{
  return ATestEraseIncomingArc::erase_incoming_arc_action;
}

ScResult ATestEraseIncomingArc::DoProgram(
    ScEventBeforeEraseIncomingArc<ScType::ConstPermPosArc> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseOutgoingArc::GetActionClass() const
{
  return ATestEraseOutgoingArc::erase_outgoing_arc_action;
}

ScResult ATestEraseOutgoingArc::DoProgram(
    ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseEdge::GetActionClass() const
{
  return ATestEraseEdge::erase_edge_action;
}

ScResult ATestEraseEdge::DoProgram(ScEventBeforeEraseEdge<ScType::ConstCommonEdge> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseElement::GetActionClass() const
{
  return ATestEraseElement::erase_element_action;
}

ScResult ATestEraseElement::DoProgram(ScEventBeforeEraseElement const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestChangeLinkContent::GetActionClass() const
{
  return ATestChangeLinkContent::content_change_action;
}

ScResult ATestChangeLinkContent::DoProgram(ScEventBeforeChangeLinkContent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateMultipleOutputArc::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGenerateMultipleOutputArc::DoProgram(
    ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestDoProgramOne::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestDoProgramOne::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestDoProgramTwo::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestDoProgramTwo::DoProgram(ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestDoProgram::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScAddr ATestDoProgram::GetAbstractAgent() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;  // invalid abstract agent
}

ScResult ATestDoProgram::DoProgram(ScAction & action)
{
  GetAbstractAgent();

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestException::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestException::DoProgram(ScAction & action)
{
  SC_THROW_EXCEPTION(utils::ScException, "Some exception.");
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseActionWithException::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestEraseActionWithException::DoProgram(ScAction & action)
{
  m_context.EraseElement(action);
  SC_THROW_EXCEPTION(utils::ScException, "Some exception.");
}

/// --------------------------------------

ScAddr ATestCheckResult::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestCheckResult::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
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
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScTemplate ATestGetInitiationConditionTemplate::GetInitiationConditionTemplate(
    ScActionInitiatedEvent const & event) const
{
  ScTemplate initiationCondition;
  initiationCondition.Triple(
      event.GetSubscriptionElement(), ScType::VarPermPosArc, event.GetOtherElement() >> "_action");
  initiationCondition.Triple(GetActionClass(), ScType::VarPermPosArc, "_action");
  initiationCondition.Triple("_action", ScType::VarPermPosArc, ScType::VarNode);
  return initiationCondition;
}

ScResult ATestGetInitiationConditionTemplate::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScAddr ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement::GetInitiationCondition() const
{
  return initiationCondition;
}

ScResult ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement::DoProgram(
    ScActionInitiatedEvent const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScAddr ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple::GetInitiationCondition() const
{
  return initiationCondition;
}

ScResult ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple::DoProgram(
    ScActionInitiatedEvent const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetInitiationConditionTemplateHasEventTripleTwice::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScAddr ATestGetInitiationConditionTemplateHasEventTripleTwice::GetInitiationCondition() const
{
  return initiationCondition;
}

ScResult ATestGetInitiationConditionTemplateHasEventTripleTwice::DoProgram(
    ScActionInitiatedEvent const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetInvalidInitiationConditionTemplate::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScTemplate ATestGetInvalidInitiationConditionTemplate::GetInitiationConditionTemplate(
    ScActionInitiatedEvent const & event) const
{
  ScTemplate templ;
  templ.Triple(ScAddr::Empty, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

ScResult ATestGetInvalidInitiationConditionTemplate::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestCheckInitiationCondition::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

bool ATestCheckInitiationCondition::CheckInitiationCondition(ScActionInitiatedEvent const & event)
{
  return m_context.CheckConnector(
             ATestGenerateOutgoingArc::generate_outgoing_arc_action,
             event.GetArcTargetElement(),
             ScType::ConstPermPosArc)
         && m_context.CreateIterator3(event.GetArcTargetElement(), ScType::ConstPermPosArc, ScType::ConstNode)->Next();
}

ScResult ATestCheckInitiationCondition::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetResultConditionTemplate::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGetResultConditionTemplate::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

ScTemplate ATestGetResultConditionTemplate::GetResultConditionTemplate(
    ScActionInitiatedEvent const & event,
    ScAction & action) const
{
  ScTemplate initiationCondition;
  initiationCondition.Triple(GetActionClass(), ScType::VarPermPosArc, action >> "_action");
  initiationCondition.Triple(action, ScType::VarPermPosArc, ScType::VarNode);
  return initiationCondition;
}

/// --------------------------------------

ScAddr ATestGetInvalidResultConditionTemplate::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGetInvalidResultConditionTemplate::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

ScTemplate ATestGetInvalidResultConditionTemplate::GetResultConditionTemplate(
    ScActionInitiatedEvent const & event,
    ScAction & action) const
{
  ScTemplate templ;
  templ.Triple(ScAddr::Empty, ScType::VarPermPosArc, ScType::VarNode);
  return templ;
}

/// --------------------------------------

ScAddr ATestCheckResultCondition::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestCheckResultCondition::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

bool ATestCheckResultCondition::CheckResultCondition(ScActionInitiatedEvent const &, ScAction & action)
{
  return m_context.CheckConnector(ScKeynodes::action_finished_successfully, action, ScType::ConstPermPosArc)
         && m_context.CreateIterator3(action, ScType::ConstPermPosArc, ScType::ConstNode)->Next();
}

/// --------------------------------------

ScAddr ATestActionDeactivated::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestActionDeactivated::DoProgram(ScActionInitiatedEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}
