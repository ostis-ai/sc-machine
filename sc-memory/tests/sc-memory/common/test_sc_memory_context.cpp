#include <gtest/gtest.h>

#include <thread>

#include "sc-memory/sc_memory.hpp"
#include "sc-core/sc_keynodes.h"
#include "sc-memory/sc_struct.hpp"

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

void TestAddAccessLevelsForUserToInitActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelUserActionClassAddr{nrel_user_action_class_addr};
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeDCommonConst, userAddr, actionClassAddr);
  context->CreateEdge(arcType, nrelUserActionClassAddr, edgeAddr);
}

void TestAddAccessLevelsForUserToInitReadActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActions(context, userAddr, readActionInScMemoryAddr, arcType);
}

void TestAddAccessLevelsForUserToInitWriteActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_in_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActions(context, userAddr, writeActionInScMemoryAddr, arcType);
}

void TestAddAccessLevelsForUserToInitEraseActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_in_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActions(context, userAddr, eraseActionInScMemoryAddr, arcType);
}

void TestAddAllAccessLevelsForUserToInitActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  TestAddAccessLevelsForUserToInitReadActions(context, userAddr, arcType);
  TestAddAccessLevelsForUserToInitWriteActions(context, userAddr, arcType);
  TestAddAccessLevelsForUserToInitEraseActions(context, userAddr, arcType);
}

void TestRemoveAccessLevelsForUserToInitActions(
    std::unique_ptr<ScMemoryContext> const & context,
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

void TestRemoveAccessLevelsForUserToInitReadActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestRemoveAccessLevelsForUserToInitActions(context, userAddr, readActionInScMemoryAddr);
}

void TestAuthenticationRequestUser(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & conceptAuthenticationRequestUserAddr{concept_authentication_request_user_addr};
  context->CreateEdge(arcType, conceptAuthenticationRequestUserAddr, userAddr);
}

void TestReadActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

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

void TestReadActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

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
    std::unique_ptr<ScMemoryContext> const & context,
    ScMemoryContext & userContext)
{
  TestReadActionsUnsuccessfully(context, userContext);

  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CreateLink(ScType::LinkConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContent("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperFindBySystemIdtf("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test"), utils::ExceptionInvalidState);

  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_THROW(it3->Next(), utils::ExceptionInvalidState);

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_THROW(it5->Next(), utils::ExceptionInvalidState);

  EXPECT_THROW(
      userContext.HelperCheckEdge(nodeAddr, nodeAddr, ScType::EdgeAccessConstPosTemp), utils::ExceptionInvalidState);
}

void TestIteratorsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_TRUE(it3->Next());

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_TRUE(userContext.HelperCheckEdge(nodeAddr, linkAddr, ScType::EdgeAccessConstPosTemp));
}

void TestIteratorsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_FALSE(it3->Next());

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_FALSE(userContext.HelperCheckEdge(nodeAddr, linkAddr, ScType::EdgeAccessConstPosTemp));
}

void TestWriteActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);

  EXPECT_TRUE(userContext.CreateNode(ScType::NodeConst).IsValid());
  EXPECT_TRUE(userContext.CreateLink(ScType::LinkConst).IsValid());
  EXPECT_TRUE(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr).IsValid());

  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst));
  EXPECT_TRUE(userContext.Save());
}

void TestWriteActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);

  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr), utils::ExceptionInvalidState);

  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.Save(), utils::ExceptionInvalidState);
}

void TestEraseActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(userContext.EraseElement(nodeAddr));
}

void TestEraseActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_THROW(userContext.EraseElement(nodeAddr), utils::ExceptionInvalidState);
}

void TestApplyActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_NO_THROW(userContext.SetLinkContent(linkAddr, "test"));
}

void TestApplyActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test"), utils::ExceptionInvalidState);
}

void TestChangeActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_NO_THROW(userContext.HelperSetSystemIdtf("test", linkAddr));
  EXPECT_NO_THROW(userContext.HelperResolveSystemIdtf("test", ScType::NodeConst));
}

void TestChangeActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_THROW(userContext.HelperSetSystemIdtf("test", linkAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test", ScType::NodeConst), utils::ExceptionInvalidState);
}

void TestActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  TestReadActionsSuccessfully(context, userContext);
  TestWriteActionsSuccessfully(context, userContext);
  TestEraseActionsSuccessfully(context, userContext);
  TestApplyActionsSuccessfully(context, userContext);
  TestChangeActionsSuccessfully(context, userContext);
}

void TestActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  TestReadActionsUnsuccessfully(context, userContext);
  TestWriteActionsUnsuccessfully(context, userContext);
  TestEraseActionsUnsuccessfully(context, userContext);
  TestApplyActionsUnsuccessfully(context, userContext);
  TestChangeActionsUnsuccessfully(context, userContext);
}

TEST_F(ScMemoryTestWithUserMode, GetUserAddrFromContext)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  EXPECT_EQ(userContext.GetUserAddr(), userAddr);
}

TEST_F(ScMemoryTestWithUserMode, GetGuestUserAddrFromContext)
{
  ScMemoryContext userContext;
  EXPECT_TRUE(userContext.GetUserAddr().IsValid());
}

TEST_F(ScMemoryTestWithUserMode, SetUserAddrForContext)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext;
  userContext.SetUserAddr(userAddr);
  EXPECT_EQ(userContext.GetUserAddr(), userAddr);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByGuestUser)
{
  ScMemoryContext userContext;
  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userContext.GetUserAddr());
  TestActionsSuccessfully(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByUnauthenticatedUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  TestActionsUnsuccessfully(m_ctx, userContext);
  TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserCreatedBefore)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestActionsSuccessfully(m_ctx, userContext);
        TestIteratorsSuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });

  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserSetAfter)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext;

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestActionsSuccessfully(m_ctx, userContext);
        TestIteratorsSuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });

  userContext.SetUserAddr(userAddr);
  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByInvalidConnectorToUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        return isChecked = true;
      });

  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);
  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr, ScType::EdgeDCommonConst);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(!isChecked.load());

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  ScEventAddOutputEdge event2(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        return isChecked = false;
      });

  ScIterator3Ptr it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeDCommonConst, userAddr);
  while (it3->Next())
  {
    m_ctx->EraseElement(it3->Get(2));
  }

  SC_LOCK_WAIT_WHILE_TRUE(isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, NoHandleElementsByUserWithInvalidConnectorsToAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &userContext, &isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestActionsUnsuccessfully(m_ctx, userContext);
        TestIteratorsUnsuccessfully(m_ctx, userContext);
        return isChecked = true;
      });

  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr, ScType::EdgeAccessConstNegTemp);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserCreatedAfter)
{
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        ScMemoryContext userContext{userAddr};
        TestActionsSuccessfully(m_ctx, userContext);
        TestIteratorsSuccessfully(m_ctx, userContext);

        isChecked = true;

        return true;
      });

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, SeveralHandleElementsByAuthenticatedUserCreatedAfter)
{
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        ScMemoryContext userContext{userAddr};
        TestActionsSuccessfully(m_ctx, userContext);
        TestIteratorsSuccessfully(m_ctx, userContext);

        isChecked = true;

        return true;
      });

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  {
    ScMemoryContext userContext{userAddr};
    TestActionsSuccessfully(m_ctx, userContext);
    TestIteratorsSuccessfully(m_ctx, userContext);
  }
  {
    ScMemoryContext userContext{userAddr};
    TestActionsSuccessfully(m_ctx, userContext);
    TestIteratorsSuccessfully(m_ctx, userContext);
  }
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserCreatedBeforeAndUnauthenticatedAfterAndAuthenticatedAfter)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isAuthenticated = false;
  {
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
        {
          EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

          TestActionsSuccessfully(m_ctx, userContext);
          TestIteratorsSuccessfully(m_ctx, userContext);

          isAuthenticated = true;

          return true;
        });

    TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  {
    ScEventAddOutputEdge event2(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
        {
          EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstNegTemp);

          TestActionsUnsuccessfully(m_ctx, userContext);
          TestReadActionsUnsuccessfullyByNotAuthorizedUserOnly(m_ctx, userContext);

          isAuthenticated = false;

          return true;
        });

    ScIterator3Ptr const it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeAccessConstPosTemp, userAddr);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    SC_LOCK_WAIT_WHILE_TRUE(isAuthenticated.load());
    EXPECT_FALSE(isAuthenticated.load());
  }

  EXPECT_TRUE(m_ctx->HelperCheckEdge(conceptAuthenticatedUserAddr, userAddr, ScType::EdgeAccessConstNegTemp));

  {
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
        {
          EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

          TestActionsSuccessfully(m_ctx, userContext);
          TestIteratorsSuccessfully(m_ctx, userContext);

          isAuthenticated = true;

          return true;
        });

    TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  EXPECT_FALSE(m_ctx->HelperCheckEdge(conceptAuthenticatedUserAddr, userAddr, ScType::EdgeAccessConstNegTemp));
  EXPECT_TRUE(m_ctx->HelperCheckEdge(conceptAuthenticatedUserAddr, userAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestActionsUnsuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutWriteAndEraseAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsSuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutEraseAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsSuccessfully(m_ctx, userContext);
        TestWriteActionsSuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  TestAddAccessLevelsForUserToInitWriteActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutWriteAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsSuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsSuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  TestAddAccessLevelsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutReadAndEraseAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsSuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitWriteActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutReadAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsSuccessfully(m_ctx, userContext);
        TestEraseActionsSuccessfully(m_ctx, userContext);
        TestApplyActionsSuccessfully(m_ctx, userContext);
        TestChangeActionsSuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitWriteActions(m_ctx, userAddr);
  TestAddAccessLevelsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutReadAndWriteAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsSuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithReadAccessLevelsAndWithoutReadAccessLevelsAfter)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  {
    std::atomic_bool isAuthenticated = false;
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
        {
          EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

          TestReadActionsSuccessfully(m_ctx, userContext);
          TestWriteActionsUnsuccessfully(m_ctx, userContext);
          TestEraseActionsUnsuccessfully(m_ctx, userContext);
          TestApplyActionsUnsuccessfully(m_ctx, userContext);
          TestChangeActionsUnsuccessfully(m_ctx, userContext);

          isAuthenticated = true;

          return true;
        });
    TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  ScAddr const & nrelUserActionClassAddr{nrel_user_action_class_addr};
  std::atomic_bool isAccessLevelsUpdated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      nrelUserActionClassAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstNegTemp);

        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAccessLevelsUpdated = true;

        return true;
      });

  TestRemoveAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  SC_LOCK_WAIT_WHILE_TRUE(!isAccessLevelsUpdated.load());
  EXPECT_TRUE(isAccessLevelsUpdated.load());
}

