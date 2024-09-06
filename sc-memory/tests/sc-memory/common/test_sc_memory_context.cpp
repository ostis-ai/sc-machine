#include <gtest/gtest.h>

#include <thread>

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_event_subscription.hpp"

#include "sc-memory/sc_memory.hpp"

#include "sc-core/sc_keynodes.h"
#include "sc-memory/sc_structure.hpp"

#include "sc_test.hpp"

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
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelUserActionClassAddr{nrel_user_action_class_addr};
  ScAddr const & edgeAddr = context->GenerateConnector(ScType::EdgeDCommonConst, userAddr, actionClassAddr);
  context->GenerateConnector(arcType, nrelUserActionClassAddr, edgeAddr);
}

void TestAddPermissionsForUserToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, readActionInScMemoryAddr, arcType);
}

void TestAddPermissionsForUserToInitWriteActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_in_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, writeActionInScMemoryAddr, arcType);
}

void TestAddPermissionsForUserToInitEraseActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, eraseActionInScMemoryAddr, arcType);
}

void TestAddAllPermissionsForUserToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
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
  ScAddr const & nrelUserActionClassAddr{nrel_user_action_class_addr};
  ScIterator5Ptr it5 = context->Iterator5(
      userAddr, ScType::EdgeDCommonConst, actionClassAddr, ScType::EdgeAccessConstPosTemp, nrelUserActionClassAddr);
  EXPECT_TRUE(it5->Next());
  ScAddr const & arcAddr = it5->Get(3);
  context->EraseElement(arcAddr);
}

void TestRemovePermissionsForUserToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestRemovePermissionsForUserToInitActions(context, userAddr, readActionInScMemoryAddr);
}

void TestAddPermissionsForUsersSetToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScAddr const & actionClassAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelUsersSetActionClassAddr{nrel_users_set_action_class_addr};
  ScAddr const & edgeAddr = context->GenerateConnector(ScType::EdgeDCommonConst, usersSetAddr, actionClassAddr);
  context->GenerateConnector(arcType, nrelUsersSetActionClassAddr, edgeAddr);
}

void TestAddPermissionsForUsersSetToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddPermissionsForUsersSetToInitActions(context, usersSetAddr, readActionInScMemoryAddr, arcType);
}

void TestAddPermissionsForUsersSetToInitWriteActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_in_sc_memory_addr};
  TestAddPermissionsForUsersSetToInitActions(context, usersSetAddr, writeActionInScMemoryAddr, arcType);
}

void TestAddPermissionsForUsersSetToInitEraseActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_from_sc_memory_addr};
  TestAddPermissionsForUsersSetToInitActions(context, usersSetAddr, eraseActionInScMemoryAddr, arcType);
}

void TestAddAllPermissionsForUsersSetToInitActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
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
  ScAddr const & nrelUsersSetActionClassAddr{nrel_users_set_action_class_addr};
  ScIterator5Ptr it5 = context->Iterator5(
      userAddr, ScType::EdgeDCommonConst, actionClassAddr, ScType::EdgeAccessConstPosTemp, nrelUsersSetActionClassAddr);
  EXPECT_TRUE(it5->Next());
  ScAddr const & arcAddr = it5->Get(3);
  context->EraseElement(arcAddr);
}

void TestRemovePermissionsForUsersSetToInitReadActions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestRemovePermissionsForUsersSetToInitActions(context, userAddr, readActionInScMemoryAddr);
}

ScAddr TestGenerateClassForUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & setAddr = context->GenerateNode(ScType::NodeConst);
  context->GenerateConnector(arcType, setAddr, userAddr);
  return setAddr;
}

ScAddr TestAddClassForUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & usersSetAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  return context->GenerateConnector(arcType, usersSetAddr, userAddr);
}

void TestAuthenticationRequestUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & conceptAuthenticationRequestUserAddr{concept_authentication_request_user_addr};
  context->GenerateConnector(arcType, conceptAuthenticationRequestUserAddr, userAddr);
}

void TestReadActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  ScAddr const & edgeAddr = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  EXPECT_TRUE(userContext.IsElement(nodeAddr));
  EXPECT_EQ(userContext.GetElementInputArcsCount(nodeAddr), 0u);
  EXPECT_EQ(userContext.GetElementOutputArcsCount(nodeAddr), 1u);
  EXPECT_EQ(userContext.GetEdgeSource(edgeAddr), nodeAddr);
  EXPECT_EQ(userContext.GetEdgeTarget(edgeAddr), linkAddr);
  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_TRUE(userContext.GetEdgeInfo(edgeAddr, nodeAddr1, nodeAddr2));
  EXPECT_EQ(nodeAddr1, nodeAddr);
  EXPECT_EQ(nodeAddr2, linkAddr);
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::NodeConst);
  EXPECT_NO_THROW(userContext.CalculateStat());
  std::string content;
  EXPECT_FALSE(userContext.GetLinkContent(linkAddr, content));
  EXPECT_TRUE(content.empty());
  EXPECT_NO_THROW(userContext.FindLinksByContent("test"));
  EXPECT_NO_THROW(userContext.FindLinksByContentSubstring("test"));
  EXPECT_NO_THROW(userContext.FindLinksContentsByContentSubstring("test"));
  EXPECT_NO_THROW(userContext.HelperFindBySystemIdtf("test"));
  EXPECT_NO_THROW(userContext.HelperResolveSystemIdtf("test"));
}

void TestReadActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  ScAddr const & edgeAddr = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  EXPECT_THROW(userContext.IsElement(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementInputArcsCount(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementOutputArcsCount(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeSource(edgeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeTarget(edgeAddr), utils::ExceptionInvalidState);
  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_THROW(userContext.GetEdgeInfo(edgeAddr, nodeAddr1, nodeAddr2), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementType(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CalculateStat(), utils::ExceptionInvalidState);
  std::string content;
  EXPECT_THROW(userContext.GetLinkContent(linkAddr, content), utils::ExceptionInvalidState);
}

void TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(
    std::unique_ptr<ScAgentContext> const & context,
    TestScMemoryContext & userContext)
{
  TestReadActionsUnsuccessfully(context, userContext);

  EXPECT_THROW(userContext.GenerateNode(ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GenerateLink(ScType::LinkConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContent("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperFindBySystemIdtf("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test"), utils::ExceptionInvalidState);

  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_THROW(it3->Next(), utils::ExceptionInvalidState);

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_THROW(it5->Next(), utils::ExceptionInvalidState);

  EXPECT_THROW(
      userContext.HelperCheckEdge(nodeAddr, nodeAddr, ScType::EdgeAccessConstPosTemp), utils::ExceptionInvalidState);
}

void TestIteratorsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_TRUE(it3->Next());

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_TRUE(userContext.HelperCheckEdge(nodeAddr, linkAddr, ScType::EdgeAccessConstPosTemp));
}

void TestIteratorsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_FALSE(it3->Next());

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_FALSE(userContext.HelperCheckEdge(nodeAddr, linkAddr, ScType::EdgeAccessConstPosTemp));
}

void TestWriteActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);

  EXPECT_TRUE(userContext.GenerateNode(ScType::NodeConst).IsValid());
  EXPECT_TRUE(userContext.GenerateLink(ScType::LinkConst).IsValid());
  EXPECT_TRUE(userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr).IsValid());

  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst));
  EXPECT_TRUE(userContext.Save());
}

void TestWriteActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);

  EXPECT_THROW(
      userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.Save(), utils::ExceptionInvalidState);
}

void TestEraseActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);

  EXPECT_TRUE(userContext.EraseElement(nodeAddr));
}

void TestEraseActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->GenerateNode(ScType::NodeConst);

  EXPECT_THROW(userContext.EraseElement(nodeAddr), utils::ExceptionInvalidState);
}

void TestApplyActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::LinkConst);

  EXPECT_NO_THROW(userContext.SetLinkContent(linkAddr, "test"));
}

void TestApplyActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::LinkConst);

  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test"), utils::ExceptionInvalidState);
}

void TestChangeActionsSuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::LinkConst);

  EXPECT_NO_THROW(userContext.HelperSetSystemIdtf("test", linkAddr));
  EXPECT_NO_THROW(userContext.HelperResolveSystemIdtf("test", ScType::NodeConst));
}

void TestChangeActionsUnsuccessfully(std::unique_ptr<ScAgentContext> const & context, TestScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->GenerateNode(ScType::LinkConst);

  EXPECT_THROW(userContext.HelperSetSystemIdtf("test", linkAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test", ScType::NodeConst), utils::ExceptionInvalidState);
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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

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
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelIdentifiedUserAddr{nrel_identified_user_addr};
  ScAddr const & arcAddr = context->GenerateConnector(ScType::EdgeDCommonConst, guestUserAddr, userAddr);
  context->GenerateConnector(arcType, nrelIdentifiedUserAddr, arcAddr);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByIdentifiedUser)
{
  TestScMemoryContext userContext;

  ScAddr const & guestUserAddr = userContext.GetUser();
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestSetIdentifiedUser(m_ctx, guestUserAddr, userAddr);

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  TestActionsUnsuccessfully(m_ctx, userContext);
  TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserCreatedBefore)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserCreatedBeforeAndHavingClassWithPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
    HandleElementsByAuthenticatedUserCreatedBeforeAndBoughtClassWithPermissionsAndSoldThisClassAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestActionsUnsuccessfully(m_ctx, userContext);
            isAuthenticated = true;
          });

  ScAddr const & usersSetAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestAddAllPermissionsForUsersSetToInitActions(m_ctx, usersSetAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);
  TestAddClassForUser(m_ctx, userAddr, usersSetAddr, ScType::EdgeAccessConstNegTemp);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());

  ScAddr edgeAddr;
  std::atomic_bool isChecked = false;
  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::EdgeAccess>>(
            usersSetAddr,
            [this, &userContext, &isChecked](ScEventBeforeEraseOutgoingArc<ScType::EdgeAccess> const &)
            {
              TestActionsSuccessfully(m_ctx, userContext);
              isChecked = true;
            });

    edgeAddr = TestAddClassForUser(m_ctx, userAddr, usersSetAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
    EXPECT_TRUE(isChecked.load());
  }

  isChecked = false;
  eventSubscription = m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
      usersSetAddr,
      [this, &userContext, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
      {
        EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstNegTemp);

        TestActionsUnsuccessfully(m_ctx, userContext);
        isChecked = true;
      });

  m_ctx->EraseElement(edgeAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByInvalidConnectorToUser)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
          {
            isChecked = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);
  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::EdgeDCommonConst);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  eventSubscription = m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
      conceptAuthenticatedUserAddr,
      [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
      {
        isChecked = false;
      });

  ScIterator3Ptr it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeDCommonConst, userAddr);
  while (it3->Next())
  {
    m_ctx->EraseElement(it3->Get(2));
  }

  SC_LOCK_WAIT_WHILE_TRUE(isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByInvalidConnectorToUsersSet)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
          {
            isChecked = true;
          });

  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);
  TestAddAllPermissionsForUsersSetToInitActions(m_ctx, usersSetAddr, ScType::EdgeAccessConstNegTemp);
  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::EdgeDCommonConst);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  eventSubscription = m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
      conceptAuthenticatedUserAddr,
      [&isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
      {
        isChecked = false;
      });

  ScIterator3Ptr it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeDCommonConst, userAddr);
  while (it3->Next())
  {
    m_ctx->EraseElement(it3->Get(2));
  }

  SC_LOCK_WAIT_WHILE_TRUE(isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByUserWithInvalidConnectorsToPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &userContext, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
          {
            TestActionsUnsuccessfully(m_ctx, userContext);
            TestIteratorsUnsuccessfully(m_ctx, userContext);
            isChecked = true;
          });

  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserCreatedAfter)
{
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestScMemoryContext userContext{event.GetUser()};
            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isChecked = true;
          });

  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, SeveralHandleElementsByAuthenticatedUserCreatedAfter)
{
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isChecked = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [this, &isChecked](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestScMemoryContext userContext{event.GetUser()};
            TestActionsSuccessfully(m_ctx, userContext);
            TestIteratorsSuccessfully(m_ctx, userContext);

            isChecked = true;
          });

  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
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
    HandleElementsByAuthenticatedUserCreatedBeforeAndUnauthenticatedAfterAndAuthenticatedAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isAuthenticated = false;
  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstNegTemp);

              TestActionsUnsuccessfully(m_ctx, userContext);
              TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(m_ctx, userContext);

              isAuthenticated = false;
            });

    ScIterator3Ptr const it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeAccessConstPosTemp, userAddr);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    SC_LOCK_WAIT_WHILE_TRUE(isAuthenticated.load());
    EXPECT_FALSE(isAuthenticated.load());
  }

  EXPECT_TRUE(m_ctx->HelperCheckEdge(conceptAuthenticatedUserAddr, userAddr, ScType::EdgeAccessConstNegTemp));

  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

              TestActionsSuccessfully(m_ctx, userContext);
              TestIteratorsSuccessfully(m_ctx, userContext);

              isAuthenticated = true;
            });

    TestAddAllPermissionsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  EXPECT_FALSE(m_ctx->HelperCheckEdge(conceptAuthenticatedUserAddr, userAddr, ScType::EdgeAccessConstNegTemp));
  EXPECT_TRUE(m_ctx->HelperCheckEdge(conceptAuthenticatedUserAddr, userAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestActionsUnsuccessfully(m_ctx, userContext);

            isAuthenticated = true;
          });
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutWriteAndErasePermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  {
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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

  ScAddr const & nrelUserActionClassAddr{nrel_user_action_class_addr};
  std::atomic_bool isPermissionsUpdated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          nrelUserActionClassAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstNegTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  {
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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

  ScAddr const & nrelUsersSetActionClassAddr{nrel_users_set_action_class_addr};
  std::atomic_bool isPermissionsUpdated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          nrelUsersSetActionClassAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstNegTemp);

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

  ScIterator3Ptr it3 = userContext.Iterator3(elementAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_FALSE(it3->Next());

  ScAddr const & nodeAddr = userContext.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());

  EXPECT_THROW(
      userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, elementAddr, nodeAddr),
      utils::ExceptionInvalidState);
  EXPECT_TRUE(userContext.GenerateConnector(ScType::EdgeDCommonConst, elementAddr, nodeAddr).IsValid());

  it3 = userContext.Iterator3(elementAddr, ScType::EdgeDCommonConst, nodeAddr);
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

  ScAddr const & nodeAddr = userContext.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());

  EXPECT_TRUE(userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, elementAddr, nodeAddr).IsValid());

  ScIterator3Ptr it3 = userContext.Iterator3(elementAddr, ScType::EdgeAccessConstPosTemp, nodeAddr);
  EXPECT_TRUE(it3->Next());

  EXPECT_TRUE(userContext.GenerateConnector(ScType::EdgeDCommonConst, elementAddr, nodeAddr).IsValid());

  it3 = userContext.Iterator3(elementAddr, ScType::EdgeDCommonConst, nodeAddr);
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
  ScAddr const & readActionInScMemoryAddr{action_read_permissions_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, readActionInScMemoryAddr);
}

