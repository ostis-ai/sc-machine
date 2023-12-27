#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include <algorithm>

#include "sc_test.hpp"

TEST_F(ScMemoryTest, ScMemory)
{
  EXPECT_TRUE(ScMemory::IsInitialized());
  EXPECT_TRUE(m_ctx->IsValid());
}

TEST_F(ScMemoryTest, CreateElements)
{
  ScAddr const & nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(nodeAddr));

  ScAddr const & linkAddr = m_ctx->CreateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(linkAddr));

  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccess, nodeAddr, linkAddr);
  EXPECT_TRUE(edgeAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(edgeAddr));
}

TEST_F(ScMemoryTest, CreateElementsWithInvalidTypes)
{
  EXPECT_THROW(m_ctx->CreateNode(ScType::EdgeAccess), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateLink(ScType::NodeConst), utils::ExceptionInvalidParams);

  ScAddr const & nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(nodeAddr));

  ScAddr const & linkAddr = m_ctx->CreateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr.IsValid());
  EXPECT_TRUE(m_ctx->IsElement(linkAddr));

  EXPECT_THROW(m_ctx->CreateEdge(ScType::NodeConst, nodeAddr, linkAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryTest, SetGetFindSystemIdentifier)
{
  ScAddr const & addr = m_ctx->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_node", addr));
  EXPECT_EQ(m_ctx->HelperGetSystemIdtf(addr), "test_node");
  EXPECT_EQ(m_ctx->HelperFindBySystemIdtf("test_node"), addr);
}

TEST_F(ScMemoryTest, SetGetFindSystemIdentifierWithOutFiver)
{
  ScAddr const & addr = m_ctx->CreateNode(ScType::NodeConst);

  ScSystemIdentifierFiver setFiver;
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_node", addr, setFiver));
  EXPECT_EQ(m_ctx->HelperGetSystemIdtf(addr), "test_node");

  ScSystemIdentifierFiver foundFiver;
  EXPECT_TRUE(m_ctx->HelperFindBySystemIdtf("test_node", foundFiver));

  EXPECT_EQ(setFiver.addr1, foundFiver.addr1);
  EXPECT_EQ(setFiver.addr2, foundFiver.addr2);
  EXPECT_EQ(setFiver.addr3, foundFiver.addr3);
  EXPECT_EQ(setFiver.addr4, foundFiver.addr4);
  EXPECT_EQ(setFiver.addr5, foundFiver.addr5);
  EXPECT_TRUE(setFiver.addr1.IsValid());
  EXPECT_TRUE(setFiver.addr2.IsValid());
  EXPECT_TRUE(setFiver.addr3.IsValid());
  EXPECT_TRUE(setFiver.addr4.IsValid());
  EXPECT_TRUE(setFiver.addr5.IsValid());
}

TEST_F(ScMemoryTest, SetGetSystemIdentifierErrorSetTwice)
{
  ScAddr const & addr = m_ctx->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("test_node", addr));
  EXPECT_EQ(m_ctx->HelperGetSystemIdtf(addr), "test_node");
  EXPECT_EQ(m_ctx->HelperFindBySystemIdtf("test_node"), addr);

  ScAddr const & otherAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_FALSE(m_ctx->HelperSetSystemIdtf("test_node", otherAddr));
}

TEST_F(ScMemoryTest, ResolveGetSystemIdentifier)
{
  ScAddr const & addr = m_ctx->HelperResolveSystemIdtf("test_node", ScType::NodeConst);

  EXPECT_TRUE(addr.IsValid());
  EXPECT_EQ(m_ctx->HelperGetSystemIdtf(addr), "test_node");
  EXPECT_EQ(m_ctx->HelperFindBySystemIdtf("test_node"), addr);
}

TEST_F(ScMemoryTest, ResolveGetSystemIdentifierWithOutFiver)
{
  ScSystemIdentifierFiver resolveFiver;
  EXPECT_TRUE(m_ctx->HelperResolveSystemIdtf("test_node", ScType::NodeConst, resolveFiver));

  EXPECT_EQ(m_ctx->HelperGetSystemIdtf(resolveFiver.addr1), "test_node");
  EXPECT_EQ(m_ctx->HelperFindBySystemIdtf("test_node"), resolveFiver.addr1);

  ScSystemIdentifierFiver foundFiver;
  EXPECT_TRUE(m_ctx->HelperFindBySystemIdtf("test_node", foundFiver));

  ScAddr addr;
  EXPECT_FALSE(m_ctx->HelperFindBySystemIdtf("test_node1", addr));

  EXPECT_EQ(resolveFiver.addr1, foundFiver.addr1);
  EXPECT_EQ(resolveFiver.addr2, foundFiver.addr2);
  EXPECT_EQ(resolveFiver.addr3, foundFiver.addr3);
  EXPECT_EQ(resolveFiver.addr4, foundFiver.addr4);
  EXPECT_EQ(resolveFiver.addr5, foundFiver.addr5);
  EXPECT_TRUE(resolveFiver.addr1.IsValid());
  EXPECT_TRUE(resolveFiver.addr2.IsValid());
  EXPECT_TRUE(resolveFiver.addr3.IsValid());
  EXPECT_TRUE(resolveFiver.addr4.IsValid());
  EXPECT_TRUE(resolveFiver.addr5.IsValid());
}
