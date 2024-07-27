#include "sc_test.hpp"

#include "sc-memory/sc_action.hpp"

#include "test_sc_agent.hpp"

using ScActionTest = ScMemoryTest;

TEST_F(ScActionTest, CreateActionAndGetClass)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, CreateActionAndGetAnswer)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);

  action.FinishSuccessfully();
  EXPECT_TRUE(action.GetAnswer().IsEmpty());
}

TEST_F(ScActionTest, CreateActionAndSetGetAnswer)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetAnswer(structureAddr1));
  EXPECT_NO_THROW(action.SetAnswer(structureAddr1));

  ScAddr const & structureAddr2 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetAnswer(structureAddr2));
  EXPECT_FALSE(m_ctx->IsElement(structureAddr1));
  EXPECT_THROW(action.SetAnswer(structureAddr1), utils::ExceptionInvalidParams);
  EXPECT_NO_THROW(action.SetAnswer(structureAddr2));

  action.FinishSuccessfully();
  EXPECT_THROW(action.SetAnswer(structureAddr2), utils::ExceptionInvalidState);
  ScStruct const & answer = action.GetAnswer();
  EXPECT_TRUE(answer.IsEmpty());
  EXPECT_EQ(answer, action.GetAnswer());
  EXPECT_EQ(answer, structureAddr2);
}

TEST_F(ScActionTest, CreateActionAndSetFormUpdateGetAnswer)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetAnswer(structureAddr1));

  ScAddr const & structureAddr2 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetAnswer(structureAddr2));
  EXPECT_FALSE(m_ctx->IsElement(structureAddr1));

  EXPECT_NO_THROW(action.FormAnswer());
  EXPECT_FALSE(m_ctx->IsElement(structureAddr2));
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.FormAnswer(elementAddr1));

  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.UpdateAnswer(elementAddr2));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormAnswer(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateAnswer(), utils::ExceptionInvalidState);
  ScStruct const & answer = action.GetAnswer();
  EXPECT_FALSE(answer.IsEmpty());

  EXPECT_TRUE(answer.HasElement(elementAddr1));
  EXPECT_TRUE(answer.HasElement(elementAddr2));
}

TEST_F(ScActionTest, CreateActionAndUpdateGetAnswer)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetAnswer(), utils::ExceptionInvalidState);

  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.UpdateAnswer(elementAddr1));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormAnswer(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateAnswer(), utils::ExceptionInvalidState);
  ScStruct const & answer = action.GetAnswer();
  EXPECT_FALSE(answer.IsEmpty());

  EXPECT_TRUE(answer.HasElement(elementAddr1));
}

TEST_F(ScActionTest, CreateActionAndSetGetArgument)
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

TEST_F(ScActionTest, CreateActionAndSetGetArguments)
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

TEST_F(ScActionTest, CreateActionSetResetGetArguments)
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

  EXPECT_EQ(&action.SetArguments(testDefaultArgumentAddr1, testDefaultArgumentAddr2), &action);
  {
    auto [argAddr1, _] = action.GetArguments<2>();

    EXPECT_EQ(argAddr1, testDefaultArgumentAddr1);
    EXPECT_EQ(_, testDefaultArgumentAddr2);
  }
}

TEST_F(ScActionTest, InitiateAndWaitAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action =
      context.CreateAction(ATestAddOutputArc::add_output_arc_action)
          .SetArguments(ATestAddOutputArc::add_output_arc_action, ATestAddOutputArc::add_output_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishSuccessfullyNotInitiatedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateInitiatedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateFinishedAction)
{
  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_NO_THROW(action.FinishSuccessfully());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
}

TEST_F(ScActionTest, InitiateAndWaitFinishedAction)
{
  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_NO_THROW(action.InitiateAndWait(1));
  EXPECT_NO_THROW(action.FinishSuccessfully());
  EXPECT_THROW(action.InitiateAndWait(1), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
}

TEST_F(ScActionTest, InitiateWaitAndFinishUnsuccessfullyNotInitiatedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_THROW(action.FinishUnsuccessfully(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishWithErrorNotInitiatedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_THROW(action.FinishWithError(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishSuccessfullyFinishedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishUnsuccessfullyFinishedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishUnsuccessfully(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishWithErrorFinishedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishWithError(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndInitiateInitiatedAction)
{
  SubscribeAgent<ATestCheckAnswer>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.InitiateAndWait(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  UnsubscribeAgent<ATestCheckAnswer>(&*m_ctx);
}