void TestAddPermissionsToHandleWritePermissions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_permissions_in_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, writeActionInScMemoryAddr);
}

void TestAddPermissionsToHandleErasePermissions(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_permissions_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActions(context, userAddr, eraseActionInScMemoryAddr);
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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  {
    ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestScMemoryContext userContext{userAddr};

  {
    ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
    std::atomic_bool isAuthenticated = false;
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [this, &userContext, &isAuthenticated](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
            {
              EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelUserActionClassWithinScStructureAddr{nrel_user_action_class_within_sc_structure_addr};
  ScAddr const & edgeBetweenActionAndStructureAddr =
      context->GenerateConnector(ScType::EdgeDCommonConst, actionClassAddr, structureAddr);
  ScAddr const & edgeAddr =
      context->GenerateConnector(ScType::EdgeDCommonConst, userAddr, edgeBetweenActionAndStructureAddr);
  context->GenerateConnector(arcType, nrelUserActionClassWithinScStructureAddr, edgeAddr);
}

void TestAddPermissionsForUserToInitReadActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActionsWithinStructure(
      context, userAddr, readActionInScMemoryAddr, structureAddr, arcType);
}

void TestAddPermissionsForUserToInitWriteActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_in_sc_memory_addr};
  TestAddPermissionsForUserToInitActionsWithinStructure(
      context, userAddr, writeActionInScMemoryAddr, structureAddr, arcType);
}

void TestAddPermissionsForUserToInitEraseActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_from_sc_memory_addr};
  TestAddPermissionsForUserToInitActionsWithinStructure(
      context, userAddr, eraseActionInScMemoryAddr, structureAddr, arcType);
}

void TestAddPermissionsForUsersSetToInitActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScAddr const & actionClassAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelUsersSetActionClassWithinScStructureAddr{nrel_users_set_action_class_within_sc_structure_addr};
  ScAddr const & edgeBetweenActionAndStructureAddr =
      context->GenerateConnector(ScType::EdgeDCommonConst, actionClassAddr, structureAddr);
  ScAddr const & edgeAddr =
      context->GenerateConnector(ScType::EdgeDCommonConst, usersSetAddr, edgeBetweenActionAndStructureAddr);
  context->GenerateConnector(arcType, nrelUsersSetActionClassWithinScStructureAddr, edgeAddr);
}

void TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & usersSetAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddPermissionsForUsersSetToInitActionsWithinStructure(
      context, usersSetAddr, readActionInScMemoryAddr, structureAddr, arcType);
}

void TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr,
    ScAddr const & edgeAddr,
    ScAddr const & linkAddr,
    ScAddr const & relationEdgeAddr,
    ScAddr const & relationAddr)
{
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::NodeConst);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  EXPECT_EQ(it3->Get(1), edgeAddr);
  EXPECT_EQ(it3->Get(2), linkAddr);

  ScAddr sourceAddr, targetAddr;
  EXPECT_TRUE(userContext.GetEdgeInfo(edgeAddr, sourceAddr, targetAddr));
  EXPECT_EQ(nodeAddr, sourceAddr);
  EXPECT_EQ(linkAddr, targetAddr);
  EXPECT_EQ(userContext.GetEdgeSource(edgeAddr), nodeAddr);
  EXPECT_EQ(userContext.GetEdgeTarget(edgeAddr), linkAddr);
  std::string content;
  EXPECT_FALSE(userContext.GetLinkContent(linkAddr, content));
  EXPECT_TRUE(content.empty());

  EXPECT_FALSE(it3->Next());

  it3 = userContext.Iterator3(nodeAddr, edgeAddr, linkAddr);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  EXPECT_EQ(it3->Get(1), edgeAddr);
  EXPECT_EQ(it3->Get(2), linkAddr);

  EXPECT_FALSE(it3->Next());

  auto const & TestIterator5Successfully = [&](ScIterator5Ptr const & it5)
  {
    EXPECT_TRUE(it5->Next());
    EXPECT_EQ(it5->Get(0), nodeAddr);
    EXPECT_EQ(it5->Get(1), edgeAddr);
    EXPECT_EQ(it5->Get(2), linkAddr);
    EXPECT_EQ(it5->Get(3), relationEdgeAddr);
    EXPECT_EQ(it5->Get(4), relationAddr);

    EXPECT_FALSE(it5->Next());
  };

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr,
      ScType::EdgeAccessConstPosTemp,
      ScType::LinkConst,
      ScType::EdgeAccessConstPosTemp,
      ScType::NodeConstRole);
  TestIterator5Successfully(it5);

  it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, linkAddr, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  TestIterator5Successfully(it5);

  it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, linkAddr, ScType::EdgeAccessConstPosTemp, relationAddr);
  TestIterator5Successfully(it5);

  it5 = userContext.Iterator5(
      ScType::NodeConst, ScType::EdgeAccessConstPosTemp, linkAddr, ScType::EdgeAccessConstPosTemp, relationAddr);
  TestIterator5Successfully(it5);

  it5 = userContext.Iterator5(
      ScType::NodeConst,
      ScType::EdgeAccessConstPosTemp,
      ScType::LinkConst,
      ScType::EdgeAccessConstPosTemp,
      relationAddr);
  TestIterator5Successfully(it5);

  it5 = userContext.Iterator5(
      ScType::NodeConst,
      ScType::EdgeAccessConstPosTemp,
      linkAddr,
      ScType::EdgeAccessConstPosTemp,
      ScType::NodeConstRole);
  TestIterator5Successfully(it5);
}

void TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.GetElementType(nodeAddr), utils::ExceptionInvalidState);

  ScIterator3Ptr const it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_FALSE(it3->Next());
}

void TestWriteActionsWithinStructureSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & structureAddr,
    ScAddr const & nodeAddr)
{
  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst));

  ScAddr const & linkAddr1 = userContext.GenerateLink(ScType::LinkConst);
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, structureAddr, linkAddr1));
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr1));

  ScAddr const & linkAddr2 = userContext.GenerateLink(ScType::LinkConst);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr2), utils::ExceptionInvalidState);
}

void TestWriteActionsWithinStructureUnsuccessfully(TestScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst), utils::ExceptionInvalidState);

  ScAddr const & linkAddr = userContext.GenerateLink(ScType::LinkConst);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, linkAddr, nodeAddr), utils::ExceptionInvalidState);
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
  ScAddr const & linkAddr1 = userContext.GenerateLink(ScType::LinkConst);
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, structureAddr, linkAddr1));
  EXPECT_TRUE(userContext.SetLinkContent(linkAddr1, "test_content"));
  EXPECT_NO_THROW(userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr1));

  ScAddr const & linkAddr2 = userContext.GenerateLink(ScType::LinkConst);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr2), utils::ExceptionInvalidState);
}

void TestChangeActionsWithinStructureUnsuccessfully(TestScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  ScAddr const & linkAddr = userContext.GenerateLink(ScType::LinkConst);
  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test_content"), utils::ExceptionInvalidState);
  EXPECT_THROW(
      userContext.GenerateConnector(ScType::EdgeAccessConstPosTemp, linkAddr, nodeAddr), utils::ExceptionInvalidState);
}

