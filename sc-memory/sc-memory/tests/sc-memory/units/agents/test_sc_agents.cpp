/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "agents_test_utils.hpp"

#include <sc-memory/sc_agent.hpp>

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

TEST_F(ScAgentTest, AgentClass)
{
  ScAddr const & testImplementationAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr arcAddr =
      m_ctx->GenerateConnector(ScType::ConstCommonArc, m_ctx->GenerateNode(ScType::ConstNode), testImplementationAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::information_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->GenerateAction(testClassAddr);

  TestScAgent agent;
  agent.SetImplementation(testImplementationAddr);
  EXPECT_THROW(agent.GetActionClass(), utils::ExceptionItemNotFound);
  EXPECT_TRUE(agent.CheckInitiationCondition(TestScEvent()));
  EXPECT_NO_THROW(agent.GetInitiationConditionTemplate(TestScEvent()));
  EXPECT_TRUE(agent.CheckResultCondition(TestScEvent(), action));
  EXPECT_NO_THROW(agent.GetResultConditionTemplate(TestScEvent(), action));

  EXPECT_NO_THROW(agent.DoProgram(TestScEvent(), action));
  EXPECT_NO_THROW(agent.DoProgram(action));
}

TEST_F(ScAgentTest, SubscribeAgentToInvalidSubscriptionElement)
{
  ScAddr subscriptionElementAddr;
  EXPECT_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentTest, SubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_NO_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
  EXPECT_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
}

TEST_F(ScAgentTest, UnsubscribeAgentFromEventForWhichAgentIsNotSubscribed)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_NO_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
  ScAddr const & otherSubscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(
      m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(otherSubscriptionElementAddr), utils::ExceptionInvalidState);

  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
}

TEST_F(ScAgentTest, UnsubscribeNotSubscribedAgent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(
      m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, ATestGenerateConnector)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SubscribeAgent<ATestGenerateConnector>(subscriptionElementAddr);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateConnector::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateConnector>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateIncomingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateIncomingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateOutgoingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SubscribeAgent<ATestGenerateOutgoingArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstPermPosArc, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateOutgoingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateOutgoingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->SubscribeAgent<ATestGenerateEdge>(subscriptionElementAddr);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstCommonEdge, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateEdge::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateEdge>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateEdgeAsConnector)
{
  m_ctx->SubscribeAgent<ATestGenerateEdgeAsConnector>(ATestGenerateEdgeAsConnector::subscription_element);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e =
      m_ctx->GenerateConnector(ScType::ConstCommonEdge, ATestGenerateEdgeAsConnector::subscription_element, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateEdgeAsConnector::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateEdgeAsConnector>(ATestGenerateEdgeAsConnector::subscription_element);
}

TEST_F(ScAgentTest, ATestEraseIncomingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestEraseIncomingArc>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestEraseIncomingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseIncomingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseOutgoingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstPermPosArc, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestEraseOutgoingArc>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestEraseOutgoingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseOutgoingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstCommonEdge, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestEraseEdge>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestEraseEdge::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseEdge>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseElement)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  m_ctx->SubscribeAgent<ATestEraseElement>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionElementAddr));
  EXPECT_TRUE(ATestEraseElement::msWaiter.Wait());

  EXPECT_THROW(m_ctx->UnsubscribeAgent<ATestEraseElement>(subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, ATestEraseElementNotInitiated)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  m_ctx->SubscribeAgent<ATestEraseElement>(subscriptionElementAddr);
  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestEraseElement>(subscriptionElementAddr));
}

TEST_F(ScAgentTest, ATestChangeLinkContent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);

  m_ctx->SubscribeAgent<ATestChangeLinkContent>(subscriptionElementAddr);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(subscriptionElementAddr, stream));
  EXPECT_TRUE(ATestChangeLinkContent::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestChangeLinkContent>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateMultipleOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  m_ctx->SubscribeAgent<ATestGenerateMultipleOutputArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, subscriptionElementAddr, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestGenerateMultipleOutputArc::msWaiter.Wait());

  ScAddr const e2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, subscriptionElementAddr, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestGenerateMultipleOutputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateMultipleOutputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestDoProgramOne)
{
  m_ctx->SubscribeAgent<ATestDoProgramOne>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestDoProgramOne::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestDoProgramOne>();
}

TEST_F(ScAgentTest, ATestDoProgramTwo)
{
  m_ctx->SubscribeAgent<ATestDoProgramTwo>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestDoProgramTwo::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestDoProgramTwo>();
}

TEST_F(ScAgentTest, ATestInvalidAbstractAgent)
{
  ATestDoProgram agent;
  EXPECT_NO_THROW(agent.GetAbstractAgent());
  EXPECT_THROW(agent.GetEventClass(), utils::ExceptionItemNotFound);
  EXPECT_THROW(agent.GetEventSubscriptionElement(), utils::ExceptionItemNotFound);
  EXPECT_THROW(agent.GetInitiationCondition(), utils::ExceptionItemNotFound);
  EXPECT_THROW(agent.GetResultCondition(), utils::ExceptionItemNotFound);
}

