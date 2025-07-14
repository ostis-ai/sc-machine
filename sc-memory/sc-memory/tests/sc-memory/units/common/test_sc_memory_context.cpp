/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <thread>

#include <sc-memory/sc_event.hpp>
#include <sc-memory/sc_event_subscription.hpp>

#include <sc-memory/sc_memory.hpp>

#include <sc-core/sc_keynodes.h>
#include <sc-memory/sc_structure.hpp>

#define SC_LOCK_WAIT_WHILE_TRUE(expression) \
  ({ \
    sc_uint32 retries = 50; \
    sc_uint32 i = 0; \
    while (expression && i < retries) \
    { \
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
      ++i; \
    } \
  })

void TestAddPermissionsForUserToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & arcAddr = context->GenerateConnector(ScType::ConstCommonArc, userAddr, actionClassAddr);
  context->GenerateConnector(arcType, ScKeynodes::nrel_user_action_class, arcAddr);
}

void TestAddPermissionsForUserToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, readActionInScMemoryAddr, arcType);
}

void TestAddPermissionsForUserToInitWriteActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUserToInitActions(context, userAddr, ScKeynodes::action_generate_in_sc_memory, arcType);
}

void TestAddPermissionsForUserToInitEraseActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUserToInitActions(context, userAddr, ScKeynodes::action_erase_from_sc_memory, arcType);
}

void TestAddAllPermissionsForUserToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUserToInitReadActions(context, userAddr, arcType);
  TestAddPermissionsForUserToInitWriteActions(context, userAddr, arcType);
  TestAddPermissionsForUserToInitEraseActions(context, userAddr, arcType);
}

void TestRemovePermissionsForUserToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr)
{
  ScIterator5Ptr it5 = context->CreateIterator5(
      userAddr, ScType::ConstCommonArc, actionClassAddr, ScType::ConstTempPosArc, ScKeynodes::nrel_user_action_class);
  EXPECT_TRUE(it5->Next());
  ScAddr const & arcAddr = it5->Get(3);
  context->EraseElement(arcAddr);
}

void TestRemovePermissionsForUserToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  TestRemovePermissionsForUserToInitActions(context, userAddr, ScKeynodes::action_read_from_sc_memory);
}

void TestAddPermissionsForUsersSetToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScAddr const & actionClassAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & arcAddr = context->GenerateConnector(ScType::ConstCommonArc, usersSetAddr, actionClassAddr);
  context->GenerateConnector(arcType, ScKeynodes::nrel_users_set_action_class, arcAddr);
}

void TestAddPermissionsForUsersSetToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUsersSetToInitActions(context, usersSetAddr, ScKeynodes::action_read_from_sc_memory, arcType);
}

void TestAddPermissionsForUsersSetToInitWriteActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUsersSetToInitActions(context, usersSetAddr, ScKeynodes::action_generate_in_sc_memory, arcType);
}

void TestAddPermissionsForUsersSetToInitEraseActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUsersSetToInitActions(context, usersSetAddr, ScKeynodes::action_erase_from_sc_memory, arcType);
}

void TestAddAllPermissionsForUsersSetToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUsersSetToInitReadActions(context, usersSetAddr, arcType);
  TestAddPermissionsForUsersSetToInitWriteActions(context, usersSetAddr, arcType);
  TestAddPermissionsForUsersSetToInitEraseActions(context, usersSetAddr, arcType);
}

void TestRemovePermissionsForUsersSetToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr)
{
  ScIterator5Ptr it5 = context->CreateIterator5(
      userAddr,
      ScType::ConstCommonArc,
      actionClassAddr,
      ScType::ConstTempPosArc,
      ScKeynodes::nrel_users_set_action_class);
  EXPECT_TRUE(it5->Next());
  ScAddr const & arcAddr = it5->Get(3);
  context->EraseElement(arcAddr);
}

void TestRemovePermissionsForUsersSetToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  TestRemovePermissionsForUsersSetToInitActions(context, userAddr, ScKeynodes::action_read_from_sc_memory);
}

ScAddr TestGenerateClassForUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & setAddr = context->GenerateNode(ScType::ConstNode);
  context->GenerateConnector(arcType, setAddr, userAddr);
  return setAddr;
}

ScAddr TestAddClassForUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  return context->GenerateConnector(arcType, usersSetAddr, userAddr);
}

void TestAuthenticationRequestUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  context->GenerateConnector(arcType, ScKeynodes::concept_authentication_request_user, userAddr);
}

void TestReadActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  ScAddr const & arcAddr = context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr);

  EXPECT_TRUE(userContext.IsElement(nodeAddr));
  EXPECT_EQ(userContext.GetElementEdgesAndIncomingArcsCount(nodeAddr), 0u);
  EXPECT_EQ(userContext.GetElementEdgesAndOutgoingArcsCount(nodeAddr), 1u);
  EXPECT_EQ(userContext.GetArcSourceElement(arcAddr), nodeAddr);
  EXPECT_EQ(userContext.GetArcTargetElement(arcAddr), linkAddr);
  auto [nodeAddr1, nodeAddr2] = userContext.GetConnectorIncidentElements(arcAddr);
  EXPECT_EQ(nodeAddr1, nodeAddr);
  EXPECT_EQ(nodeAddr2, linkAddr);
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::ConstNode);
  EXPECT_NO_THROW(userContext.CalculateStatistics());
  std::string content;
  EXPECT_FALSE(userContext.GetLinkContent(linkAddr, content));
  EXPECT_TRUE(content.empty());
  EXPECT_NO_THROW(userContext.SearchLinksByContent("test"));
  EXPECT_NO_THROW(userContext.SearchLinksByContentSubstring("test"));
  EXPECT_NO_THROW(userContext.SearchLinksContentsByContentSubstring("test"));
  EXPECT_NO_THROW(userContext.SearchElementBySystemIdentifier("test"));
  EXPECT_NO_THROW(userContext.ResolveElementSystemIdentifier("test"));
}

void TestReadActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  ScAddr const & arcAddr = context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr);

  EXPECT_THROW(userContext.IsElement(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementEdgesAndIncomingArcsCount(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementEdgesAndOutgoingArcsCount(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetArcSourceElement(arcAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetArcTargetElement(arcAddr), utils::ExceptionInvalidState);
  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_THROW(userContext.GetConnectorIncidentElements(arcAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementType(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CalculateStatistics(), utils::ExceptionInvalidState);
  std::string content;
  EXPECT_THROW(userContext.GetLinkContent(linkAddr, content), utils::ExceptionInvalidState);
}

void TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(
    std::unique_ptr<ScAgentContext> const & context,
    TestScMemoryContext & userContext)
{
  TestReadActionsUnsuccessfully(context, userContext);

  EXPECT_THROW(userContext.GenerateNode(ScType::ConstNode), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GenerateLink(ScType::ConstNodeLink), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.SearchLinksByContent("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.SearchLinksByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.SearchLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.SearchElementBySystemIdentifier("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.ResolveElementSystemIdentifier("test"), utils::ExceptionInvalidState);

  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.CreateIterator3(nodeAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_THROW(it3->Next(), utils::ExceptionInvalidState);

  ScIterator5Ptr it5 = userContext.CreateIterator5(
      nodeAddr, ScType::ConstTempPosArc, ScType::Unknown, ScType::ConstTempPosArc, ScType::ConstNodeRole);
  EXPECT_THROW(it5->Next(), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.CheckConnector(nodeAddr, nodeAddr, ScType::ConstTempPosArc), utils::ExceptionInvalidState);
}

void TestIteratorsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.CreateIterator3(nodeAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_TRUE(it3->Next());

  ScIterator5Ptr it5 = userContext.CreateIterator5(
      nodeAddr, ScType::ConstTempPosArc, ScType::Unknown, ScType::ConstTempPosArc, ScType::ConstNodeRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_TRUE(userContext.CheckConnector(nodeAddr, linkAddr, ScType::ConstTempPosArc));
}

void TestIteratorsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.CreateIterator3(nodeAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_FALSE(it3->Next());

  ScIterator5Ptr it5 = userContext.CreateIterator5(
      nodeAddr, ScType::ConstTempPosArc, ScType::Unknown, ScType::ConstTempPosArc, ScType::ConstNodeRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_FALSE(userContext.CheckConnector(nodeAddr, linkAddr, ScType::ConstTempPosArc));
}

void TestWriteActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);

  EXPECT_TRUE(userContext.GenerateNode(ScType::ConstNode).IsValid());
  EXPECT_TRUE(userContext.GenerateLink(ScType::ConstNodeLink).IsValid());
  EXPECT_TRUE(userContext.GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr).IsValid());

  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::ConstNode));
  EXPECT_TRUE(userContext.Save());
}

void TestWriteActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);

  EXPECT_THROW(
      userContext.GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::ConstNode), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.Save(), utils::ExceptionInvalidState);
}

void TestEraseActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);

  EXPECT_TRUE(userContext.EraseElement(nodeAddr));
}

void TestEraseActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::ConstNode);

  EXPECT_THROW(userContext.EraseElement(nodeAddr), utils::ExceptionInvalidState);
}

void TestApplyActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::ConstNodeLink);

  EXPECT_NO_THROW(userContext.SetLinkContent(linkAddr, "test"));
}

void TestApplyActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::ConstNodeLink);

  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test"), utils::ExceptionInvalidState);
}

void TestChangeActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::ConstNodeLink);

  EXPECT_NO_THROW(userContext.SetElementSystemIdentifier("test", linkAddr));
  EXPECT_NO_THROW(userContext.ResolveElementSystemIdentifier("test", ScType::ConstNode));
}

void TestChangeActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::ConstNodeLink);

  EXPECT_THROW(userContext.SetElementSystemIdentifier("test", linkAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.ResolveElementSystemIdentifier("test", ScType::ConstNode), utils::ExceptionInvalidState);
}

void TestActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  TestReadActionsSuccessfully(context, userContext);
  TestWriteActionsSuccessfully(context, userContext);
  TestEraseActionsSuccessfully(context, userContext);
  TestApplyActionsSuccessfully(context, userContext);
  TestChangeActionsSuccessfully(context, userContext);
}

void TestActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  TestReadActionsUnsuccessfully(context, userContext);
  TestWriteActionsUnsuccessfully(context, userContext);
  TestEraseActionsUnsuccessfully(context, userContext);
  TestApplyActionsUnsuccessfully(context, userContext);
  TestChangeActionsUnsuccessfully(context, userContext);
}

TEST_F(ScMemoryTestWithUserMode, GetUserAddrFromContext)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  EXPECT_EQ(userContext.GetUser(), userAddr);
}

TEST_F(ScMemoryTestWithUserMode, GetGuestUserAddrFromContext)
{
  TestScMemoryContext userContext;
  EXPECT_TRUE(userContext.GetUser().IsValid());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByGuestUser)
{
  TestScMemoryContext userContext;
  TestActionsUnsuccessfully(m_ctx, userContext);
}

void TestSetIdentifiedUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & guestUserAddr,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & arcAddr = context->GenerateConnector(ScType::ConstCommonArc, guestUserAddr, userAddr);
  context->GenerateConnector(ScType::ConstTempNegArc, ScKeynodes::nrel_identified_user, arcAddr);

  auto eventWaiter = context->CreateConditionWaiter<ScEventBeforeEraseIncomingArc<ScType::ConstTempNegArc>>(
      arcAddr,
      [&]() -> void
      {
        context->GenerateConnector(arcType, ScKeynodes::nrel_identified_user, arcAddr);
      },
      [&](ScEventBeforeEraseIncomingArc<ScType::ConstTempNegArc> const & event) -> bool
      {
        return event.GetArcSourceElement() == ScKeynodes::nrel_identified_user;
      });

  eventWaiter->Wait(200);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByIdentifiedUser)
{
  TestScMemoryContext userContext;

  ScAddr const & guestUserAddr = userContext.GetUser();
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestSetIdentifiedUser(m_ctx, guestUserAddr, userAddr);

  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByUnauthenticatedUser)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  TestActionsUnsuccessfully(m_ctx, userContext);
  TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserGeneratedBefore)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserGeneratedBeforeAndHavingClassWithPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });

  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr);
  TestAddAllPermissionsForUsersSetToInitActions(m_ctx, usersSetAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserGeneratedBeforeAndBoughtClassWithPermissionsAndSoldThisClassAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestActionsUnsuccessfully(m_ctx, userContext);
            isAuthenticated = true;
          });

  ScAddr const & usersSetAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestAddAllPermissionsForUsersSetToInitActions(m_ctx, usersSetAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);
  TestAddClassForUser(m_ctx, userAddr, usersSetAddr, ScType::ConstTempNegArc);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());

  ScAddr arcAddr;
  std::atomic_bool isChecked = false;
  {
    auto eventSubscriptionForErasure =
        m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::MembershipArc>>(
            usersSetAddr,
            [this, &userContext, &isChecked](ScEventBeforeEraseOutgoingArc<ScType::MembershipArc> const &)
            {
              TestActionsSuccessfully(m_ctx, userContext);
              isChecked = true;
            });

    arcAddr = TestAddClassForUser(m_ctx, userAddr, usersSetAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
    EXPECT_TRUE(isChecked.load());
  }

  isChecked = false;
  eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
      usersSetAddr,
      [this, &userContext, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
      {
        EXPECT_EQ(event.GetArcType(), ScType::ConstTempNegArc);

        TestActionsUnsuccessfully(m_ctx, userContext);
        isChecked = true;
      });

  m_ctx->EraseElement(arcAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByInvalidConnectorToUser)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
          {
            isChecked = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr, ScType::ConstTempNegArc);
  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::ConstTempNegArc);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::ConstCommonArc);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
      ScKeynodes::concept_authenticated_user,
      [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
      {
        isChecked = false;
      });

  ScIterator3Ptr it3 = m_ctx->CreateIterator3(ScKeynodes::concept_authenticated_user, ScType::ConstCommonArc, userAddr);
  while (it3->Next())
  {
    m_ctx->EraseElement(it3->Get(2));
  }

  SC_LOCK_WAIT_WHILE_TRUE(isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByInvalidConnectorToUsersSet)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
          {
            isChecked = true;
          });

  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr, ScType::ConstTempNegArc);
  TestAddAllPermissionsForUsersSetToInitActions(m_ctx, usersSetAddr, ScType::ConstTempNegArc);
  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::ConstTempNegArc);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::ConstCommonArc);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
      ScKeynodes::concept_authenticated_user,
      [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
      {
        isChecked = false;
      });

  ScIterator3Ptr it3 = m_ctx->CreateIterator3(ScKeynodes::concept_authenticated_user, ScType::ConstCommonArc, userAddr);
  while (it3->Next())
  {
    m_ctx->EraseElement(it3->Get(2));
  }

  SC_LOCK_WAIT_WHILE_TRUE(isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByUserWithInvalidConnectorsToPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &userContext, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
          {
            TestActionsUnsuccessfully(m_ctx, userContext);
            TestIteratorsUnsuccessfully(m_ctx, userContext);
            isChecked = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr, ScType::ConstTempNegArc);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserGeneratedAfter)
{
  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestScMemoryContext userContext{event.GetUser()};
            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isChecked = true;
          });

  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, SeveralHandleElementsByAuthenticatedUserGeneratedAfter)
{
  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [this, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestScMemoryContext userContext{event.GetUser()};
            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isChecked = true;
          });

  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  {
    TestScMemoryContext userContext{userAddr};
    TestActionsSuccessfully(m_ctx, userContext);
    TestIteratorsSuccessfully(m_ctx, userContext);
  }
  {
    TestScMemoryContext userContext{userAddr};
    TestActionsSuccessfully(m_ctx, userContext);
    TestIteratorsSuccessfully(m_ctx, userContext);
  }
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserGeneratedBeforeAndUnauthenticatedAfterAndAuthenticatedAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  std::atomic_bool isAuthenticated = false;
  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

              TestActionsSuccessfully(m_ctx, userContext);
              TestIteratorsSuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });

    TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempNegArc);

              TestActionsUnsuccessfully(m_ctx, userContext);
              TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(m_ctx, userContext);

              isAuthenticated = false;
            });

    ScIterator3Ptr const it3 =
        m_ctx->CreateIterator3(ScKeynodes::concept_authenticated_user, ScType::ConstTempPosArc, userAddr);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    SC_LOCK_WAIT_WHILE_TRUE(isAuthenticated.load());
    EXPECT_FALSE(isAuthenticated.load());
  }

  EXPECT_TRUE(m_ctx->CheckConnector(ScKeynodes::concept_authenticated_user, userAddr, ScType::ConstTempNegArc));

  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

              TestActionsSuccessfully(m_ctx, userContext);
              TestIteratorsSuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });

    TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  EXPECT_FALSE(m_ctx->CheckConnector(ScKeynodes::concept_authenticated_user, userAddr, ScType::ConstTempNegArc));
  EXPECT_TRUE(m_ctx->CheckConnector(ScKeynodes::concept_authenticated_user, userAddr, ScType::ConstTempPosArc));
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutWriteAndErasePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsSuccessfully(m_ctx, userContext);
            TestWriteActionsUnsuccessfully(m_ctx, userContext);
            TestEraseActionsUnsuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutErasePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsSuccessfully(m_ctx, userContext);
            TestWriteActionsSuccessfully(m_ctx, userContext);
            TestEraseActionsUnsuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActions(m_ctx, userAddr);
  TestAddPermissionsForUserToInitWriteActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutWritePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsSuccessfully(m_ctx, userContext);
            TestWriteActionsUnsuccessfully(m_ctx, userContext);
            TestEraseActionsSuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActions(m_ctx, userAddr);
  TestAddPermissionsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutReadAndErasePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsUnsuccessfully(m_ctx, userContext);
            TestWriteActionsSuccessfully(m_ctx, userContext);
            TestEraseActionsUnsuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitWriteActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutReadPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsUnsuccessfully(m_ctx, userContext);
            TestWriteActionsSuccessfully(m_ctx, userContext);
            TestEraseActionsSuccessfully(m_ctx, userContext);
            TestApplyActionsSuccessfully(m_ctx, userContext);
            TestChangeActionsSuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitWriteActions(m_ctx, userAddr);
  TestAddPermissionsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutReadAndWritePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsUnsuccessfully(m_ctx, userContext);
            TestWriteActionsUnsuccessfully(m_ctx, userContext);
            TestEraseActionsSuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithReadPermissionsAndWithoutReadPermissionsAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  TestScMemoryContext userContext{userAddr};
  {
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

              TestReadActionsSuccessfully(m_ctx, userContext);
              TestWriteActionsUnsuccessfully(m_ctx, userContext);
              TestEraseActionsUnsuccessfully(m_ctx, userContext);
              TestApplyActionsUnsuccessfully(m_ctx, userContext);
              TestChangeActionsUnsuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });
    TestAddPermissionsForUserToInitReadActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  std::atomic_bool isPermissionsUpdated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::nrel_user_action_class,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempNegArc);

            TestReadActionsUnsuccessfully(m_ctx, userContext);
            TestWriteActionsUnsuccessfully(m_ctx, userContext);
            TestEraseActionsUnsuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isPermissionsUpdated = true;
          });

  TestRemovePermissionsForUserToInitReadActions(m_ctx, userAddr);
  SC_LOCK_WAIT_WHILE_TRUE(!isPermissionsUpdated.load());
  EXPECT_TRUE(isPermissionsUpdated.load());
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserHavingClassWithReadPermissionsAndWithoutReadPermissionsAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr);

  TestScMemoryContext userContext{userAddr};
  {
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

              TestReadActionsSuccessfully(m_ctx, userContext);
              TestWriteActionsUnsuccessfully(m_ctx, userContext);
              TestEraseActionsUnsuccessfully(m_ctx, userContext);
              TestApplyActionsUnsuccessfully(m_ctx, userContext);
              TestChangeActionsUnsuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });
    TestAddPermissionsForUsersSetToInitReadActions(m_ctx, usersSetAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  std::atomic_bool isPermissionsUpdated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::nrel_users_set_action_class,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempNegArc);

            TestReadActionsUnsuccessfully(m_ctx, userContext);
            TestWriteActionsUnsuccessfully(m_ctx, userContext);
            TestEraseActionsUnsuccessfully(m_ctx, userContext);
            TestApplyActionsUnsuccessfully(m_ctx, userContext);
            TestChangeActionsUnsuccessfully(m_ctx, userContext);

            isPermissionsUpdated = true;
          });

  TestRemovePermissionsForUsersSetToInitReadActions(m_ctx, usersSetAddr);
  SC_LOCK_WAIT_WHILE_TRUE(!isPermissionsUpdated.load());
  EXPECT_TRUE(isPermissionsUpdated.load());
}

