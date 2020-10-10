#include <gtest/gtest.h>

#include "sc-memory/sc_common_templ.hpp"
#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

using ScCommonTemplTest = ScMemoryTest;

TEST_F(ScCommonTemplTest, ResolveRelationTuple)
{
  ScAddr const el = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const relAddr = m_ctx->CreateNode(ScType::NodeConstNoRole);

  ScAddr const tuple = sc::ResolveRelationTuple(*m_ctx, el, relAddr);
  {
    ScTemplate templ;
    templ.TripleWithRelation(
      ScType::NodeVarTuple >> "_tuple",
      ScType::EdgeDCommonVar,
      el,
      ScType::EdgeAccessVarPosPerm,
      relAddr);

    ScTemplateSearchResult res;
    EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, res));
    EXPECT_EQ(res.Size(), 1u);
    EXPECT_EQ(tuple, res[0]["_tuple"]);
  }

  ScAddr const tuple2 = sc::ResolveRelationTuple(*m_ctx, el, relAddr);
  EXPECT_EQ(tuple, tuple2);
}

TEST_F(ScCommonTemplTest, SetRelationValue)
{
  ScAddr const el = m_ctx->CreateNode(ScType::NodeConst);

  std::string const value1 = "test_value";
  ScAddr const relAddr1 = m_ctx->CreateNode(ScType::NodeConstNoRole);
  ScAddr const linkAddr1 = sc::SetRelationValue(*m_ctx, el, relAddr1, value1);

  ScTemplate templ;
  templ.TripleWithRelation(
    el,
    ScType::EdgeDCommonVar,
    ScType::Link >> "_link",
    ScType::EdgeAccessVarPosPerm,
    relAddr1);

  ScLink link(*m_ctx, linkAddr1);

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, res));
  EXPECT_EQ(res.Size(), 1u);
  EXPECT_EQ(linkAddr1, res[0]["_link"]);
  EXPECT_EQ(link.Get<std::string>(), value1);

  // change value
  uint32_t const value2 = 57;
  ScAddr const linkAddr2 = sc::SetRelationValue(*m_ctx, el, relAddr1, value2);
  EXPECT_EQ(linkAddr1, linkAddr2);
  EXPECT_EQ(link.Get<uint32_t>(), value2);
}
