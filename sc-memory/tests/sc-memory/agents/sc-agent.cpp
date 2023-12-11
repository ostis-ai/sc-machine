#include <gtest/gtest.h>

#include "sc-memory/kpm/sc_agent.hpp"

#include "test_sc_agent.hpp"

#include "agents_test_utils.hpp"

TEST_F(ScAgentTest, ATestAddInputEdge)
{
  SC_AGENT_REGISTER(ATestAddInputEdge, ATestAddInputEdge::msAgentKeynode);

  EXPECT_TRUE(ATestAddInputEdgeInstance.GetName() == "ATestAddInputEdge");

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, ATestAddInputEdge::msAgentKeynode);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputEdge::msWaiter.Wait());
  SC_AGENT_UNREGISTER(ATestAddInputEdge);
}

TEST_F(ScAgentTest, ATestAddOutputEdge)
{
  SC_AGENT_REGISTER(ATestAddOutputEdge, ATestAddOutputEdge::msAgentKeynode);

  EXPECT_TRUE(ATestAddOutputEdgeInstance.GetName() == "ATestAddOutputEdge");

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputEdge::msWaiter.Wait());
  SC_AGENT_UNREGISTER(ATestAddOutputEdge);
}

TEST_F(ScAgentTest, ATestRemoveInputEdge)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, node, ATestRemoveInputEdge::msAgentKeynode);
  EXPECT_TRUE(e.IsValid());

  SC_AGENT_REGISTER(ATestRemoveInputEdge, ATestRemoveInputEdge::msAgentKeynode);

  EXPECT_TRUE(ATestRemoveInputEdgeInstance.GetName() == "ATestRemoveInputEdge");

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputEdge::msWaiter.Wait());
  SC_AGENT_UNREGISTER(ATestRemoveInputEdge);
}

TEST_F(ScAgentTest, ATestRemoveOutputEdge)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, ATestRemoveOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());

  SC_AGENT_REGISTER(ATestRemoveOutputEdge, ATestRemoveOutputEdge::msAgentKeynode);

  EXPECT_TRUE(ATestRemoveOutputEdgeInstance.GetName() == "ATestRemoveOutputEdge");

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputEdge::msWaiter.Wait());
  SC_AGENT_UNREGISTER(ATestRemoveOutputEdge);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  SC_AGENT_REGISTER(ATestRemoveElement, ATestRemoveElement::msAgentKeynode);

  EXPECT_TRUE(ATestRemoveElementInstance.GetName() == "ATestRemoveElement");

  EXPECT_TRUE(m_ctx->EraseElement(ATestRemoveElement::msAgentKeynode));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());
  SC_AGENT_UNREGISTER(ATestRemoveElement);
}

TEST_F(ScAgentTest, ATestContentChanged)
{
  ScAddr const link = ATestContentChanged::msAgentKeynode;

  SC_AGENT_REGISTER(ATestContentChanged, ATestContentChanged::msAgentKeynode);

  EXPECT_TRUE(ATestContentChangedInstance.GetName() == "ATestContentChanged");

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(link, stream));
  EXPECT_TRUE(ATestContentChanged::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestContentChanged);
}