void TestReadWriteEraseAccessedElementUnsuccessfully(
    std::unique_ptr<ScMemoryContext> const & context,
    ScMemoryContext & userContext,
    sc_addr const & addr)
{
  ScAddr const & elementAddr{addr};

  EXPECT_THROW(userContext.EraseElement(elementAddr), utils::ExceptionInvalidState);

  ScIterator3Ptr it3 = userContext.Iterator3(elementAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_FALSE(it3->Next());

  ScAddr const & nodeAddr = userContext.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());

  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, elementAddr, nodeAddr), utils::ExceptionInvalidState);
  EXPECT_TRUE(userContext.CreateEdge(ScType::EdgeDCommonConst, elementAddr, nodeAddr).IsValid());

  it3 = userContext.Iterator3(elementAddr, ScType::EdgeDCommonConst, nodeAddr);
  EXPECT_TRUE(it3->Next());
}

void TestReadWriteEraseAccessedAllElementsUnsuccessfully(
    std::unique_ptr<ScMemoryContext> const & context,
    ScMemoryContext & userContext)
{
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, concept_authentication_request_user_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, concept_authenticated_user_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, nrel_user_action_class_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_read_from_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_generate_in_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_erase_in_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_read_permissions_from_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_generate_permissions_in_sc_memory_addr);
  TestReadWriteEraseAccessedElementUnsuccessfully(context, userContext, action_erase_permissions_from_sc_memory_addr);
}