void TestReadWriteEraseAccessedElementUnsuccessfully(
    std::unique_ptr<ScAgentContext> const &,
    TestScMemoryContext & userContext,
    sc_addr const & addr)
{
  ScAddr const & elementAddr{addr};

  EXPECT_THROW(userContext.EraseElement(elementAddr), utils::ExceptionInvalidState);

  ScIterator3Ptr it3 = userContext.CreateIterator3(elementAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_FALSE(it3->Next());

  ScAddr const & nodeAddr = userContext.GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(nodeAddr.IsValid());

  EXPECT_THROW(
      userContext.GenerateConnector(ScType::ConstTempPosArc, elementAddr, nodeAddr), utils::ExceptionInvalidState);
  EXPECT_TRUE(userContext.GenerateConnector(ScType::ConstCommonArc, elementAddr, nodeAddr).IsValid());

  it3 = userContext.CreateIterator3(elementAddr, ScType::ConstCommonArc, nodeAddr);
  EXPECT_TRUE(it3->Next());
}

void TestReadWriteEraseAccessedAllElementsUnsuccessfully(
    std::unique_ptr<ScAgentContext> const & context,
    TestScMemoryContext & userContext)
{
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, concept_authentication_request_user_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, concept_authenticated_user_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, nrel_user_action_class_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_read_from_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_generate_in_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_erase_from_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_read_permissions_from_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_generate_permissions_in_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_erase_permissions_from_sc_memory_addr);
}

