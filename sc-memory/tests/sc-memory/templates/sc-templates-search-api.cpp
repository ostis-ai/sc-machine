#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "template_test_utils.hpp"

using ScTemplateSearchApiTest = ScTemplateTest;

TEST_F(ScTemplateSearchApiTest, SearchWithResultNotSafeGet)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge.IsValid());

  ScTemplate templ;
  templ.Triple(
      addr1 >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::Unknown >> "_addr2");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  ScTemplateSearchResultItem const & item = result[0];

  EXPECT_EQ(item[0], addr1);
  EXPECT_EQ(item["_addr1"], addr1);
  EXPECT_EQ(item[1], edge);
  EXPECT_EQ(item["_edge"], edge);
  EXPECT_EQ(item[2], addr2);
  EXPECT_EQ(item["_addr2"], addr2);

  EXPECT_TRUE(item.Has("_addr1"));
  EXPECT_TRUE(item.Has("_edge"));
  EXPECT_TRUE(item.Has("_addr2"));
  EXPECT_FALSE(item.Has("_other_edge"));

  EXPECT_THROW(result[-1], utils::ExceptionInvalidParams);
  EXPECT_THROW(result[1], utils::ExceptionInvalidParams);
  EXPECT_THROW(item["_other_edge"], utils::ExceptionInvalidParams);

  EXPECT_THROW(item[-1], utils::ExceptionInvalidParams);
  EXPECT_THROW(item[3], utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateSearchApiTest, SearchWithResultSafeGet)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge.IsValid());

  ScTemplate templ;
  templ.Triple(
      addr1 >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::Unknown >> "_addr2");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  ScTemplateSearchResultItem item;
  EXPECT_TRUE(result.Get(0, item));
  ScAddr foundAddr;

  EXPECT_TRUE(item.Get(0, foundAddr));
  EXPECT_EQ(foundAddr, addr1);
  EXPECT_TRUE(item.Get("_addr1", foundAddr));
  EXPECT_EQ(foundAddr, addr1);

  EXPECT_TRUE(item.Get(1, foundAddr));
  EXPECT_EQ(foundAddr, edge);
  EXPECT_TRUE(item.Get("_edge", foundAddr));
  EXPECT_EQ(foundAddr, edge);

  EXPECT_TRUE(item.Get(2, foundAddr));
  EXPECT_EQ(foundAddr, addr2);
  EXPECT_TRUE(item.Get("_addr2", foundAddr));
  EXPECT_EQ(foundAddr, addr2);

  EXPECT_FALSE(item.Get(3, foundAddr));
  EXPECT_FALSE(foundAddr.IsValid());
  EXPECT_FALSE(item.Get("_other_edge", foundAddr));
  EXPECT_FALSE(foundAddr.IsValid());

  EXPECT_FALSE(result.Get(1, item));
}

TEST_F(ScTemplateSearchApiTest, SearchWithCallback)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge.IsValid());

  ScTemplate templ;
  templ.Triple(
      addr1 >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::Unknown >> "_addr2");

  size_t count = 0;
  m_ctx->HelperSearchTemplate(templ, [&](ScTemplateSearchResultItem const & item) {
    ScAddr foundAddr;

    EXPECT_TRUE(item.Get(0, foundAddr));
    EXPECT_EQ(foundAddr, addr1);
    EXPECT_TRUE(item.Get("_addr1", foundAddr));
    EXPECT_EQ(foundAddr, addr1);

    EXPECT_TRUE(item.Get(1, foundAddr));
    EXPECT_EQ(foundAddr, edge);
    EXPECT_TRUE(item.Get("_edge", foundAddr));
    EXPECT_EQ(foundAddr, edge);

    EXPECT_TRUE(item.Get(2, foundAddr));
    EXPECT_EQ(foundAddr, addr2);
    EXPECT_TRUE(item.Get("_addr2", foundAddr));
    EXPECT_EQ(foundAddr, addr2);

    ++count;
  });

  EXPECT_EQ(count, 1u);
}

TEST_F(ScTemplateSearchApiTest, SearchWithCallbackAndCheck)
{
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge.IsValid());

  ScTemplate templ;
  templ.Triple(
      addr1 >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::Unknown >> "_addr2");

  size_t count = 0;
  m_ctx->HelperSearchTemplate(templ, [&](ScTemplateSearchResultItem const & item) {
    ++count;
  }, [&](ScAddr const & beginAddr, ScAddr const & edgeAddr, ScAddr const & endAddr) -> bool
  {
    return !m_ctx->HelperCheckEdge(addr1, endAddr, ScType::EdgeAccessConstPosPerm);
  });

  EXPECT_EQ(count, 0u);

  m_ctx->HelperSearchTemplate(templ, [&](ScTemplateSearchResultItem const & item) {
    ++count;
  }, [&](ScAddr const & beginAddr, ScAddr const & edgeAddr, ScAddr const & endAddr) -> bool
  {
    return m_ctx->HelperCheckEdge(addr1, endAddr, ScType::EdgeAccessConstPosPerm);
  });

  EXPECT_EQ(count, 1u);
}

TEST_F(ScTemplateSearchApiTest, SearchVarTriple)
{
  ScTemplate templ;
  templ.Triple(
      ScType::Unknown >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::Unknown >> "_addr2");

  EXPECT_THROW(m_ctx->HelperSearchTemplate(
      templ, [&](ScTemplateSearchResultItem const & item) {}), utils::ExceptionInvalidState);
}

TEST_F(ScTemplateSearchApiTest, SearchEmpty)
{
  ScTemplate templ;
  size_t count = 0;
  m_ctx->HelperSearchTemplate(
      templ, [&](ScTemplateSearchResultItem const & item) { ++count; });

  EXPECT_EQ(count, 0u);
}