void TestReadWriteAccessedElementSuccessfully(
    std::unique_ptr<ScMemoryContext> const & context,
    ScMemoryContext & userContext,
    sc_addr const addr)
{
  ScAddr const & elementAddr{addr};

  ScAddr const & nodeAddr = userContext.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());

  EXPECT_TRUE(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, elementAddr, nodeAddr).IsValid());

  ScIterator3Ptr it3 = userContext.Iterator3(elementAddr, ScType::EdgeAccessConstPosTemp, nodeAddr);
  EXPECT_TRUE(it3->Next());

  EXPECT_TRUE(userContext.CreateEdge(ScType::EdgeDCommonConst, elementAddr, nodeAddr).IsValid());

  it3 = userContext.Iterator3(elementAddr, ScType::EdgeDCommonConst, nodeAddr);
  EXPECT_TRUE(it3->Next());
}

void TestReadWriteEraseAccessedAllElementsSuccessfully(
    std::unique_ptr<ScMemoryContext> const & context,
    ScMemoryContext & userContext)
{
  TestReadWriteAccessedElementSuccessfully(context, userContext, concept_authentication_request_user_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, nrel_user_action_class_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_read_from_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_generate_in_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_erase_in_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_read_permissions_from_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_generate_permissions_in_sc_memory_addr);
  TestReadWriteAccessedElementSuccessfully(context, userContext, action_erase_permissions_from_sc_memory_addr);
}

void TestAddAccessLevelsToHandleReadAccessLevels(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & readActionInScMemoryAddr{action_read_permissions_from_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActions(context, userAddr, readActionInScMemoryAddr);
}

void TestAddAccessLevelsToHandleWriteAccessLevels(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_permissions_in_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActions(context, userAddr, writeActionInScMemoryAddr);
}

void TestAddAccessLevelsToHandleEraseAccessLevels(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_permissions_from_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActions(context, userAddr, eraseActionInScMemoryAddr);
}

void TestAddAllAccessLevelsToHandleAllAccessLevels(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  TestAddAccessLevelsToHandleReadAccessLevels(context, userAddr);
  TestAddAccessLevelsToHandleWriteAccessLevels(context, userAddr);
  TestAddAccessLevelsToHandleEraseAccessLevels(context, userAddr);
}

TEST_F(ScMemoryTestWithUserMode, HandleAccessedElementsByAuthenticatedUserWithoutAccessLevelsForAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  {
    ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
    std::atomic_bool isAuthenticated = false;
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
        {
          EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

          TestActionsSuccessfully(m_ctx, userContext);

          isAuthenticated = true;

          return true;
        });

    TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  TestReadWriteEraseAccessedAllElementsUnsuccessfully(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleAccessedElementsByAuthenticatedUserWithAccessLevelsForAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  {
    ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
    std::atomic_bool isAuthenticated = false;
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
        {
          EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

          TestActionsSuccessfully(m_ctx, userContext);

          isAuthenticated = true;

          return true;
        });

    TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
    TestAddAllAccessLevelsToHandleAllAccessLevels(m_ctx, userAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  TestReadWriteEraseAccessedAllElementsSuccessfully(m_ctx, userContext);
}

void TestAddAccessLevelsForUserToInitActionsWithinStructure(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & nrelUserActionClassWithinScStructureAddr{nrel_user_action_class_within_sc_structure_addr};
  ScAddr const & edgeBetweenActionAndStructureAddr =
      context->CreateEdge(ScType::EdgeDCommonConst, actionClassAddr, structureAddr);
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeDCommonConst, userAddr, edgeBetweenActionAndStructureAddr);
  context->CreateEdge(arcType, nrelUserActionClassWithinScStructureAddr, edgeAddr);
}

void TestAddAccessLevelsForUserToInitReadActionsWithinStructure(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & readActionInScMemoryAddr{action_read_from_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActionsWithinStructure(
      context, userAddr, readActionInScMemoryAddr, structureAddr, arcType);
}

void TestAddAccessLevelsForUserToInitWriteActionsWithinStructure(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & writeActionInScMemoryAddr{action_generate_in_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActionsWithinStructure(
      context, userAddr, writeActionInScMemoryAddr, structureAddr, arcType);
}

void TestAddAccessLevelsForUserToInitEraseActionsWithinStructure(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & structureAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & eraseActionInScMemoryAddr{action_erase_in_sc_memory_addr};
  TestAddAccessLevelsForUserToInitActionsWithinStructure(
      context, userAddr, eraseActionInScMemoryAddr, structureAddr, arcType);
}

void TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
    ScMemoryContext & userContext,
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
    ScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.GetElementType(nodeAddr), utils::ExceptionInvalidState);

  ScIterator3Ptr const it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_FALSE(it3->Next());
}

void TestWriteActionsWithinStructureSuccessfully(
    ScMemoryContext & userContext,
    ScAddr const & structureAddr,
    ScAddr const & nodeAddr)
{
  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst));

  ScAddr const & linkAddr1 = userContext.CreateLink(ScType::LinkConst);
  EXPECT_NO_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, structureAddr, linkAddr1));
  EXPECT_NO_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr1));

  ScAddr const & linkAddr2 = userContext.CreateLink(ScType::LinkConst);
  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr2), utils::ExceptionInvalidState);
}

