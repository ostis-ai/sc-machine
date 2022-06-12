#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"

#include "builder_test.hpp"

namespace
{

std::string GetIdtf(ScMemoryContext & ctx, ScAddr const& addr)
{
  ScAddr const nrelIdtf = ctx.HelperResolveSystemIdtf("nrel_idtf", ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelIdtf.IsValid());

  ScTemplate templ;
  templ.TripleWithRelation(
    addr,
    ScType::EdgeDCommonVar,
    ScType::Link >> "_link",
    ScType::EdgeAccessVarPosPerm,
    nrelIdtf);

  ScTemplateSearchResult result;
  EXPECT_TRUE(ctx.HelperSearchTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  ScAddr const linkAddr = result[0]["_link"];
  EXPECT_TRUE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);
  EXPECT_TRUE(link.IsType<std::string>());

  return link.Get<std::string>();
};

} // namespace

TEST_F(ScBuilderTest, visibility_sys_idtf)
{
  ScAddr const visFirst = m_ctx->HelperResolveSystemIdtf("visibility_first");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->HelperResolveSystemIdtf("visibility_second");
  EXPECT_TRUE(visSecond.IsValid());

  ScAddr const element = m_ctx->HelperResolveSystemIdtf("visibility_sys_idtf");
  EXPECT_TRUE(element.IsValid());

  ScTemplate templ;
  templ.Triple(
    visFirst,
    ScType::EdgeAccessVarPosPerm,
    element);

  templ.Triple(
    visSecond,
    ScType::EdgeAccessVarPosPerm,
    element);

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
}

TEST_F(ScBuilderTest, visibility_global)
{
  ScAddr const visFirst = m_ctx->HelperResolveSystemIdtf("visibility_first_global");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->HelperResolveSystemIdtf("visibility_second_global");
  EXPECT_TRUE(visSecond.IsValid());

  ScTemplate templ;
  templ.Triple(
    visFirst,
    ScType::EdgeAccessVarPosTemp,
    ScType::Node >> ".visibility_global");

  templ.Triple(
    visSecond,
    ScType::EdgeAccessVarPosTemp,
    ".visibility_global");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  ScAddr const element = result[0][".visibility_global"];
  EXPECT_TRUE(element.IsValid());

  EXPECT_EQ(GetIdtf(*m_ctx, element), ".visibility_global");
}

TEST_F(ScBuilderTest, visibility_local)
{
  ScAddr const visFirst = m_ctx->HelperResolveSystemIdtf("visibility_first_local");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->HelperResolveSystemIdtf("visibility_second_local");
  EXPECT_TRUE(visSecond.IsValid());

  ScAddr const visLocal = m_ctx->HelperResolveSystemIdtf("visibility_local");
  EXPECT_TRUE(visLocal.IsValid());

  ScTemplate templ;
  templ.Triple(
    visLocal,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "_local");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
  EXPECT_EQ(result.Size(), 2u);

  std::map<std::string, ScAddr> elements;
  for (size_t i = 0; i < result.Size(); ++i)
  {
    auto const & item = result[i];
    ScAddr const a = item["_local"];

    EXPECT_TRUE(a.IsValid());

    std::string const idtf = GetIdtf(*m_ctx, a);
    EXPECT_FALSE(idtf.empty());

    elements[idtf] = a;
  }

  EXPECT_EQ(elements.size(), 2u);

  ScAddr const localFirst = elements["..visibility_local_first"];
  EXPECT_TRUE(localFirst.IsValid());

  ScAddr const localSecond = elements["..visibility_local_second"];
  EXPECT_TRUE(localSecond.IsValid());

  EXPECT_NE(localFirst, localSecond);
}
