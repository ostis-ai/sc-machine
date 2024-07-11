#include <gtest/gtest.h>

#include "sc-memory/sc_agent.hpp"

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

#include "agents_test_utils.hpp"

TEST_F(ScAgentTest, ATestAddInputEdge)
{
  RegisterAgent<ATestAddInputEdge>(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, ATestAddInputEdge::msAgentKeynode);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputEdge::msWaiter.Wait());

  UnregisterAgent<ATestAddInputEdge>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestAddOutputEdge)
{
  RegisterAgent<ATestAddOutputEdge>(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputEdge::msWaiter.Wait());

  UnregisterAgent<ATestAddOutputEdge>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestRemoveInputEdge)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, node, ATestRemoveInputEdge::msAgentKeynode);
  EXPECT_TRUE(e.IsValid());

  RegisterAgent<ATestRemoveInputEdge>(&*m_ctx);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputEdge::msWaiter.Wait());

  UnregisterAgent<ATestRemoveInputEdge>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestRemoveOutputEdge)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, ATestRemoveOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());

  RegisterAgent<ATestRemoveOutputEdge>(&*m_ctx);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputEdge::msWaiter.Wait());

  UnregisterAgent<ATestRemoveOutputEdge>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  RegisterAgent<ATestRemoveElement>(&*m_ctx);

  EXPECT_TRUE(m_ctx->EraseElement(ATestRemoveElement::msAgentKeynode));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());

  UnregisterAgent<ATestRemoveElement>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestContentChanged)
{
  ScAddr const link = ATestContentChanged::msAgentKeynode;

  RegisterAgent<ATestContentChanged>(&*m_ctx);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(link, stream));
  EXPECT_TRUE(ATestContentChanged::msWaiter.Wait());

  UnregisterAgent<ATestContentChanged>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestAddMultipleOutputEdge)
{
  RegisterAgent<ATestAddMultipleOutputEdge>(
      &*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 =
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddMultipleOutputEdge::msAgentKeynode1, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputEdge::msWaiter.Wait());

  ScAddr const e2 =
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddMultipleOutputEdge::msAgentKeynode2, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputEdge::msWaiter.Wait());

  UnregisterAgent<ATestAddMultipleOutputEdge>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgument)
{
  RegisterAgent<ATestCheckResult>(&*m_ctx);

  auto * context = new ScAgentContext();
  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(
      context
          ->InitializeEvent<ScEventAddInputEdge>(
              actionAddr,
              [context, &actionAddr]()
              {
                context->SetActionArgument(actionAddr, actionAddr, 1);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestCheckResult::msAgentKeynode, actionAddr);
              },
              [context](ScAddr const & classAddr, ScAddr const &, ScAddr const &) -> sc_result
              {
                return context->HelperCheckEdge(
                           ScKeynodes::action_finished_unsuccessfully, classAddr, ScType::EdgeAccessConstPosPerm)
                           ? SC_RESULT_OK
                           : SC_RESULT_NO;
              })
          ->Wait(
              2000,
              {},
              [context, &actionAddr]()
              {
                EXPECT_TRUE(context->HelperCheckEdge(
                    ScKeynodes::action_finished_unsuccessfully, actionAddr, ScType::EdgeAccessConstPosPerm));
                EXPECT_TRUE(context->HelperCheckEdge(
                    ATestCheckResult::msAgentSet, ATestCheckResult::msAgentKeynode, ScType::EdgeAccessConstNegPerm));
              }));

  context->Destroy();
  delete context;

  UnregisterAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgument)
{
  RegisterAgent<ATestCheckResult>(&*m_ctx);

  auto * context = new ScAgentContext();
  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(
      context
          ->InitializeEvent<ScEventAddInputEdge>(
              actionAddr,
              [context, &actionAddr]()
              {
                context->SetActionArgument(actionAddr, actionAddr, 2);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestCheckResult::msAgentKeynode, actionAddr);
              },
              [context](ScAddr const & classAddr, ScAddr const &, ScAddr const &) -> sc_result
              {
                return context->HelperCheckEdge(
                           ScKeynodes::action_finished_with_error, classAddr, ScType::EdgeAccessConstPosPerm)
                           ? SC_RESULT_OK
                           : SC_RESULT_NO;
              })
          ->Wait(
              2000,
              {},
              [context, &actionAddr]()
              {
                EXPECT_TRUE(context->HelperCheckEdge(
                    ScKeynodes::action_finished_with_error, actionAddr, ScType::EdgeAccessConstPosPerm));
                EXPECT_TRUE(context->HelperCheckEdge(
                    ATestCheckResult::msAgentSet, ATestCheckResult::msAgentKeynode, ScType::EdgeAccessConstFuzPerm));
              }));

  context->Destroy();
  delete context;

  UnregisterAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, ATestCheckResultTwoArguments)
{
  RegisterAgent<ATestCheckResult>(&*m_ctx);

  auto * context = new ScAgentContext();
  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(
      context
          ->InitializeEvent<ScEventAddInputEdge>(
              actionAddr,
              [context, &actionAddr]()
              {
                ScAddr const & addr1 = context->CreateNode(ScType::NodeConst);
                ScAddr const & addr2 = context->CreateNode(ScType::NodeConst);
                context->SetActionArgument(actionAddr, addr1, 1);
                context->SetActionArgument(actionAddr, addr2, 2);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestCheckResult::msAgentKeynode, actionAddr);
              },
              [context](ScAddr const & classAddr, ScAddr const &, ScAddr const &) -> sc_result
              {
                return context->HelperCheckEdge(
                           ScKeynodes::action_finished_successfully, classAddr, ScType::EdgeAccessConstPosPerm)
                           ? SC_RESULT_OK
                           : SC_RESULT_NO;
              })
          ->Wait(
              2000,
              [context, &actionAddr]()
              {
                EXPECT_TRUE(context->HelperCheckEdge(
                    ScKeynodes::action_finished_successfully, actionAddr, ScType::EdgeAccessConstPosPerm));
                EXPECT_TRUE(context->HelperCheckEdge(
                    ATestCheckResult::msAgentSet, ATestCheckResult::msAgentKeynode, ScType::EdgeAccessConstPosPerm));
              }));

  context->Destroy();
  delete context;

  UnregisterAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  TestModule module;
  module.Agent<ATestAddOutputEdge>();
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputEdge::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}