void TestWriteActionsWithinStructureUnsuccessfully(ScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst), utils::ExceptionInvalidState);

  ScAddr const & linkAddr = userContext.CreateLink(ScType::LinkConst);
  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, linkAddr, nodeAddr), utils::ExceptionInvalidState);
}

void TestEraseActionsWithinStructureSuccessfully(ScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_TRUE(userContext.EraseElement(nodeAddr));
}

void TestEraseActionsWithinStructureUnsuccessfully(ScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  EXPECT_THROW(userContext.EraseElement(nodeAddr), utils::ExceptionInvalidState);
}

void TestChangeActionsWithinStructureSuccessfully(
    ScMemoryContext & userContext,
    ScAddr const & structureAddr,
    ScAddr const & nodeAddr)
{
  ScAddr const & linkAddr1 = userContext.CreateLink(ScType::LinkConst);
  EXPECT_NO_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, structureAddr, linkAddr1));
  EXPECT_TRUE(userContext.SetLinkContent(linkAddr1, "test_content"));
  EXPECT_NO_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr1));

  ScAddr const & linkAddr2 = userContext.CreateLink(ScType::LinkConst);
  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr2), utils::ExceptionInvalidState);
}

void TestChangeActionsWithinStructureUnsuccessfully(ScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  ScAddr const & linkAddr = userContext.CreateLink(ScType::LinkConst);
  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test_content"), utils::ExceptionInvalidState);
  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, linkAddr, nodeAddr), utils::ExceptionInvalidState);
}

