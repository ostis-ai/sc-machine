#include <gtest/gtest.h>

#include "sc-memory/sc_agent_context.hpp"

#include "agents_test_utils.hpp"

TEST_F(ScAgentTest, SetGetActionArgument)
{
  auto * context = new ScAgentContext();

  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & argumentAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(context->SetActionArgument(actionAddr, argumentAddr, 1).IsValid());
  EXPECT_TRUE(context->GetActionArgument(actionAddr, 1) == argumentAddr);

  context->Destroy();
  delete context;
}

TEST_F(ScAgentTest, SetGetSetGetActionArgument)
{
  auto * context = new ScAgentContext();

  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & argumentAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(context->SetActionArgument(actionAddr, argumentAddr, 1).IsValid());
  EXPECT_TRUE(context->GetActionArgument(actionAddr, 1) == argumentAddr);

  ScAddr const & newArgumentAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(context->SetActionArgument(actionAddr, newArgumentAddr, 1).IsValid());
  EXPECT_TRUE(context->GetActionArgument(actionAddr, 1) == newArgumentAddr);

  context->Destroy();
  delete context;
}

TEST_F(ScAgentTest, GetEmptyActionArgument)
{
  auto * context = new ScAgentContext();

  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_FALSE(context->GetActionArgument(actionAddr, 1).IsValid());

  context->Destroy();
  delete context;
}

TEST_F(ScAgentTest, FormNewStructure)
{
  auto * context = new ScAgentContext();

  ScAddr const & addr1 = context->CreateNode(ScType::NodeConst);
  ScAddr const & addr2 = context->CreateNode(ScType::NodeConst);

  ScAddr const & structAddr = context->FormStructure({addr1, addr2});

  for (auto const & addr : {addr1, addr2})
    EXPECT_TRUE(context->HelperCheckEdge(structAddr, addr, ScType::EdgeAccessConstPosPerm));

  context->Destroy();
  delete context;
}

TEST_F(ScAgentTest, AppendToStructure)
{
  auto * context = new ScAgentContext();

  ScAddr const & addr1 = context->CreateNode(ScType::NodeConst);
  ScAddr const & addr2 = context->CreateNode(ScType::NodeConst);

  ScAddr const & structAddr1 = context->FormStructure({addr1, addr2});

  ScAddr const & addr3 = context->CreateNode(ScType::NodeConst);
  ScAddr const & addr4 = context->CreateNode(ScType::NodeConst);

  ScAddr const & structAddr2 = context->FormStructure({addr3, addr4}, structAddr1);
  EXPECT_TRUE(structAddr1 == structAddr2);

  for (auto const & addr : {addr1, addr2, addr3, addr4})
    EXPECT_TRUE(context->HelperCheckEdge(structAddr1, addr, ScType::EdgeAccessConstPosPerm));

  context->Destroy();
  delete context;
}

TEST_F(ScAgentTest, FormActionAnswer)
{
  auto * context = new ScAgentContext();

  ScAddr const & actionAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & addr1 = context->CreateNode(ScType::NodeConst);
  ScAddr const & addr2 = context->CreateNode(ScType::NodeConst);

  ScAddr structAddr = context->FormStructure({addr1, addr2});

  context->FormActionAnswer(actionAddr, structAddr);

  ScTemplate templ;
  templ.Quintuple(
      actionAddr, ScType::EdgeDCommonVar, ScType::NodeVar, ScType::EdgeAccessVarPosPerm, ScKeynodes::kNrelAnswer);
  ScTemplateSearchResult result;
  context->HelperSearchTemplate(templ, result);

  EXPECT_EQ(result.Size(), 1u);
  ScAddr gottenStructAddr = result[0][2];
  EXPECT_EQ(gottenStructAddr, structAddr);

  for (auto const & addr : {addr1, addr2})
    EXPECT_TRUE(context->HelperCheckEdge(gottenStructAddr, addr, ScType::EdgeAccessConstPosPerm));

  context->Destroy();
  delete context;
}
