#include "agents_test_utils.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

TEST_F(ScAgentTest, InvalidSubscription)
{
  ScAddr const & subscriptionElementAddr{1233241};
  EXPECT_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentTest, SubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
  EXPECT_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
}

TEST_F(ScAgentTest, UnsubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr));
  EXPECT_THROW(
      m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, UnsubscribeNotSubscribedAgent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(
      m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, ATestGenerateConnector)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestGenerateConnector>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateConnector::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateConnector>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateIncomingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateIncomingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateIncomingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateOutgoingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestGenerateOutgoingArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateOutgoingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateOutgoingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestGenerateEdge>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateEdge::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateEdge>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseIncomingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestEraseIncomingArc>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestEraseIncomingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseIncomingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseOutgoingArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestEraseOutgoingArc>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestEraseOutgoingArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseOutgoingArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestEraseEdge>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestEraseEdge::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseEdge>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestEraseElement)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->SubscribeAgent<ATestEraseElement>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionElementAddr));
  EXPECT_TRUE(ATestEraseElement::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestEraseElement>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestChangeLinkContent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateLink(ScType::LinkConst);

  m_ctx->SubscribeAgent<ATestChangeLinkContent>(subscriptionElementAddr);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(subscriptionElementAddr, stream));
  EXPECT_TRUE(ATestChangeLinkContent::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestChangeLinkContent>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestGenerateMultipleOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->SubscribeAgent<ATestGenerateMultipleOutputArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestGenerateMultipleOutputArc::msWaiter.Wait());

  ScAddr const e2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestGenerateMultipleOutputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestGenerateMultipleOutputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestDoProgramOne)
{
  m_ctx->SubscribeAgent<ATestDoProgramOne>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestDoProgramOne::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestDoProgramOne>();
}

TEST_F(ScAgentTest, ATestDoProgramTwo)
{
  m_ctx->SubscribeAgent<ATestDoProgramTwo>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
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
  ScAction action = m_ctx->CreateAction(agent.GetActionClass()).Initiate();
  agent.DoProgram(action);
  EXPECT_TRUE(ATestDoProgram::msWaiter.Wait());
}

TEST_F(ScAgentTest, ATestExceptionInDoProgram)
{
  m_ctx->SubscribeAgent<ATestException>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .Initiate();

  EXPECT_FALSE(ATestException::msWaiter.Wait(0.1));
  EXPECT_TRUE(action.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<ATestException>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentWithWaitingAgentWaiter)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentWithWaitingActionToBeFinished)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentWithWaitingAgentWaiter)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(2, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentWithWaitingActionToBeFinished)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .SetArgument(2, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsWithWaitingAgentWaiter)
{
  ATestCheckResult::msWaiter.Reset();

  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
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

  EXPECT_TRUE(m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .SetArguments(
                      ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                      ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplate>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplate::msWaiter.Wait(0.2));

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetInitiationConditionTemplate::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplate>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplateWithoutEventSubscriptionElement>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplateWithInvalidConnectorTypeInEventTriple>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplateHasEventTripleTwice)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplateHasEventTripleTwice>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplateHasEventTripleTwice::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplateHasEventTripleTwice>();
}

TEST_F(ScAgentTest, ATestCheckInitiationCondition)
{
  m_ctx->SubscribeAgent<ATestCheckInitiationCondition>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestCheckInitiationCondition::msWaiter.Wait(0.2));

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckInitiationCondition::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestCheckInitiationCondition>();
}

TEST_F(ScAgentTest, ATestGetResultConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetResultConditionTemplate>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestGetResultConditionTemplate>();
}

TEST_F(ScAgentTest, ATestCheckResultCondition)
{
  m_ctx->SubscribeAgent<ATestCheckResultCondition>();

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArguments(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestCheckResultCondition>();
}

TEST_F(ScAgentTest, ActionDeactivated)
{
  m_ctx->SubscribeAgent<ATestActionDeactivated>();

  ScAddr const & arcAddr = m_ctx->CreateEdge(
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::action_deactivated,
      ATestGenerateOutgoingArc::generate_outgoing_arc_action);

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestActionDeactivated::msWaiter.Wait(0.2));

  m_ctx->EraseElement(arcAddr);

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestActionDeactivated::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestActionDeactivated>();
}

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  ATestGenerateOutgoingArc::msWaiter.Reset();

  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.Agent<ATestGenerateOutgoingArc>(subscriptionElementAddr);
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestGenerateOutgoingArc::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentTest, RegisterActionInitiatedAgentWithinModule)
{
  ATestGenerateOutgoingArc::msWaiter.Reset();

  TestModule module;
  module.Agent<ATestCheckResult>();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .SetArgument(1, ATestGenerateOutgoingArc::generate_outgoing_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

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
  EXPECT_NO_THROW(agent.GetResultConditionTemplate());
  EXPECT_EQ(agent.GetName(), "ATestCheckResult");
}
