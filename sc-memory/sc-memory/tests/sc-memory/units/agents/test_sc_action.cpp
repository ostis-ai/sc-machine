/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_action.hpp>

#include "test_sc_agent.hpp"

using ScActionTest = ScMemoryTest;

TEST_F(ScActionTest, GenerateActionAndGetClassWithReceptorActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::receptor_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->GenerateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, GenerateActionAndGetClassWithEffectorActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::effector_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->GenerateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, GenerateActionAndGetClassWithBehavioralActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr =
      m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::behavioral_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->GenerateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, GenerateActionAndGetClassWithInformationActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr =
      m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::information_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->GenerateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, GenerateActionAndGetClassWithoutType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);

  ScAction action = m_ctx->GenerateAction(testClassAddr);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
}

TEST_F(ScActionTest, GenerateActionWithInvalidActionClass)
{
  EXPECT_THROW(m_ctx->GenerateAction(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithReceptorActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::receptor_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithEffectorActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::effector_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithBehavioralActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr =
      m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::behavioral_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithInformationActionType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr =
      m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::information_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);
  ScAddr const & actionAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
  EXPECT_EQ(action.GetClass(), testClassAddr);
}

TEST_F(ScActionTest, ConvertToActionAndGetClassWithoutType)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & actionAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, testClassAddr, actionAddr);

  ScAction action = m_ctx->ConvertToAction(actionAddr);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
  EXPECT_EQ(action.GetClass(), ScAddr::Empty);
}

TEST_F(ScActionTest, ConvertInvalidAction)
{
  EXPECT_THROW(m_ctx->ConvertToAction(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScActionTest, GenerateActionAndGetResult)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, ScKeynodes::receptor_action, testClassAddr);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);

  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  action.FinishSuccessfully();
  EXPECT_TRUE(action.GetResult().IsEmpty());
}

TEST_F(ScActionTest, GenerateActionAndSetGetResult)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_NO_THROW(action.SetResult(structureAddr1));
  EXPECT_NO_THROW(action.SetResult(structureAddr1));

  ScAddr const & structureAddr2 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
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

TEST_F(ScActionTest, GenerateActionAndSetGetArcAsResult)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & arcAddr1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr1, nodeAddr1);
  EXPECT_NO_THROW(action.SetResult(arcAddr1));
  EXPECT_NO_THROW(action.SetResult(arcAddr1));

  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & arcAddr2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr2, nodeAddr2);
  EXPECT_NO_THROW(action.SetResult(arcAddr2));
  EXPECT_FALSE(m_ctx->IsElement(arcAddr1));
  EXPECT_THROW(action.SetResult(arcAddr1), utils::ExceptionInvalidParams);
  EXPECT_NO_THROW(action.SetResult(arcAddr2));

  action.FinishSuccessfully();
  EXPECT_THROW(action.SetResult(arcAddr2), utils::ExceptionInvalidState);
  ScStructure const & result = action.GetResult();
  EXPECT_TRUE(result.IsEmpty());
  EXPECT_EQ(result, action.GetResult());
  EXPECT_EQ(result, arcAddr2);
}

TEST_F(ScActionTest, GenerateActionAndSetResultIfOtherResultExists)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  ScAddr const & structureAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, structureAddr1);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_result, arcAddr);

  ScAddr const & structureAddr2 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  action.SetResult(structureAddr2);

  EXPECT_THROW(action.FinishSuccessfully(), utils::ExceptionInvalidState);
}

TEST_F(ScActionTest, GenerateActionAndSetResultIfSameResultExists)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  ScAddr const & structureAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, structureAddr1);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_result, arcAddr);

  action.SetResult(structureAddr1);
  EXPECT_NO_THROW(action.FinishSuccessfully());
  EXPECT_EQ(action.GetResult(), structureAddr1);
}

TEST_F(ScActionTest, GenerateActionAndDontSetResultIfOtherResultExists)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  ScAddr const & structureAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, structureAddr1);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_result, arcAddr);

  action.FinishSuccessfully();
  EXPECT_EQ(action.GetResult(), structureAddr1);
}

TEST_F(ScActionTest, GenerateActionAndSetRemoveGetResult)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_NO_THROW(action.SetResult(structureAddr1));

  action.FinishSuccessfully();

  ScIterator5Ptr it5 = m_ctx->CreateIterator5(
      action, ScType::ConstCommonArc, ScType::ConstNodeStructure, ScType::ConstPermPosArc, ScKeynodes::nrel_result);
  EXPECT_TRUE(it5->Next());
  m_ctx->EraseElement(it5->Get(1));
  EXPECT_THROW(action.GetResult(), utils::ExceptionItemNotFound);
}

