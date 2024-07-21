#include <gtest/gtest.h>

#include "sc-memory/sc_agent.hpp"

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

#include "agents_test_utils.hpp"

TEST_F(ScAgentTest, ATestAddInputEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddInputEdge>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestAddInputEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestAddOutputEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  SubscribeAgent<ATestAddOutputEdge>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestAddOutputEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveInputEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, node, subscriptionAddr);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveInputEdge>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveInputEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveOutputEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());

  SubscribeAgent<ATestRemoveOutputEdge>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveOutputEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  SubscribeAgent<ATestRemoveElement>(&*m_ctx, subscriptionAddr);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionAddr));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());

  UnsubscribeAgent<ATestRemoveElement>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestContentChanged)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateLink(ScType::LinkConst);

  SubscribeAgent<ATestContentChanged>(&*m_ctx, subscriptionAddr);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(subscriptionAddr, stream));
  EXPECT_TRUE(ATestContentChanged::msWaiter.Wait());

  UnsubscribeAgent<ATestContentChanged>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestAddMultipleOutputEdge)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  SubscribeAgent<ATestAddMultipleOutputEdge>(&*m_ctx, subscriptionAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputEdge::msWaiter.Wait());

  ScAddr const e2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputEdge::msWaiter.Wait());

  UnsubscribeAgent<ATestAddMultipleOutputEdge>(&*m_ctx, subscriptionAddr);
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgument)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  EXPECT_TRUE(context.CreateAction(ATestAddOutputEdge::add_output_edge_action)
                  .SetArgument(1, ATestAddOutputEdge::add_output_edge_action)
                  .Initiate()
                  ->Wait(2000));

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgument)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  EXPECT_TRUE(context.CreateAction(ATestAddOutputEdge::add_output_edge_action)
                  .SetArgument(2, ATestAddOutputEdge::add_output_edge_action)
                  .Initiate()
                  ->Wait(2000));

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultTwoArguments)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;

  EXPECT_TRUE(context.CreateAction(ATestAddOutputEdge::add_output_edge_action)
                  .SetArguments(ATestAddOutputEdge::add_output_edge_action, ATestAddOutputEdge::add_output_edge_action)
                  .Initiate()
                  ->Wait(2000));

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  ScAddr const & subscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.Agent<ATestAddOutputEdge>(subscriptionAddr);
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputEdge::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}