void TestReadWriteAccessedElementSuccessfully(
    std::unique_ptr<ScAgentContext> const &,
    TestScMemoryContext & userContext,
    sc_addr const addr)
{
  ScAddr const & elementAddr{addr};

  ScAddr const & nodeAddr = userContext.GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(nodeAddr.IsValid());

  EXPECT_TRUE(userContext.GenerateConnector(ScType::ConstTempPosArc, elementAddr, nodeAddr).IsValid());

  ScIterator3Ptr it3 = userContext.CreateIterator3(elementAddr, ScType::ConstTempPosArc, nodeAddr);
  EXPECT_TRUE(it3->Next());

  EXPECT_TRUE(userContext.GenerateConnector(ScType::ConstCommonArc, elementAddr, nodeAddr).IsValid());

  it3 = userContext.CreateIterator3(elementAddr, ScType::ConstCommonArc, nodeAddr);
  EXPECT_TRUE(it3->Next());
}

void TestReadWriteEraseAccessedAllElementsSuccessfully(
    std::unique_ptr<ScAgentContext> const & context,
    TestScMemoryContext & userContext)
{
  TestReadWriteAccessedElementSuccessfully(context, userContext, concept_guest_user_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, nrel_identified_user_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, concept_authentication_request_user_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, concept_authenticated_user_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, nrel_user_action_class_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, nrel_users_set_action_class_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_read_from_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_generate_in_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_erase_from_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_read_permissions_from_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_generate_permissions_in_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_erase_permissions_from_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, nrel_user_action_class_within_sc_structure_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, nrel_users_set_action_class_within_sc_structure_addr);
}

void TestAddPermissionsToHandleReadPermissions(std::unique_ptr<ScAgentContext> const & context, ScAddr const & userAddr)
{
  TestAddPermissionsForUserToInitActions(context, userAddr, ScKeynodes::action_read_permissions_from_sc_memory);
}

void TestAddPermissionsToHandleWritePermissions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  TestAddPermissionsForUserToInitActions(context, userAddr, ScKeynodes::action_generate_permissions_in_sc_memory);
}

void TestAddPermissionsToHandleErasePermissions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  TestAddPermissionsForUserToInitActions(context, userAddr, ScKeynodes::action_erase_permissions_from_sc_memory);
}

void TestAddAllPermissionsToHandleAllPermissions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  TestAddPermissionsToHandleReadPermissions(context, userAddr);
  TestAddPermissionsToHandleWritePermissions(context, userAddr);
  TestAddPermissionsToHandleErasePermissions(context, userAddr);
}

TEST_F(ScMemoryTestWithUserMode, HandleAccessedElementsByAuthenticatedUserWithoutPermissionsForPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  {
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

              TestActionsSuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });

    TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  TestReadWriteEraseAccessedAllElementsUnsuccessfully(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleAccessedElementsByAuthenticatedUserWithPermissionsForPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestScMemoryContext userContext{userAddr};

  {
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

              TestActionsSuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });

    TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
    TestAddAllPermissionsToHandleAllPermissions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  TestReadWriteEraseAccessedAllElementsSuccessfully(m_ctx, userContext);
}

void TestAddPermissionsForUserToInitActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & arcBetweenActionAndStructureAddr =
      context->GenerateConnector(ScType::ConstCommonArc, actionClassAddr, structureAddr);
  ScAddr const & arcAddr =
      context->GenerateConnector(ScType::ConstCommonArc, userAddr, arcBetweenActionAndStructureAddr);
  context->GenerateConnector(arcType, ScKeynodes::nrel_user_action_class_within_sc_structure, arcAddr);
}

void TestAddPermissionsForUserToInitReadActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUserToInitActionsWithinStructure(
      context, userAddr, ScKeynodes::action_read_from_sc_memory, structureAddr, arcType);
}

void TestAddPermissionsForUserToInitWriteActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUserToInitActionsWithinStructure(
      context, userAddr, ScKeynodes::action_generate_in_sc_memory, structureAddr, arcType);
}

void TestAddPermissionsForUserToInitEraseActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUserToInitActionsWithinStructure(
      context, userAddr, ScKeynodes::action_erase_from_sc_memory, structureAddr, arcType);
}

void TestAddPermissionsForUsersSetToInitActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScAddr const & actionClassAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  ScAddr const & arcBetweenActionAndStructureAddr =
      context->GenerateConnector(ScType::ConstCommonArc, actionClassAddr, structureAddr);
  ScAddr const & arcAddr =
      context->GenerateConnector(ScType::ConstCommonArc, usersSetAddr, arcBetweenActionAndStructureAddr);
  context->GenerateConnector(arcType, ScKeynodes::nrel_users_set_action_class_within_sc_structure, arcAddr);
}

void TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::ConstTempPosArc)
{
  TestAddPermissionsForUsersSetToInitActionsWithinStructure(
      context, usersSetAddr, ScKeynodes::action_read_from_sc_memory, structureAddr, arcType);
}

void TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr,
    ScAddr const & arcAddr,
    ScAddr const & linkAddr,
    ScAddr const & relationEdgeAddr,
    ScAddr const & relationAddr)
{
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::ConstNode);

  ScIterator3Ptr it3 = userContext.CreateIterator3(nodeAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  EXPECT_EQ(it3->Get(1), arcAddr);
  EXPECT_EQ(it3->Get(2), linkAddr);

  auto [sourceAddr, targetAddr] = userContext.GetConnectorIncidentElements(arcAddr);
  EXPECT_EQ(nodeAddr, sourceAddr);
  EXPECT_EQ(linkAddr, targetAddr);
  EXPECT_EQ(userContext.GetArcSourceElement(arcAddr), nodeAddr);
  EXPECT_EQ(userContext.GetArcTargetElement(arcAddr), linkAddr);
  std::string content;
  EXPECT_FALSE(userContext.GetLinkContent(linkAddr, content));
  EXPECT_TRUE(content.empty());

  EXPECT_FALSE(it3->Next());

  it3 = userContext.CreateIterator3(nodeAddr, arcAddr, linkAddr);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  EXPECT_EQ(it3->Get(1), arcAddr);
  EXPECT_EQ(it3->Get(2), linkAddr);

  EXPECT_FALSE(it3->Next());

  auto const & TestIterator5Successfully = [&](ScIterator5Ptr const & it5)
  {
    EXPECT_TRUE(it5->Next());
    EXPECT_EQ(it5->Get(0), nodeAddr);
    EXPECT_EQ(it5->Get(1), arcAddr);
    EXPECT_EQ(it5->Get(2), linkAddr);
    EXPECT_EQ(it5->Get(3), relationEdgeAddr);
    EXPECT_EQ(it5->Get(4), relationAddr);

    EXPECT_FALSE(it5->Next());
  };

  ScIterator5Ptr it5 = userContext.CreateIterator5(
      nodeAddr, ScType::ConstTempPosArc, ScType::ConstNodeLink, ScType::ConstTempPosArc, ScType::ConstNodeRole);
  TestIterator5Successfully(it5);

  it5 = userContext.CreateIterator5(
      nodeAddr, ScType::ConstTempPosArc, linkAddr, ScType::ConstTempPosArc, ScType::ConstNodeRole);
  TestIterator5Successfully(it5);

  it5 = userContext.CreateIterator5(nodeAddr, ScType::ConstTempPosArc, linkAddr, ScType::ConstTempPosArc, relationAddr);
  TestIterator5Successfully(it5);

  it5 = userContext.CreateIterator5(
      ScType::ConstNode, ScType::ConstTempPosArc, linkAddr, ScType::ConstTempPosArc, relationAddr);
  TestIterator5Successfully(it5);

  it5 = userContext.CreateIterator5(
      ScType::ConstNode, ScType::ConstTempPosArc, ScType::ConstNodeLink, ScType::ConstTempPosArc, relationAddr);
  TestIterator5Successfully(it5);

  it5 = userContext.CreateIterator5(
      ScType::ConstNode, ScType::ConstTempPosArc, linkAddr, ScType::ConstTempPosArc, ScType::ConstNodeRole);
  TestIterator5Successfully(it5);
}

void TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.GetElementType(nodeAddr), utils::ExceptionInvalidState);

  ScIterator3Ptr const it3 = userContext.CreateIterator3(nodeAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_FALSE(it3->Next());
}

void TestWriteActionsWithinStructureSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & structureAddr,
    ScAddr const & nodeAddr)
{
  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::ConstNode));

  ScAddr const & linkAddr1 = userContext.GenerateLink(ScType::ConstNodeLink);
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::ConstTempPosArc, structureAddr, linkAddr1));
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr1));

  ScAddr const & linkAddr2 = userContext.GenerateLink(ScType::ConstNodeLink);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr2), utils::ExceptionInvalidState);
}

void TestWriteActionsWithinStructureUnsuccessfully(TestScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::ConstNode), utils::ExceptionInvalidState);

  ScAddr const & linkAddr = userContext.GenerateLink(ScType::ConstNodeLink);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::ConstTempPosArc, linkAddr, nodeAddr), utils::ExceptionInvalidState);
}

void TestEraseActionsWithinStructureSuccessfully(TestScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_TRUE(userContext.EraseElement(nodeAddr));
}

void TestEraseActionsWithinStructureUnsuccessfully(TestScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.EraseElement(nodeAddr), utils::ExceptionInvalidState);
}

void TestChangeActionsWithinStructureSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & structureAddr,
    ScAddr const & nodeAddr)
{
  ScAddr const & linkAddr1 = userContext.GenerateLink(ScType::ConstNodeLink);
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::ConstTempPosArc, structureAddr, linkAddr1));
  EXPECT_TRUE(userContext.SetLinkContent(linkAddr1, "test_content"));
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr1));

  ScAddr const & linkAddr2 = userContext.GenerateLink(ScType::ConstNodeLink);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::ConstTempPosArc, nodeAddr, linkAddr2), utils::ExceptionInvalidState);
}

void TestChangeActionsWithinStructureUnsuccessfully(TestScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  ScAddr const & linkAddr = userContext.GenerateLink(ScType::ConstNodeLink);
  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test_content"), utils::ExceptionInvalidState);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::ConstTempPosArc, linkAddr, nodeAddr), utils::ExceptionInvalidState);
}

