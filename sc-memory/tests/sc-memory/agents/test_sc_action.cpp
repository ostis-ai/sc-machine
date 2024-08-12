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

TEST_F(ScActionTest, CreateActionAndGetResult)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  action.FinishSuccessfully();
  EXPECT_TRUE(action.GetResult().IsEmpty());
}

TEST_F(ScActionTest, CreateActionAndSetGetResult)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetResult(structureAddr1));
  EXPECT_NO_THROW(action.SetResult(structureAddr1));

  ScAddr const & structureAddr2 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetResult(structureAddr2));
  EXPECT_FALSE(m_ctx->IsElement(structureAddr1));
  EXPECT_THROW(action.SetResult(structureAddr1), utils::ExceptionInvalidParams);
  EXPECT_NO_THROW(action.SetResult(structureAddr2));

  action.FinishSuccessfully();
  EXPECT_THROW(action.SetResult(structureAddr2), utils::ExceptionInvalidState);
  ScStruct const & result = action.GetResult();
  EXPECT_TRUE(result.IsEmpty());
  EXPECT_EQ(result, action.GetResult());
  EXPECT_EQ(result, structureAddr2);
}

TEST_F(ScActionTest, CreateActionAndSetRemoveGetResult)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetResult(structureAddr1));

  action.FinishSuccessfully();

  ScIterator5Ptr it5 = m_ctx->Iterator5(
      action,
      ScType::EdgeDCommonConst,
      ScType::NodeConstStruct,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_result);
  EXPECT_TRUE(it5->Next());
  m_ctx->EraseElement(it5->Get(1));
  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
}

TEST_F(ScActionTest, CreateActionAndSetFormUpdateGetResult)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetResult(structureAddr1));

  ScAddr const & structureAddr2 = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_NO_THROW(action.SetResult(structureAddr2));
  EXPECT_FALSE(m_ctx->IsElement(structureAddr1));

  EXPECT_NO_THROW(action.FormResult());
  EXPECT_FALSE(m_ctx->IsElement(structureAddr2));
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.FormResult(elementAddr1));

  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.UpdateResult(elementAddr2));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormResult(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateResult(), utils::ExceptionInvalidState);
  ScStruct const & result = action.GetResult();
  EXPECT_FALSE(result.IsEmpty());

  EXPECT_TRUE(result.HasElement(elementAddr1));
  EXPECT_TRUE(result.HasElement(elementAddr2));
}

TEST_F(ScActionTest, CreateActionAndUpdateGetResult)
{
  ScAgentContext context;
  ScAddr const & testClassAddr = context.CreateNode(ScType::NodeConstClass);
  ScAction action = context.CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.UpdateResult(elementAddr1));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormResult(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateResult(), utils::ExceptionInvalidState);
  ScStruct const & result = action.GetResult();
  EXPECT_FALSE(result.IsEmpty());

  EXPECT_TRUE(result.HasElement(elementAddr1));
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
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action =
      context.CreateAction(ATestAddOutputArc::add_output_arc_action)
          .SetArguments(ATestAddOutputArc::add_output_arc_action, ATestAddOutputArc::add_output_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishSuccessfullyNotInitiatedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateInitiatedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateFinishedAction)
{
  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_NO_THROW(action.FinishSuccessfully());
  ScIterator3Ptr it3 = m_ctx->Iterator3(ScKeynodes::action_initiated, ScType::EdgeAccessConstPosPerm, action);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
}

TEST_F(ScActionTest, InitiateAndWaitFinishedAction)
{
  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_NO_THROW(action.InitiateAndWait(1));
  EXPECT_NO_THROW(action.FinishSuccessfully());
  ScIterator3Ptr it3 = m_ctx->Iterator3(ScKeynodes::action_initiated, ScType::EdgeAccessConstPosPerm, action);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_THROW(action.InitiateAndWait(1), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
}

TEST_F(ScActionTest, InitiateWaitAndFinishUnsuccessfullyNotInitiatedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_THROW(action.FinishUnsuccessfully(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishWithErrorNotInitiatedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

  ScAgentContext context;
  ScAction action = context.CreateAction(ATestAddOutputArc::add_output_arc_action);
  EXPECT_THROW(action.FinishWithError(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishSuccessfullyFinishedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

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

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishUnsuccessfullyFinishedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

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

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndFinishWithErrorFinishedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

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

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}

TEST_F(ScActionTest, InitiateWaitAndInitiateInitiatedAction)
{
  SubscribeAgent<ATestCheckResult>(&*m_ctx);

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

  UnsubscribeAgent<ATestCheckResult>(&*m_ctx);
}
