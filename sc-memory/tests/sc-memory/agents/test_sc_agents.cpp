#include "agents_test_utils.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

TEST_F(ScAgentTest, InvalidSubscription)
{
  ScAddr const & subscriptionAddr{1233241};
  EXPECT_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentTest, SubscribeAgentTwice)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr));
  EXPECT_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr));
}

TEST_F(ScAgentTest, UnsubscribeAgentTwice)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr));
  EXPECT_NO_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr));
  EXPECT_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, UnsubscribeNotSubscribedAgent)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, ATestAddInputArc)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestAddInputArc>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestAddOutputArc)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddOutputArc>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestAddOutputArc>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestAddEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddEdge>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestAddEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveInputArc)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionAddr);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveInputArc>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveInputArc>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveOutputArc)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveOutputArc>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveOutputArc>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveEdge>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  SubscribeAgent<ATestRemoveElement>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionAddr));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveElement>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestChangeLinkContent)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateLink(ScType::LinkConst);

  SubscribeAgent<ATestChangeLinkContent>(&*m_ctx, subscriptionAddr);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(subscriptionAddr, stream));
  EXPECT_TRUE(ATestChangeLinkContent::msWaiter.Wait());

  UnsubscribeAgent<ATestChangeLinkContent>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestAddMultipleOutputArc)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  SubscribeAgent<ATestAddMultipleOutputArc>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputArc::msWaiter.Wait());

  ScAddr const e2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputArc::msWaiter.Wait());

  UnsubscribeAgent<ATestAddMultipleOutputArc>(&*m_ctx, subscriptionAddr);
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

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.Agent<ATestAddOutputArc>(subscriptionAddr);
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputArc::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}
