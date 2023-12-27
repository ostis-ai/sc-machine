#include <gtest/gtest.h>

#include <algorithm>
#include <thread>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

TEST_F(ScMemoryTestWithUserMode, CreateEraseElementsByUnauthorizedUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CreateLink(ScType::LinkConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, userAddr, userAddr), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.EraseElement(userAddr), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByUnauthorizedUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, userAddr, userAddr);

  ScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.GetElementInputArcsCount(userAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementOutputArcsCount(edgeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeSource(edgeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeTarget(edgeAddr), utils::ExceptionInvalidState);
  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_THROW(userContext.GetEdgeInfo(edgeAddr, nodeAddr1, nodeAddr2), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementType(userAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.SetElementSubtype(userAddr, ScType::NodeConst), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, HandleLinkContentByUnauthorizedUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = m_ctx->CreateLink();

  ScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetLinkContent(linkAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContent("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperSetSystemIdtf("test", linkAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperFindBySystemIdtf("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test"), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, IterateElementByUnathorizedUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScIterator3Ptr it3 = userContext.Iterator3(userAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_THROW(it3->Next(), utils::ExceptionInvalidState);

  ScIterator5Ptr it5 = userContext.Iterator5(
      userAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_THROW(it5->Next(), utils::ExceptionInvalidState);

  EXPECT_THROW(userContext.HelperCheckEdge(
                   userAddr, userAddr, ScType::EdgeAccessConstPosTemp), utils::ExceptionInvalidState);
}

TEST_F(ScMemoryTestWithUserMode, CreateElementsByAuthorizedUserCreatedBefore)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptActionSubjectAddr = m_ctx->HelperFindBySystemIdtf("concept_action_subject_addr");

  std::atomic_bool isAuthorized = false;
  ScEventAddOutputEdge event(*m_ctx, conceptActionSubjectAddr,
    [&userContext, &isAuthorized](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
  {
    ScAddr const & testNodeAddr = userContext.CreateNode(ScType::NodeConst);
    EXPECT_TRUE(userContext.IsElement(testNodeAddr));

    ScAddr const & testLinkAddr = userContext.CreateLink(ScType::LinkConst);
    EXPECT_TRUE(userContext.IsElement(testLinkAddr));

    ScAddr const & testArcAddr = userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, testNodeAddr, testLinkAddr);
    EXPECT_TRUE(userContext.IsElement(testArcAddr));

    isAuthorized = true;

    return true;
  });

  ScAddr const & conceptAuthorizedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authorized_user");
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthorizedUserAddr, userAddr);

  while (!isAuthorized.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(ScMemoryTestWithUserMode, CreateElementsByAuthorizedUserCreatedAfter)
{
  ScAddr const & conceptActionSubjectAddr = m_ctx->HelperFindBySystemIdtf("concept_action_subject_addr");

  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(*m_ctx, conceptActionSubjectAddr,
    [&isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
  {
    ScMemoryContext userContext{userAddr};
    ScAddr const & testNodeAddr = userContext.CreateNode(ScType::NodeConst);
    EXPECT_TRUE(userContext.IsElement(testNodeAddr));

    ScAddr const & testLinkAddr = userContext.CreateLink(ScType::LinkConst);
    EXPECT_TRUE(userContext.IsElement(testLinkAddr));

    ScAddr const & testArcAddr = userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, testNodeAddr, testLinkAddr);
    EXPECT_TRUE(userContext.IsElement(testArcAddr));

    isChecked = true;

    return true;
  });

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & conceptAuthorizedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authorized_user");
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthorizedUserAddr, userAddr);

  while (!isChecked.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(ScMemoryTestWithUserMode, CreateElementsByAuthorizedUserCreatedBeforeAndUnathorizedAfter)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptActionSubjectAddr = m_ctx->HelperFindBySystemIdtf("concept_action_subject_addr");

  std::atomic_bool isAuthorized = false;
  ScEventAddOutputEdge event(*m_ctx, conceptActionSubjectAddr,
    [&userContext, &isAuthorized](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
  {
    ScAddr const & testNodeAddr = userContext.CreateNode(ScType::NodeConst);
    EXPECT_TRUE(userContext.IsElement(testNodeAddr));

    ScAddr const & testLinkAddr = userContext.CreateLink(ScType::LinkConst);
    EXPECT_TRUE(userContext.IsElement(testLinkAddr));

    ScAddr const & testArcAddr = userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, testNodeAddr, testLinkAddr);
    EXPECT_TRUE(userContext.IsElement(testArcAddr));

    isAuthorized = true;

    return true;
  });

  ScAddr const & conceptAuthorizedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authorized_user");
  ScAddr const & authEdgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthorizedUserAddr, userAddr);

  while (!isAuthorized.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  ScEventRemoveOutputEdge event2(*m_ctx, conceptAuthorizedUserAddr,
    [&userContext, &isAuthorized](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
  {
    EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);
    EXPECT_THROW(userContext.CreateLink(ScType::LinkConst), utils::ExceptionInvalidState);
    EXPECT_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, addr, userAddr), utils::ExceptionInvalidState);

    isAuthorized = false;

    return true;
  });

  m_ctx->EraseElement(authEdgeAddr);

  while (isAuthorized.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
