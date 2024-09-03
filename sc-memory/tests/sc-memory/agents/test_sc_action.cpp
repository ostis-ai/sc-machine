#include "sc_test.hpp"

#include "sc-memory/sc_action.hpp"

#include "test_sc_agent.hpp"

using ScActionTest = ScMemoryTest;

TEST_F(ScActionTest, CreateActionAndGetClassWithReceptorActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::receptor_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->CreateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, CreateActionAndGetClassWithEffectorActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::effector_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->CreateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, CreateActionAndGetClassWithBehavioralActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::behavioral_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->CreateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, CreateActionAndGetClassWithInformationActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::information_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->CreateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, CreateActionAndGetClassWithoutType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);

  ScAction action = m_ctx->CreateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
}

TEST_F(ScActionTest, CreateActionWithInvalidActionClass)
{
  EXPECT_THROW(m_ctx->CreateAction(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithReceptorActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::receptor_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithEffectorActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::effector_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithBehavioralActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::behavioral_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithInformationActionType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::information_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithoutType)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & actionAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
}

TEST_F(ScActionTest, ConvertInvalidAction)
{
  EXPECT_THROW(m_ctx->ConvertToAction(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScActionTest, CreateActionAndGetResult)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::receptor_action, testClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  action.FinishSuccessfully();
  EXPECT_TRUE(action.GetResult().IsEmpty());
}

TEST_F(ScActionTest, CreateActionAndSetGetResult)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

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
  ScStructure const & result = action.GetResult();
  EXPECT_TRUE(result.IsEmpty());
  EXPECT_EQ(result, action.GetResult());
  EXPECT_EQ(result, structureAddr2);
}

TEST_F(ScActionTest, CreateActionAndSetRemoveGetResult)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

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
  EXPECT_THROW(action.GetResult(), utils::ExceptionItemNotFound);
}

TEST_F(ScActionTest, CreateActionAndSetFormUpdateGetResult)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

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
  ScStructure const & result = action.GetResult();
  EXPECT_FALSE(result.IsEmpty());

  EXPECT_TRUE(result.HasElement(elementAddr1));
  EXPECT_TRUE(result.HasElement(elementAddr2));
}

TEST_F(ScActionTest, CreateActionAndUpdateGetResult)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(action.UpdateResult(elementAddr1));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormResult(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateResult(), utils::ExceptionInvalidState);
  ScStructure const & result = action.GetResult();
  EXPECT_FALSE(result.IsEmpty());

  EXPECT_TRUE(result.HasElement(elementAddr1));
}

TEST_F(ScActionTest, CreateActionAndSetGetArgument)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

  EXPECT_EQ(action.GetArgument(1), ScAddr::Empty);

  ScAddr const & testDefaultArgumentAddr = m_ctx->CreateNode(ScType::NodeConst);
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
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

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

  ScAddr const & testDefaultArgumentAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & testDefaultArgumentAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_EQ(&action.SetArguments(testDefaultArgumentAddr1, testDefaultArgumentAddr2), &action);
  {
    auto [argAddr1, _] = action.GetArguments<2>();

    EXPECT_EQ(argAddr1, testDefaultArgumentAddr1);
    EXPECT_EQ(_, testDefaultArgumentAddr2);
  }
}

TEST_F(ScActionTest, CreateActionSetResetGetArguments)
{
  ScAddr const & testClassAddr = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAction action = m_ctx->CreateAction(testClassAddr);

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

  ScAddr const & testDefaultArgumentAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & testDefaultArgumentAddr2 = m_ctx->CreateNode(ScType::NodeConst);
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
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArguments(
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, FinishSuccessfullyNotInitiatedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateInitiatedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateFinishedAction)
{
  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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
  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_NO_THROW(action.InitiateAndWait(1));
  EXPECT_NO_THROW(action.FinishSuccessfully());
  ScIterator3Ptr it3 = m_ctx->Iterator3(ScKeynodes::action_initiated, ScType::EdgeAccessConstPosPerm, action);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_THROW(action.InitiateAndWait(1), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
}

TEST_F(ScActionTest, FinishUnsuccessfullyNotInitiatedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_THROW(action.FinishUnsuccessfully(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, FinishWithErrorNotInitiatedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_THROW(action.FinishWithError(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_FALSE(action.IsFinished());
  EXPECT_FALSE(action.IsFinishedWithError());
  EXPECT_FALSE(action.IsFinishedSuccessfully());
  EXPECT_FALSE(action.IsFinishedUnsuccessfully());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateWaitAndFinishSuccessfullyFinishedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateWaitAndFinishUnsuccessfullyFinishedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishUnsuccessfully(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateWaitAndFinishWithErrorFinishedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.FinishWithError(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateWaitAndInitiateInitiatedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->CreateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_TRUE(action.InitiateAndWait());
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  EXPECT_THROW(action.InitiateAndWait(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}