TEST_F(ScActionTest, GenerateActionAndSetFormUpdateGetResult)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  ScAddr const & structureAddr1 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_NO_THROW(action.SetResult(structureAddr1));

  ScAddr const & structureAddr2 = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_NO_THROW(action.SetResult(structureAddr2));
  EXPECT_FALSE(m_ctx->IsElement(structureAddr1));

  EXPECT_NO_THROW(action.FormResult());
  EXPECT_FALSE(m_ctx->IsElement(structureAddr2));
  ScAddr const & elementAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_NO_THROW(action.FormResult(elementAddr1));

  ScAddr const & elementAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_NO_THROW(action.UpdateResult(elementAddr2));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormResult(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateResult(), utils::ExceptionInvalidState);
  ScStructure const & result = action.GetResult();
  EXPECT_FALSE(result.IsEmpty());

  EXPECT_TRUE(result.HasElement(elementAddr1));
  EXPECT_TRUE(result.HasElement(elementAddr2));
}

TEST_F(ScActionTest, GenerateActionAndUpdateGetResult)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.InitiateAndWait(1));

  EXPECT_THROW(action.GetResult(), utils::ExceptionInvalidState);

  ScAddr const & elementAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_NO_THROW(action.UpdateResult(elementAddr1));

  action.FinishSuccessfully();
  EXPECT_THROW(action.FormResult(), utils::ExceptionInvalidState);
  EXPECT_THROW(action.UpdateResult(), utils::ExceptionInvalidState);
  ScStructure const & result = action.GetResult();
  EXPECT_FALSE(result.IsEmpty());

  EXPECT_TRUE(result.HasElement(elementAddr1));
}

TEST_F(ScActionTest, GenerateActionAndSetGetArgument)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

  EXPECT_EQ(action.GetArgument(1), ScAddr::Empty);

  ScAddr const & testDefaultArgumentAddr = m_ctx->GenerateNode(ScType::ConstNode);
  {
    EXPECT_EQ(action.GetArgument(1, testDefaultArgumentAddr), testDefaultArgumentAddr);
  }
  {
    EXPECT_EQ(&action.SetArgument(1, testDefaultArgumentAddr), &action);
    EXPECT_EQ(action.GetArgument(1), testDefaultArgumentAddr);
  }
}

TEST_F(ScActionTest, GenerateActionAndSetGetArguments)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

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

  ScAddr const & testDefaultArgumentAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & testDefaultArgumentAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_EQ(&action.SetArguments(testDefaultArgumentAddr1, testDefaultArgumentAddr2), &action);
  {
    auto [argAddr1, _] = action.GetArguments<2>();

    EXPECT_EQ(argAddr1, testDefaultArgumentAddr1);
    EXPECT_EQ(_, testDefaultArgumentAddr2);
  }
}

TEST_F(ScActionTest, GenerateActionSetResetGetArguments)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAction action = m_ctx->GenerateAction(testClassAddr);

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

  ScAddr const & testDefaultArgumentAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & testDefaultArgumentAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_TRUE(action.IsInitiated());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, InitiateFinishedAction)
{
  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_NO_THROW(action.Initiate());
  EXPECT_NO_THROW(action.FinishSuccessfully());
  ScIterator3Ptr it3 = m_ctx->CreateIterator3(ScKeynodes::action_initiated, ScType::ConstPermPosArc, action);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_THROW(action.Initiate(), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
}

TEST_F(ScActionTest, InitiateAndWaitFinishedAction)
{
  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  EXPECT_NO_THROW(action.InitiateAndWait(1));
  EXPECT_NO_THROW(action.FinishSuccessfully());
  ScIterator3Ptr it3 = m_ctx->CreateIterator3(ScKeynodes::action_initiated, ScType::ConstPermPosArc, action);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  EXPECT_FALSE(action.IsInitiated());
  EXPECT_THROW(action.InitiateAndWait(1), utils::ExceptionInvalidState);
  EXPECT_FALSE(action.IsInitiated());
}

TEST_F(ScActionTest, FinishUnsuccessfullyNotInitiatedAction)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action);
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

ScAddr SetMaxCustomerWaitingTime(
    ScAgentContext & context,
    ScAddr const & actionAddr,
    sc_uint32 maxCustomerWaitingTime) noexcept(false)
{
  ScAddr const & linkWithTimeAddr = context.GenerateLink();
  context.SetLinkContent(linkWithTimeAddr, std::to_string(maxCustomerWaitingTime));
  ScAddr const & relationArcAddr = context.GenerateConnector(ScType::ConstCommonArc, actionAddr, linkWithTimeAddr);
  context.GenerateConnector(
      ScType::ConstPermPosArc, ScKeynodes::nrel_max_customer_waiting_time_for_action_to_finish, relationArcAddr);
  return linkWithTimeAddr;
}