ScAddr TestCreateStructureWithConnectorAndIncidentElements(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr & nodeAddr1,
    ScAddr & edgeAddr,
    ScAddr & linkAddr,
    ScAddr & relationEdgeAddr,
    ScAddr & relationAddr,
    ScAddr & nodeAddr2)
{
  nodeAddr1 = context->CreateNode(ScType::NodeConst);
  linkAddr = context->CreateLink(ScType::LinkConst);
  edgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  relationAddr = context->CreateNode(ScType::NodeConstRole);
  relationEdgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, relationAddr, edgeAddr);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, relationAddr, edgeAddr);

  nodeAddr2 = context->CreateNode(ScType::NodeConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->CreateNode(ScType::NodeConstStruct);
  ScStruct structure{*context, structureAddr};
  structure << nodeAddr1 << edgeAddr << linkAddr << relationEdgeAddr << relationAddr;

  return structureAddr;
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const &, ScAddr const & userAddr)
      {
        TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
            userContext, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr);
        TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalWriteAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
        TestWriteActionsWithinStructureSuccessfully(userContext, structureAddr, nodeAddr1);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitWriteActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalEraseAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
        TestEraseActionsWithinStructureSuccessfully(userContext, nodeAddr1);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitEraseActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalWriteAndEraseAccessLevels)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr1);
        TestWriteActionsWithinStructureSuccessfully(userContext, structureAddr, nodeAddr1);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestChangeActionsWithinStructureSuccessfully(userContext, structureAddr, nodeAddr1);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestEraseActionsWithinStructureSuccessfully(userContext, nodeAddr1);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitWriteActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAddAccessLevelsForUserToInitEraseActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadAccessLevelsAndWithoutAfter)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndIncidentElements(
      m_ctx, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};

  std::atomic_bool isAuthenticated = false;
  {
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [&](ScAddr const & addr, ScAddr const &, ScAddr const & userAddr)
        {
          TestReadActionsWithinStructureWithConnectorAndIncidentElementsSuccessfully(
              userContext, nodeAddr1, edgeAddr, linkAddr, relationEdgeAddr, relationAddr);
          TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
          TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
          TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
          TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

          return isAuthenticated = true;
        });
    TestAddAccessLevelsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }

  ScIterator3Ptr it3 = m_ctx->Iterator3(conceptAuthenticatedUserAddr, ScType::EdgeAccessConstPosTemp, userAddr);
  EXPECT_TRUE(it3->Next());
  m_ctx->EraseElement(it3->Get(1));
  isAuthenticated = false;

  {
    ScEventAddOutputEdge event(
        *m_ctx,
        conceptAuthenticatedUserAddr,
        [&](ScAddr const & addr, ScAddr const &, ScAddr const & userAddr)
        {
          TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr2);
          TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
          TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);
          TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr1);

          return isAuthenticated = true;
        });

    ScAddr const & nrelUserActionClassWithinScStructureAddr{nrel_user_action_class_within_sc_structure_addr};
    it3 = m_ctx->Iterator3(
        nrelUserActionClassWithinScStructureAddr, ScType::EdgeAccessConstPosTemp, ScType::EdgeDCommonConst);
    EXPECT_TRUE(it3->Next());
    m_ctx->EraseElement(it3->Get(1));

    TestAddAccessLevelsForUserToInitReadActionsWithinStructure(
        m_ctx, userAddr, structureAddr, ScType::EdgeAccessConstNegTemp);
    TestAuthenticationRequestUser(m_ctx, userAddr);

    SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
    EXPECT_TRUE(isAuthenticated.load());
  }
}