TEST_F(ScAgentTest, ATestAddMultipleOutputEdge)
{
  ScAddrVector const & addrs = {
      ATestAddMultipleOutputEdge::msAgentKeynode1, ATestAddMultipleOutputEdge::msAgentKeynode2};
  SC_AGENT_REGISTER(ATestAddMultipleOutputEdge, addrs);

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

  SC_AGENT_UNREGISTER(ATestAddMultipleOutputEdge);
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgument)
{
  SC_AGENT_REGISTER(ATestCheckResult, ATestCheckResult::msAgentKeynode);

  auto * context = new ScAgentContext(sc_access_lvl_make_max, "ATestCheckResult");
  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(
      context
          ->InitializeEvent<ScEventAddInputEdge>(
              actionAddr,
              [context, &actionAddr]() {
                context->SetActionArgument(actionAddr, actionAddr, 1);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestCheckResult::msAgentKeynode, actionAddr);
              },
              [context](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr) -> sc_result {
                return context->HelperCheckEdge(
                           ScKeynodes::kQuestionFinishedUnsuccessfully, addr, ScType::EdgeAccessConstPosPerm)
                           ? SC_RESULT_OK
                           : SC_RESULT_NO;
              })
          ->Wait(2000, {}, [context, &actionAddr]() {
            EXPECT_TRUE(context->HelperCheckEdge(
                ScKeynodes::kQuestionFinishedUnsuccessfully, actionAddr, ScType::EdgeAccessConstPosPerm));
            EXPECT_TRUE(context->HelperCheckEdge(
                ATestCheckResult::msAgentSet, ATestCheckResult::msAgentKeynode, ScType::EdgeAccessConstNegPerm));
          }));

  context->Destroy();
  delete context;

  SC_AGENT_UNREGISTER(ATestCheckResult);
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgument)
{
  SC_AGENT_REGISTER(ATestCheckResult, ATestCheckResult::msAgentKeynode);

  auto * context = new ScAgentContext(sc_access_lvl_make_max, "ATestCheckResult");
  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(
      context
          ->InitializeEvent<ScEventAddInputEdge>(
              actionAddr,
              [context, &actionAddr]() {
                context->SetActionArgument(actionAddr, actionAddr, 2);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestCheckResult::msAgentKeynode, actionAddr);
              },
              [context](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr) -> sc_result {
                return context->HelperCheckEdge(
                           ScKeynodes::kQuestionFinishedWithError, addr, ScType::EdgeAccessConstPosPerm)
                           ? SC_RESULT_OK
                           : SC_RESULT_NO;
              })
          ->Wait(2000, {}, [context, &actionAddr]() {
            EXPECT_TRUE(context->HelperCheckEdge(
                ScKeynodes::kQuestionFinishedWithError, actionAddr, ScType::EdgeAccessConstPosPerm));
            EXPECT_TRUE(context->HelperCheckEdge(
                ATestCheckResult::msAgentSet, ATestCheckResult::msAgentKeynode, ScType::EdgeAccessConstFuzPerm));
          }));

  context->Destroy();
  delete context;

  SC_AGENT_UNREGISTER(ATestCheckResult);
}

TEST_F(ScAgentTest, ATestCheckResultTwoArguments)
{
  SC_AGENT_REGISTER(ATestCheckResult, ATestCheckResult::msAgentKeynode);

  auto * context = new ScAgentContext(sc_access_lvl_make_max, "ATestCheckResult");
  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(
      context
          ->InitializeEvent<ScEventAddInputEdge>(
              actionAddr,
              [context, &actionAddr]() {
                ScAddr const & addr1 = context->CreateNode(ScType::NodeConst);
                ScAddr const & addr2 = context->CreateNode(ScType::NodeConst);
                context->SetActionArgument(actionAddr, addr1, 1);
                context->SetActionArgument(actionAddr, addr2, 2);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestCheckResult::msAgentKeynode, actionAddr);
              },
              [context](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr) -> sc_result {
                return context->HelperCheckEdge(
                           ScKeynodes::kQuestionFinishedSuccessfully, addr, ScType::EdgeAccessConstPosPerm)
                           ? SC_RESULT_OK
                           : SC_RESULT_NO;
              })
          ->Wait(2000, [context, &actionAddr]() {
            EXPECT_TRUE(context->HelperCheckEdge(
                ScKeynodes::kQuestionFinishedSuccessfully, actionAddr, ScType::EdgeAccessConstPosPerm));
            EXPECT_TRUE(context->HelperCheckEdge(
                ATestCheckResult::msAgentSet, ATestCheckResult::msAgentKeynode, ScType::EdgeAccessConstPosPerm));
          }));

  context->Destroy();
  delete context;

  SC_AGENT_UNREGISTER(ATestCheckResult);
}
