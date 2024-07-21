#include <gtest/gtest.h>

#include "sc-memory/sc_action.hpp"

#include "test_sc_agent.hpp"

#include "agents_test_utils.hpp"

TEST_F(ScAgentTest, CreateActionAndGetClass)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScAgentTest, CreateActionAndGetAnswer)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_TRUE(action.GetAnswer().IsEmpty());
}

TEST_F(ScAgentTest, CreateActionAndSetGetArgument)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_EQ(action.GetArgument(1), ScAddr::Empty);

  ScAddr const & testDefaultArgumentAddr = context.CreateNode(ScType::NodeConst);
  {
    EXPECT_EQ(action.GetArgument(1, testDefaultArgumentAddr), testDefaultArgumentAddr);
  }
  {
    EXPECT_EQ(&action.SetArgument(1, testDefaultArgumentAddr), &action);
    EXPECT_EQ(action.GetArgument(1), testDefaultArgumentAddr);
  }
}

TEST_F(ScAgentTest, CreateActionAndSetGetArguments)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  {
    auto [argAddr1, argAddr2] = action.GetArguments<2>();

    EXPECT_EQ(argAddr1, ScAddr::Empty);
    EXPECT_EQ(argAddr2, ScAddr::Empty);
  }
  {
    auto [argAddr1, argAddr2, _] = action.GetArguments<3>();

    EXPECT_EQ(argAddr1, ScAddr::Empty);
    EXPECT_EQ(argAddr2, ScAddr::Empty);
    EXPECT_EQ(_, ScAddr::Empty);
  }

  ScAddr const & testDefaultArgumentAddr1 = context.CreateNode(ScType::NodeConst);
  ScAddr const & testDefaultArgumentAddr2 = context.CreateNode(ScType::NodeConst);
  EXPECT_EQ(&action.SetArguments(testDefaultArgumentAddr1, testDefaultArgumentAddr2), &action);
  {
    auto [argAddr1, _] = action.GetArguments<2>();

    EXPECT_EQ(argAddr1, testDefaultArgumentAddr1);
    EXPECT_EQ(_, testDefaultArgumentAddr2);
  }
}

TEST_F(ScAgentTest, InitiateAndWaitAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action =
      context.CreateAction(ATestAddOutputEdge::add_output_edge_action)
          .SetArguments(ATestAddOutputEdge::add_output_edge_action, ATestAddOutputEdge::add_output_edge_action);
  EXPECT_TRUE(action.Initiate()->Wait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, InitiateWaitAndFinishAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputEdge::add_output_edge_action);
  EXPECT_TRUE(action.Initiate()->Wait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScAgentTest, InitiateWaitAndInitiateAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputEdge::add_output_edge_action);
  EXPECT_TRUE(action.Initiate()->Wait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}