ScAddr TestCreateStructureWithConnectorAndSource(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr & nodeAddr1,
    ScAddr & nodeAddr2)
{
  nodeAddr1 = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  ScAddr const & edgeAddr1 = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  nodeAddr2 = context->CreateNode(ScType::NodeConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->CreateNode(ScType::NodeConstStruct);
  ScStruct structure{*context, structureAddr};
  structure << nodeAddr1 << edgeAddr1;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorAndSourceSuccessfully(
    ScMemoryContext & userContext,
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
    ScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr);
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserWithLocalReadAccessLevelsAndWithStructureWithConnectorAndSourceElement)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr nodeAddr1, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndSource(m_ctx, nodeAddr1, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithConnectorAndSourceSuccessfully(userContext, nodeAddr1);
        TestReadActionsWithinStructureWithConnectorAndSourceUnsuccessfully(userContext, nodeAddr2);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

ScAddr TestCreateStructureWithConnectorAndTarget(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr & linkAddr,
    ScAddr & nodeAddr2)
{
  ScAddr const & nodeAddr1 = context->CreateNode(ScType::NodeConst);
  linkAddr = context->CreateLink(ScType::LinkConst);
  ScAddr const & edgeAddr1 = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  nodeAddr2 = context->CreateNode(ScType::NodeConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->CreateNode(ScType::NodeConstStruct);
  ScStruct structure{*context, structureAddr};
  structure << edgeAddr1 << linkAddr;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorAndTargetSuccessfully(
    ScMemoryContext & userContext,
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
    ScMemoryContext & userContext,
    ScAddr const & nodeAddr)
{
  TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr);
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserWithLocalReadAccessLevelsAndWithStructureWithConnectorAndTargetElement)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr linkAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnectorAndTarget(m_ctx, linkAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithConnectorAndTargetSuccessfully(userContext, linkAddr);
        TestReadActionsWithinStructureWithConnectorAndTargetUnsuccessfully(userContext, nodeAddr2);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

ScAddr TestCreateStructureWithConnector(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr & edgeAddr,
    ScAddr & nodeAddr2)
{
  ScAddr const & nodeAddr1 = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  edgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, linkAddr);
  nodeAddr2 = context->CreateNode(ScType::NodeConst);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr1, nodeAddr2);

  ScAddr const & structureAddr = context->CreateNode(ScType::NodeConstStruct);
  ScStruct structure{*context, structureAddr};
  structure << edgeAddr;

  return structureAddr;
}

void TestReadActionsWithinStructureWithConnectorSuccessfully(ScMemoryContext & userContext, ScAddr const & edgeAddr)
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

void TestReadActionsWithinStructureWithConnectorUnsuccessfully(ScMemoryContext & userContext, ScAddr const & nodeAddr)
{
  TestReadActionsWithinStructureWithConnectorAndIncidentElementsUnsuccessfully(userContext, nodeAddr);
}

TEST_F(
    ScMemoryTestWithUserMode,
    HandleElementsByAuthenticatedUserWithLocalReadAccessLevelsAndWithStructureWithConnector)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr edgeAddr, nodeAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithConnector(m_ctx, edgeAddr, nodeAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const &, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithConnectorSuccessfully(userContext, edgeAddr);
        TestReadActionsWithinStructureWithConnectorUnsuccessfully(userContext, nodeAddr2);
        TestWriteActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestChangeActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);
        TestEraseActionsWithinStructureUnsuccessfully(userContext, nodeAddr2);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

ScAddr TestCreateStructureWithLinks(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr & linkAddr1,
    ScAddr & linkAddr2)
{
  linkAddr1 = context->CreateLink(ScType::LinkConst);
  context->SetLinkContent(linkAddr1, "test_content");
  linkAddr2 = context->CreateLink(ScType::LinkConst);
  context->SetLinkContent(linkAddr2, "test_content");

  ScAddr const & structureAddr = context->CreateNode(ScType::NodeConstStruct);
  ScStruct structure{*context, structureAddr};
  structure << linkAddr1;

  return structureAddr;
}

void TestReadActionsWithinStructureWithLinksSuccessfully(ScMemoryContext & userContext, ScAddr const & linkAddr)
{
  ScAddrVector vector = userContext.FindLinksByContent("test_content");
  EXPECT_EQ(vector.size(), 1u);
  EXPECT_EQ(vector[0], linkAddr);

  vector = userContext.FindLinksByContentSubstring("test");
  EXPECT_EQ(vector.size(), 1u);
  EXPECT_EQ(vector[0], linkAddr);
}

void TestReadActionsWithinStructureWithLinksUnsuccessfully(ScMemoryContext & userContext)
{
  EXPECT_THROW(userContext.FindLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithLocalReadAccessLevelsAndWithStructureWithLinks)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr linkAddr1, linkAddr2;
  ScAddr const & structureAddr = TestCreateStructureWithLinks(m_ctx, linkAddr1, linkAddr2);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        EXPECT_EQ(m_ctx->GetElementType(edgeAddr), ScType::EdgeAccessConstPosTemp);

        TestReadActionsWithinStructureWithLinksSuccessfully(userContext, linkAddr1);
        TestReadActionsWithinStructureWithLinksUnsuccessfully(userContext);

        return isAuthenticated = true;
      });
  TestAddAccessLevelsForUserToInitReadActionsWithinStructure(m_ctx, userAddr, structureAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}
