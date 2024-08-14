#include "agents_test_utils.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

TEST_F(ScAgentTest, InvalidSubscription)
{
  ScAddr const & subscriptionElementAddr{1233241};
  EXPECT_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentTest, SubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr));
  EXPECT_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr));
}

TEST_F(ScAgentTest, UnsubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr));
  EXPECT_NO_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr));
  EXPECT_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, UnsubscribeNotSubscribedAgent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, ATestAddInputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestAddOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddOutputArc>(&*m_ctx, subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestAddOutputArc>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestAddEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddEdge>(&*m_ctx, subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestAddEdge>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveInputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveInputArc>(&*m_ctx, subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveInputArc>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveOutputArc>(&*m_ctx, subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveOutputArc>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveEdge>(&*m_ctx, subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveEdge>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  SubscribeAgent<ATestRemoveElement>(&*m_ctx, subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionElementAddr));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveElement>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestChangeLinkContent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateLink(ScType::LinkConst);

  SubscribeAgent<ATestChangeLinkContent>(&*m_ctx, subscriptionElementAddr);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(subscriptionElementAddr, stream));
  EXPECT_TRUE(ATestChangeLinkContent::msWaiter.Wait());

  UnsubscribeAgent<ATestChangeLinkContent>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestAddMultipleOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  SubscribeAgent<ATestAddMultipleOutputArc>(&*m_ctx, subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputArc::msWaiter.Wait());

  ScAddr const e2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestAddMultipleOutputArc>(&*m_ctx, subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentV1)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArgument(1, ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentV2)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  EXPECT_TRUE(context.CreateAction(ATestAddOutputArc::add_output_arc_action)
                  .SetArgument(1, ATestAddOutputArc::add_output_arc_action)
                  .InitiateAndWait(2000));

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentV1)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArgument(2, ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentV2)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  EXPECT_TRUE(context.CreateAction(ATestAddOutputArc::add_output_arc_action)
                  .SetArgument(2, ATestAddOutputArc::add_output_arc_action)
                  .InitiateAndWait(2000));

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsV1)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action, ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsV2)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  EXPECT_TRUE(context.CreateAction(ATestAddOutputArc::add_output_arc_action)
                  .SetArguments(ATestAddOutputArc::add_output_arc_action, ATestAddOutputArc::add_output_arc_action)
                  .InitiateAndWait(2000));

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplate)
{
  SubscribeAgent<ATestGetInitiationConditionTemplate>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplate::msWaiter.Wait(0.2));

  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetInitiationConditionTemplate::msWaiter.Wait(1));

  UnsubscribeAgent<ATestGetInitiationConditionTemplate>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckInitiationCondition)
{
  SubscribeAgent<ATestCheckInitiationCondition>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestCheckInitiationCondition::msWaiter.Wait(0.2));

  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckInitiationCondition::msWaiter.Wait(1));

  UnsubscribeAgent<ATestCheckInitiationCondition>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestGetResultConditionTemplate)
{
  SubscribeAgent<ATestGetResultConditionTemplate>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  UnsubscribeAgent<ATestGetResultConditionTemplate>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultCondition)
{
  SubscribeAgent<ATestCheckResultCondition>(&*m_ctx);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  context.CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  UnsubscribeAgent<ATestCheckResultCondition>(&*m_ctx);
}

TEST_F(ScAgentTest, ActionDeactivated)
{
  SubscribeAgent<ATestActionDeactivated>(&*m_ctx);

  ScAddr const & arcAddr = m_ctx->CreateEdge(
      ScType::EdgeAccessConstPosPerm, ScKeynodes::action_deactivated, ATestAddOutputArc::add_output_arc_action);

  ScAgentContext context;
  context.CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestActionDeactivated::msWaiter.Wait(0.2));

  m_ctx->EraseElement(arcAddr);

  context.CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestActionDeactivated::msWaiter.Wait(1));

  UnsubscribeAgent<ATestActionDeactivated>(&*m_ctx);
}

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.Agent<ATestAddOutputArc>(subscriptionElementAddr);
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputArc::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentTest, AgentHasNoSpecificationInKb)
{
  ATestCheckResult agent;
  EXPECT_THROW(agent.GetAbstractAgent(), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(agent.GetActionClass());
  EXPECT_EQ(agent.GetEventClass(), ScKeynodes::sc_event_add_output_arc);
  EXPECT_THROW(agent.GetEventSubscriptionElement(), utils::ExceptionInvalidState);
  EXPECT_THROW(agent.GetInitiationCondition(), utils::ExceptionInvalidState);
  EXPECT_THROW(agent.GetResultCondition(), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(agent.GetInitiationConditionTemplate());
  EXPECT_NO_THROW(agent.GetResultConditionTemplate());
  EXPECT_EQ(agent.GetName(), "ATestCheckResult");
}