ScAddr TestGenerateStructureWithConnectorAndIncidentElements(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr & nodeAddr1,
    ScAddr & arcAddr,
    ScAddr & linkAddr,
    ScAddr & relationEdgeAddr,
    ScAddr & relationAddr,
    ScAddr & nodeAddr2)
{
  nodeAddr1 = context->GenerateNode(ScType::ConstNode);
  linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  arcAddr = context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, linkAddr);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, linkAddr);
  relationAddr = context->GenerateNode(ScType::ConstNodeRole);
  relationEdgeAddr = context->GenerateConnector(ScType::ConstTempPosArc, relationAddr, arcAddr);
  context->GenerateConnector(ScType::ConstTempPosArc, relationAddr, arcAddr);

  nodeAddr2 = context->GenerateNode(ScType::ConstNode);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::ConstNodeStructure);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << nodeAddr1 << arcAddr << linkAddr << relationEdgeAddr << relationAddr;

  return structureAddr;
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
          {
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                userContext, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr);
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserHavingClassWithLocalReadPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
          {
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                userContext, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr);
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

            isAuthenticated = true;
          });
  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr);
  TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(m_ctx, usersSetAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserBoughtClassWithLocalReadPermissionsAndSoldThisClassAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
          {
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            isAuthenticated = true;
          });
  ScAddr const & usersSetAddr = m_ctx->GenerateNode(ScType::ConstNode);
  TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(m_ctx, usersSetAddr, structureAddr);
  TestAddClassForUser(m_ctx, userAddr, usersSetAddr, ScType::ConstTempNegArc);

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());

  ScAddr usersSetEdgeAddr;
  std::atomic_bool isChecked = false;
  {
    auto eventSubscriptionForErasure =
        m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::MembershipArc>>(
            usersSetAddr,
            [&](ScEventBeforeEraseOutgoingArc<ScType::MembershipArc> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              isChecked = true;
            });

    usersSetEdgeAddr = TestAddClassForUser(m_ctx, userAddr, usersSetAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
    EXPECT_TRUE(isChecked.load());
  }

  isChecked = false;
  eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
      usersSetAddr,
      [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
      {
        EXPECT_EQ(event.GetArcType(), ScType::ConstTempNegArc);

        TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
        TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        isChecked = true;
      });

  m_ctx->EraseElement(usersSetEdgeAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalWritePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
            TestWriteActionsWithinStructureSuccessfully(userContext, structureAddr, nodeAddr1);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitWriteActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalErasePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureSuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitEraseActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalWriteAndErasePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
            TestWriteActionsWithinStructureSuccessfully(userContext, structureAddr, nodeAddr1);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestChangeActionsWithinStructureSuccessfully(userContext, structureAddr, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestEraseActionsWithinStructureSuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitWriteActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAddPermissionsForUserToInitEraseActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadPermissionsAndWithoutAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                  userContext, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr);
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

              isAuthenticated = true;
            });
    TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  ScIterator3Ptr it3 =
      m_ctx->CreateIterator3(ScKeynodes::concept_authenticated_user, ScType::ConstTempPosArc, userAddr);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  isAuthenticated = false;

  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

              isAuthenticated = true;
            });

    it3 = m_ctx->CreateIterator3(
        ScKeynodes::nrel_user_action_class_within_sc_structure, ScType::ConstTempPosArc, ScType::ConstCommonArc);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr, ScType::ConstTempNegArc);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserHavingClassWithLocalReadPermissionsAndWithoutAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr);

  ScAddr nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                  userContext, nodeAddr1, arcAddr, linkAddr, relationEdgeAddr, relationAddr);
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

              isAuthenticated = true;
            });
    TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(m_ctx, usersSetAddr, structureAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  ScIterator3Ptr it3 =
      m_ctx->CreateIterator3(ScKeynodes::concept_authenticated_user, ScType::ConstTempPosArc, userAddr);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  isAuthenticated = false;

  {
    auto eventSubscription =
        m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
            ScKeynodes::concept_authenticated_user,
            [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

              isAuthenticated = true;
            });

    it3 = m_ctx->CreateIterator3(
        ScKeynodes::nrel_users_set_action_class_within_sc_structure, ScType::ConstTempPosArc, ScType::ConstCommonArc);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(
        m_ctx, usersSetAddr, structureAddr, ScType::ConstTempNegArc);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }
}