TEST_F(ScAgentTest, ATestCallWithoutEventInitiation)
{
  ATestDoProgram agent;
  ScAction action = m_ctx->GenerateAction(agent.GetActionClass()).Initiate();
  agent.DoProgram(action);
  EXPECT_TRUE(ATestDoProgram::msWaiter.Wait());
}

TEST_F(ScAgentTest, ATestExceptionInDoProgram)
{
  m_ctx->SubscribeAgent<ATestException>();

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .Initiate();

  EXPECT_FALSE(ATestException::msWaiter.Wait(0.1));
  EXPECT_TRUE(action.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<ATestException>();
}

TEST_F(ScAgentTest, ATestEraseActionWithExceptionInDoProgram)
{
  m_ctx->SubscribeAgent<ATestEraseActionWithException>();

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .Initiate();

  EXPECT_FALSE(ATestEraseActionWithException::msWaiter.Wait(0.1));
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinished());

  m_ctx->UnsubscribeAgent<ATestEraseActionWithException>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentWithWaitingAgentWaiter)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentWithWaitingActionToBeFinished)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentWithWaitingAgentWaiter)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(2, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentWithWaitingActionToBeFinished)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .SetArgument(2, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsWithWaitingAgentWaiter)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(
          ATestGenerateOutgoingArc::generate_outgoing_arc_action,
          ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsWithWaitingActionToBeFinished)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .SetArguments(
                      ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                      ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplate>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplate::msWaiter.Wait(0.2));

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetInitiationConditionTemplate::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplate>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplateHasEventTripleTwice)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplateHasEventTripleTwice>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplateHasEventTripleTwice::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplateHasEventTripleTwice>();
}

TEST_F(ScAgentTest, ATestGetInvalidInitiationConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetInvalidInitiationConditionTemplate>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInvalidInitiationConditionTemplate::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInvalidInitiationConditionTemplate>();
}

TEST_F(ScAgentTest, ATestCheckInitiationCondition)
{
  m_ctx->SubscribeAgent<ATestCheckInitiationCondition>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestCheckInitiationCondition::msWaiter.Wait(0.2));

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckInitiationCondition::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestCheckInitiationCondition>();
}

TEST_F(ScAgentTest, ATestGetResultConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetResultConditionTemplate>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestGetResultConditionTemplate>();
}

TEST_F(ScAgentTest, ATestGetInvalidResultConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetInvalidResultConditionTemplate>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetInvalidResultConditionTemplate::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGetInvalidResultConditionTemplate>();
}

TEST_F(ScAgentTest, ATestCheckResultCondition)
{
  m_ctx->SubscribeAgent<ATestCheckResultCondition>();

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestCheckResultCondition>();
}

TEST_F(ScAgentTest, ActionDeactivated)
{
  m_ctx->SubscribeAgent<ATestActionDeactivated>();

  ScAddr const & arcAddr = m_ctx->GenerateConnector(
      ScType::ConstPermPosArc, ScKeynodes::action_deactivated, ATestGenerateOutgoingArc::generate_outgoing_arc_action);

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestActionDeactivated::msWaiter.Wait(0.2));

  m_ctx->EraseElement(arcAddr);

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestActionDeactivated::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestActionDeactivated>();
}

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  ATestGenerateOutgoingArc::msWaiter.Reset();

  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestModule module;
  module.Agent<ATestGenerateOutgoingArc>(subscriptionElementAddr);
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->GenerateConnector(ScType::ConstPermPosArc, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateOutgoingArc::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentTest, RegisterActionInitiatedAgentWithinModule)
{
  ATestCheckResult::msWaiter.Reset();

  TestModule module;
  module.Agent<ATestCheckResult>();
  module.Register(&*m_ctx);

  m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentTest, RegisterActionErasingElementWithinModule)
{
  ATestEraseElement::msWaiter.Reset();

  ScAddr const & subscriptionElementAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestModule module;
  module.Agent<ATestEraseElement>(subscriptionElementAddr);
  module.Register(&*m_ctx);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionElementAddr));
  EXPECT_TRUE(ATestEraseElement::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentTest, AgentHasNoSpecificationInKb)
{
  ATestCheckResult agent;
  EXPECT_THROW(agent.GetAbstractAgent(), utils::ExceptionItemNotFound);
  EXPECT_NO_THROW(agent.GetActionClass());
  EXPECT_EQ(agent.GetEventClass(), ScKeynodes::sc_event_after_generate_outgoing_arc);
  EXPECT_EQ(agent.GetEventSubscriptionElement(), ScKeynodes::action_initiated);
  EXPECT_THROW(agent.GetResultCondition(), utils::ExceptionItemNotFound);
  EXPECT_EQ(agent.GetName(), "ATestCheckResult");
}