ScAddr TestGenerateStructureWithConnectorAndIncidentElements(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr & nodeAddr1,
    ScAddr & edgeAddr,
    ScAddr & linkAddr,
    ScAddr & relationEdgeAddr,
    ScAddr & relationAddr,
    ScAddr & nodeAddr2)
{
  nodeAddr1 = context->GenerateNode(ScType::NodeConst);
  linkAddr = context->GenerateLink(ScType::LinkConst);
  edgeAddr = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  relationAddr = context->GenerateNode(ScType::NodeConstRole);
  relationEdgeAddr = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, relationAddr, edgeAddr);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, relationAddr, edgeAddr);

  nodeAddr2 = context->GenerateNode(ScType::NodeConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::NodeConstStruct);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << nodeAddr1 << edgeAddr << linkAddr << relationEdgeAddr << relationAddr;

  return structureAddr;
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadPermissions)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
          {
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                userContext, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr);
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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
          {
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                userContext, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr);
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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
          {
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
            TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
            TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
            isAuthenticated = true;
          });
  ScAddr const & usersSetAddr = m_ctx->GenerateNode(ScType::NodeConst);
  TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(m_ctx, usersSetAddr, structureAddr);
  TestAddClassForUser(m_ctx, userAddr, usersSetAddr, ScType::EdgeAccessConstNegTemp);

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());

  ScAddr usersSetEdgeAddr;
  std::atomic_bool isChecked = false;
  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::EdgeAccess>>(
            usersSetAddr,
            [&](ScEventBeforeEraseOutgoingArc<ScType::EdgeAccess> const &)
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
  eventSubscription = m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
      usersSetAddr,
      [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
      {
        EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstNegTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isAuthenticated = false;
  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                  userContext, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr);
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

  ScIterator3Ptr it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeAccessConstPosTemp, userAddr);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  isAuthenticated = false;

  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

              isAuthenticated = true;
            });

    ScAddr const & nrelUserActionClassWithinScStructureAddr{nrel_user_action_class_within_sc_structure_addr};
    it3 = m_ctx->Iterator3(
        nrelUserActionClassWithinScStructureAddr, ScType::EdgeAccessConstPosTemp, ScType::EdgeDCommonConst);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    TestAddPermissionsForUserToInitReadActionsWithinStructure(
        m_ctx, userAddr, structureAddr, ScType::EdgeAccessConstNegTemp);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserHavingClassWithLocalReadPermissionsAndWithoutAfter)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const & usersSetAddr = TestGenerateClassForUser(m_ctx, userAddr);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isAuthenticated = false;
  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
                  userContext, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr);
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

  ScIterator3Ptr it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeAccessConstPosTemp, userAddr);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  isAuthenticated = false;

  {
    auto eventSubscription =
        m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
            conceptAuthenticatedUserAddr,
            [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const &)
            {
              TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
              TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
              TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

              isAuthenticated = true;
            });

    ScAddr const & nrelUsersSetActionClassWithinScStructureAddr{nrel_users_set_action_class_within_sc_structure_addr};
    it3 = m_ctx->Iterator3(
        nrelUsersSetActionClassWithinScStructureAddr, ScType::EdgeAccessConstPosTemp, ScType::EdgeDCommonConst);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    TestAddPermissionsForUsersSetToInitReadActionsWithinStructure(
        m_ctx, usersSetAddr, structureAddr, ScType::EdgeAccessConstNegTemp);
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
  nodeAddr1 = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  ScAddr const & edgeAddr1 = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  nodeAddr2 = context->GenerateNode(ScType::NodeConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::NodeConstStruct);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << nodeAddr1 << edgeAddr1;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorAndSourceSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::NodeConst);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  ScAddr const & edgeAddr = it3->Get(1);
  EXPECT_THROW(it3->Get(2), utils::ExceptionInvalidState);

  ScAddr sourceAddr, targetAddr;
  EXPECT_THROW(userContext.GetEdgeInfo(edgeAddr, sourceAddr, targetAddr), utils::ExceptionInvalidState);
  EXPECT_EQ(userContext.GetEdgeSource(edgeAddr), nodeAddr);
  EXPECT_THROW(userContext.GetEdgeTarget(edgeAddr), utils::ExceptionInvalidState);

  EXPECT_FALSE(it3->Next());

  it3 = userContext.Iterator3(nodeAddr, edgeAddr, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), nodeAddr);
  EXPECT_EQ(it3->Get(1), edgeAddr);
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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr nodeAddr1, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndSource(m_ctx, nodeAddr1, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
  ScAddr const & nodeAddr1 = context->GenerateNode(ScType::NodeConst);
  linkAddr = context->GenerateLink(ScType::LinkConst);
  ScAddr const & edgeAddr1 = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  nodeAddr2 = context->GenerateNode(ScType::NodeConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::NodeConstStruct);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << edgeAddr1 << linkAddr;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorAndTargetSuccessfully(
    TestScMemoryContext & userContext,
    ScAddr const & linkAddr)
{
  EXPECT_EQ(userContext.GetElementType(linkAddr), ScType::LinkConst);

  ScIterator3Ptr it3 = userContext.Iterator3(ScType::Unknown, ScType::EdgeAccessConstPosTemp, linkAddr);
  EXPECT_TRUE(it3->Next());
  EXPECT_THROW(it3->Get(0), utils::ExceptionInvalidState);
  ScAddr const & edgeAddr = it3->Get(1);
  EXPECT_EQ(it3->Get(2), linkAddr);

  ScAddr sourceAddr, targetAddr;
  EXPECT_THROW(userContext.GetEdgeInfo(edgeAddr, sourceAddr, targetAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeSource(edgeAddr), utils::ExceptionInvalidState);
  EXPECT_EQ(userContext.GetEdgeTarget(edgeAddr), linkAddr);

  EXPECT_FALSE(it3->Next());

  it3 = userContext.Iterator3(ScType::Unknown, edgeAddr, linkAddr);
  EXPECT_TRUE(it3->Next());
  EXPECT_THROW(it3->Get(0), utils::ExceptionInvalidState);
  EXPECT_EQ(it3->Get(1), edgeAddr);
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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr linkAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnectorAndTarget(m_ctx, linkAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

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
    ScAddr & edgeAddr,
    ScAddr & nodeAddr2)
{
  ScAddr const & nodeAddr1 = context->GenerateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->GenerateLink(ScType::LinkConst);
  edgeAddr = context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  nodeAddr2 = context->GenerateNode(ScType::NodeConst);
  context->GenerateConnector(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->GenerateNode(ScType::NodeConstStruct);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << edgeAddr;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorSuccessfully(TestScMemoryContext & userContext, ScAddr const & edgeAddr)
{
  EXPECT_EQ(userContext.GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

  ScIterator3Ptr it3 = userContext.Iterator3(ScType::Unknown, edgeAddr, ScType::Unknown);
  EXPECT_TRUE(it3->Next());
  EXPECT_THROW(it3->Get(0), utils::ExceptionInvalidState);
  EXPECT_EQ(it3->Get(1), edgeAddr);
  EXPECT_THROW(it3->Get(2), utils::ExceptionInvalidState);

  ScAddr sourceAddr, targetAddr;
  EXPECT_THROW(userContext.GetEdgeInfo(edgeAddr, sourceAddr, targetAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeSource(edgeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeTarget(edgeAddr), utils::ExceptionInvalidState);

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
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr edgeAddr, nodeAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithConnector(m_ctx, edgeAddr, nodeAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestReadActionsWithinStructureWithConnectorSuccessfully(userContext, edgeAddr);
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
  linkAddr1 = context->GenerateLink(ScType::LinkConst);
  context->SetLinkContent(linkAddr1, "test_content");
  linkAddr2 = context->GenerateLink(ScType::LinkConst);
  context->SetLinkContent(linkAddr2, "test_content");

  ScAddr const & structureAddr = context->GenerateNode(ScType::NodeConstStruct);
  ScStructure structure = context->ConvertToStructure(structureAddr);
  structure << linkAddr1;

  return structureAddr;
}

void TestReadActionsWithinStructureWithLinksSuccessfully(TestScMemoryContext & userContext, ScAddr const & linkAddr)
{
  ScAddrVector vector = userContext.FindLinksByContent("test_content");
  EXPECT_EQ(vector.size(), 1u);
  EXPECT_EQ(vector[0], linkAddr);

  vector = userContext.FindLinksByContentSubstring("test");
  EXPECT_EQ(vector.size(), 1u);
  EXPECT_EQ(vector[0], linkAddr);
}

void TestReadActionsWithinStructureWithLinksUnsuccessfully(TestScMemoryContext & userContext)
{
  EXPECT_THROW(userContext.FindLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadPermissionsAndWithStructureWithLinks)
{
  ScAddr const & userAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScAddr linkAddr1, linkAddr2;
  ScAddr const & structureAddr = TestGenerateStructureWithLinks(m_ctx, linkAddr1, linkAddr2);

  TestScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription =
      m_ctx->GenerateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(
          conceptAuthenticatedUserAddr,
          [&](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess> const & event)
          {
            EXPECT_EQ(event.GetArcType(), ScType::EdgeAccessConstPosTemp);

            TestReadActionsWithinStructureWithLinksSuccessfully(userContext, linkAddr1);
            TestReadActionsWithinStructureWithLinksUnsuccessfully(userContext);

            isAuthenticated = true;
          });
  TestAddPermissionsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}