ScAddr TestGenerateStructureWithConnectorAndSource(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr & nodeAddr1,
    ScAddr & nodeAddr2)
{
  nodeAddr1 = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  ScAddr const & arcAddr1 = context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, linkAddr);
  nodeAddr2 = context->GenerateNode(ScType::ConstNode);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::ConstNodeStructure);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << nodeAddr1 << arcAddr1;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorAndSourceSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::ConstNode);

  ScIterator3Ptr it3 = userContext.CreateIterator3(nodeAddr, ScType::ConstTempPosArc, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  ScAddr const & arcAddr = it3->Get(1);
  EXPECT_THROW(it3->Get(2), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.GetConnectorIncidentElements(arcAddr), utils::ExceptionInvalidState);
  EXPECT_EQ(userContext.GetArcSourceElement(arcAddr), nodeAddr);
  EXPECT_THROW(userContext.GetArcTargetElement(arcAddr), utils::ExceptionInvalidState);

  EXPECT_FALSE(it3->Next());

  it3 = userContext.CreateIterator3(nodeAddr, arcAddr, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  EXPECT_EQ(it3->Get(1), arcAddr);
  EXPECT_THROW(it3->Get(2), utils::ExceptionInvalidState);

  EXPECT_FALSE(it3->Next());
}

void TestReadActionsWithinStructureWithConnectorAndSourceUnsuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr);
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserWithLocalReadPermissionsAndWithStructureWithConnectorAndSourceElement)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr nodeAddr1, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndSource(m_ctx, nodeAddr1, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithConnectorAndSourceSuccessfully(userContext, nodeAddr1);
            TestReadActionsWithinStructureWithConnectorAndSourceUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

ScAddr TestGenerateStructureWithConnectorAndTarget(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr & linkAddr,
    ScAddr & nodeAddr2)
{
  ScAddr const & nodeAddr1 = context->GenerateNode(ScType::ConstNode);
  linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  ScAddr const & arcAddr1 = context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, linkAddr);
  nodeAddr2 = context->GenerateNode(ScType::ConstNode);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::ConstNodeStructure);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << arcAddr1 << linkAddr;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorAndTargetSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & linkAddr)
{
  EXPECT_EQ(userContext.GetElementType(linkAddr), ScType::ConstNodeLink);

  ScIterator3Ptr it3 = userContext.CreateIterator3(ScType::Unknown, ScType::ConstTempPosArc, linkAddr);
  EXPECT_TRUE(it3->Next());
  EXPECT_THROW(it3->Get(0), utils::ExceptionInvalidState);
  ScAddr const & arcAddr = it3->Get(1);
  EXPECT_EQ(it3->Get(2), linkAddr);

  ScAddr sourceAddr, targetAddr;
  EXPECT_THROW(userContext.GetConnectorIncidentElements(arcAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetArcSourceElement(arcAddr), utils::ExceptionInvalidState);
  EXPECT_EQ(userContext.GetArcTargetElement(arcAddr), linkAddr);

  EXPECT_FALSE(it3->Next());

  it3 = userContext.CreateIterator3(ScType::Unknown, arcAddr, linkAddr);
  EXPECT_TRUE(it3->Next());
  EXPECT_THROW(it3->Get(0), utils::ExceptionInvalidState);
  EXPECT_EQ(it3->Get(1), arcAddr);
  EXPECT_EQ(it3->Get(2), linkAddr);

  EXPECT_FALSE(it3->Next());
}

void TestReadActionsWithinStructureWithConnectorAndTargetUnsuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr);
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserWithLocalReadPermissionsAndWithStructureWithConnectorAndTargetElement)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr linkAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndTarget(m_ctx, linkAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithConnectorAndTargetSuccessfully(userContext, linkAddr);
            TestReadActionsWithinStructureWithConnectorAndTargetUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

ScAddr TestGenerateStructureWithConnector(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr & arcAddr,
    ScAddr & nodeAddr2)
{
  ScAddr const & nodeAddr1 = context->GenerateNode(ScType::ConstNode);
  ScAddr const & linkAddr = context->GenerateLink(ScType::ConstNodeLink);
  arcAddr = context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, linkAddr);
  nodeAddr2 = context->GenerateNode(ScType::ConstNode);
  context->GenerateConnector(ScType::ConstTempPosArc, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::ConstNodeStructure);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << arcAddr;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorSuccessfully(TestScMemoryContext & userContext, ScAddr const & arcAddr)
{
  EXPECT_EQ(userContext.GetElementType(arcAddr), ScType::ConstTempPosArc);

  ScIterator3Ptr it3 = userContext.CreateIterator3(ScType::Unknown, arcAddr, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_THROW(it3->Get(0), utils::ExceptionInvalidState);
  EXPECT_EQ(it3->Get(1), arcAddr);
  EXPECT_THROW(it3->Get(2), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.GetConnectorIncidentElements(arcAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetArcSourceElement(arcAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetArcTargetElement(arcAddr), utils::ExceptionInvalidState);

  EXPECT_FALSE(it3->Next());
}

void TestReadActionsWithinStructureWithConnectorUnsuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadPermissionsAndWithStructureWithConnector)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr arcAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnector(m_ctx, arcAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithConnectorSuccessfully(userContext, arcAddr);
            TestReadActionsWithinStructureWithConnectorUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

ScAddr TestGenerateStructureWithLinks(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr & linkAddr1,
    ScAddr & linkAddr2)
{
  linkAddr1 = context->GenerateLink(ScType::ConstNodeLink);
  context->SetLinkContent(linkAddr1, "test_content");
  linkAddr2 = context->GenerateLink(ScType::ConstNodeLink);
  context->SetLinkContent(linkAddr2, "test_content");

  ScAddr const & structureAddr = context->GenerateNode(ScType::ConstNodeStructure);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << linkAddr1;

  return structureAddr;
}

void TestReadActionsWithinStructureWithLinksSuccessfully(TestScMemoryContext & userContext, ScAddr const & linkAddr)
{
  ScAddrSet linkSet = userContext.SearchLinksByContent("test_content");
  EXPECT_EQ(linkSet.size(), 1u);
  EXPECT_TRUE(linkSet.count(linkAddr));

  linkSet = userContext.SearchLinksByContentSubstring("test");
  EXPECT_EQ(linkSet.size(), 1u);
  EXPECT_TRUE(linkSet.count(linkAddr));
}

void TestReadActionsWithinStructureWithLinksUnsuccessfully(TestScMemoryContext & userContext)
{
  EXPECT_THROW(userContext.SearchLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadPermissionsAndWithStructureWithLinks)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScAddr linkAddr1, linkAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithLinks(m_ctx, linkAddr1, linkAddr2);

  TestScMemoryContext userContext{userAddr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
          ScKeynodes::concept_authenticated_user,
          [&](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::ConstTempPosArc);

            TestReadActionsWithinStructureWithLinksSuccessfully(userContext, linkAddr1);
            TestReadActionsWithinStructureWithLinksUnsuccessfully(userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}