TEST_F(ScActionTest, SetExpectedExecutionTimeAndGetExpectedExecutionTime)
{
  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArguments(
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  sc_uint32 waitTime = 500;
  SetMaxCustomerWaitingTime(*m_ctx, action, waitTime);
  ScAddr const & maxCustomerWaitingTimeAddr = action.GetMaxCustomerWaitingTimeLink();
  EXPECT_TRUE(m_ctx->IsElement(maxCustomerWaitingTimeAddr));
  EXPECT_EQ(m_ctx->GetElementType(maxCustomerWaitingTimeAddr), ScType::ConstNodeLink);
  sc_uint32 actualWaitTime;
  EXPECT_TRUE(m_ctx->GetLinkContent(maxCustomerWaitingTimeAddr, actualWaitTime));
  EXPECT_EQ(actualWaitTime, waitTime);
  EXPECT_EQ(action.GetMaxCustomerWaitingTime(), waitTime);
}

TEST_F(ScActionTest, SetNodeAsExpectedExecutionTimeAndGetExpectedExecutionTime)
{
  ScAction const action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                              .SetArguments(
                                  ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                                  ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  ScAddr const & relationArcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, ScKeynodes::nrel_inclusion);
  m_ctx->GenerateConnector(
      ScType::ConstPermPosArc, ScKeynodes::nrel_max_customer_waiting_time_for_action_to_finish, relationArcAddr);
  EXPECT_EQ(action.GetMaxCustomerWaitingTimeLink(), ScAddr::Empty);
  EXPECT_EQ(action.GetMaxCustomerWaitingTime(), 0u);
}

TEST_F(ScActionTest, SetLinkWithTextAsExpectedExecutionTimeAndGetExpectedExecutionTime)
{
  ScAction const action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                              .SetArguments(
                                  ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                                  ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  ScAddr const & linkWithTextAddr = m_ctx->GenerateLink();
  m_ctx->SetLinkContent(linkWithTextAddr, "text content");
  ScAddr const & relationArcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, action, linkWithTextAddr);
  m_ctx->GenerateConnector(
      ScType::ConstPermPosArc, ScKeynodes::nrel_max_customer_waiting_time_for_action_to_finish, relationArcAddr);
  EXPECT_EQ(action.GetMaxCustomerWaitingTimeLink(), linkWithTextAddr);
  EXPECT_EQ(action.GetMaxCustomerWaitingTime(), 0u);
}

TEST_F(ScActionTest, InitiateAndWaitActionAndCheckStoredExpectedExecutionTime)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArguments(
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  sc_uint32 waitTime = 500;
  EXPECT_TRUE(action.InitiateAndWait(waitTime));
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());
  ScAddr const & maxCustomerWaitingTimeAddr = action.GetMaxCustomerWaitingTimeLink();
  EXPECT_TRUE(m_ctx->IsElement(maxCustomerWaitingTimeAddr));
  EXPECT_EQ(m_ctx->GetElementType(maxCustomerWaitingTimeAddr), ScType::ConstNodeLink);
  std::string actualWaitTimeString;
  sc_uint32 actualWaitTimeUint32;
  EXPECT_TRUE(m_ctx->GetLinkContent(maxCustomerWaitingTimeAddr, actualWaitTimeString));
  EXPECT_TRUE(m_ctx->GetLinkContent(maxCustomerWaitingTimeAddr, actualWaitTimeUint32));
  EXPECT_EQ(actualWaitTimeUint32, waitTime);
  EXPECT_EQ(actualWaitTimeString, std::to_string(waitTime));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScActionTest, SetExpectedExecutionTimeAndInitiateAndWaitActionAndCheckStoredExpectedExecutionTime)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  ScAction action = m_ctx->GenerateAction(ATestGenerateOutgoingArc::generate_outgoing_arc_action)
                        .SetArguments(
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action,
                            ATestGenerateOutgoingArc::generate_outgoing_arc_action);
  sc_uint32 waitTime = 500;
  SetMaxCustomerWaitingTime(*m_ctx, action, waitTime);
  EXPECT_TRUE(action.InitiateAndWait(waitTime + waitTime));
  EXPECT_TRUE(action.IsInitiated());
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());
  ScAddr const & maxCustomerWaitingTimeAddr = action.GetMaxCustomerWaitingTimeLink();
  EXPECT_TRUE(m_ctx->IsElement(maxCustomerWaitingTimeAddr));
  EXPECT_EQ(m_ctx->GetElementType(maxCustomerWaitingTimeAddr), ScType::ConstNodeLink);
  sc_uint32 actualWaitTime;
  EXPECT_TRUE(m_ctx->GetLinkContent(maxCustomerWaitingTimeAddr, actualWaitTime));
  EXPECT_EQ(actualWaitTime, waitTime);

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}
