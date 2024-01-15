#include <gtest/gtest.h>

#include "sc-memory/kpm/sc_agent.hpp"

#include "test_sc_agent.hpp"

#include "agents_test_utils.hpp"

namespace
{

ScAddr CreateKeynode(ScMemoryContext & ctx, std::string const & name)
{
  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  EXPECT_TRUE(ctx.HelperSetSystemIdtf(name, node));

  return node;
}

} // namespace


TEST_F(ScAgentTest, ATestAction)
{
  ScAddr const command_1 = CreateKeynode(*m_ctx, "command_1");

  ScAgentInit(true);
  ATestAction::InitGlobal();

  SC_AGENT_REGISTER(ATestAction);

  ScAddr const cmd = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(cmd.IsValid());
  ScAddr const e1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, command_1, cmd);
  EXPECT_TRUE(e1.IsValid());
  ScAddr const e2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, ScAgentAction::GetCommandInitiatedAddr(), cmd);
  EXPECT_TRUE(e2.IsValid());

  EXPECT_TRUE(ATestAction::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestAction);
}

TEST_F(ScAgentTest, ATestAddInputEdge)
{
  ScAgentInit(true);
  ATestAddInputEdge::InitGlobal();
  SC_AGENT_REGISTER(ATestAddInputEdge);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, ATestAddInputEdge::msAgentKeynode);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestAddInputEdge);
}

TEST_F(ScAgentTest, ATestAddOutputEdge)
{
  ScAgentInit(true);
  ATestAddOutputEdge::InitGlobal();
  SC_AGENT_REGISTER(ATestAddOutputEdge);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ATestAddOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestAddOutputEdge);
}

TEST_F(ScAgentTest, ATestRemoveInputEdge)
{
  ScAgentInit(true);
  ATestRemoveInputEdge::InitGlobal();

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, node, ATestRemoveInputEdge::msAgentKeynode);
  EXPECT_TRUE(e.IsValid());

  SC_AGENT_REGISTER(ATestRemoveInputEdge);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestRemoveInputEdge);
}

TEST_F(ScAgentTest, ATestRemoveOutputEdge)
{
  ScAgentInit(true);
  ATestRemoveOutputEdge::InitGlobal();

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccess, ATestRemoveOutputEdge::msAgentKeynode, node);
  EXPECT_TRUE(e.IsValid());

  SC_AGENT_REGISTER(ATestRemoveOutputEdge);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputEdge::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestRemoveOutputEdge);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  ScAgentInit(true);
  ATestRemoveElement::InitGlobal();

  SC_AGENT_REGISTER(ATestRemoveElement);

  EXPECT_TRUE(m_ctx->EraseElement(ATestRemoveElement::msAgentKeynode));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestRemoveElement);
}

TEST_F(ScAgentTest, ATestContentChanged)
{
  ScAddr const link = m_ctx->CreateLink();
  EXPECT_TRUE(link.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("ATestContentChanged", link));

  ScAgentInit(true);
  ATestContentChanged::InitGlobal();

  SC_AGENT_REGISTER(ATestContentChanged);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);

  EXPECT_TRUE(m_ctx->SetLinkContent(link, stream));
  EXPECT_TRUE(ATestContentChanged::msWaiter.Wait());

  SC_AGENT_UNREGISTER(ATestContentChanged);
}
