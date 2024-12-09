/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <sc-memory/sc_link.hpp>

namespace
{

std::string GetIdtf(ScMemoryContext & ctx, ScAddr const & addr)
{
  ScAddr const nrelIdtf = ctx.ResolveElementSystemIdentifier("nrel_idtf", ScType::ConstNodeNonRole);
  EXPECT_TRUE(nrelIdtf.IsValid());

  ScTemplate templ;
  templ.Quintuple(addr, ScType::VarCommonArc, ScType::NodeLink >> "_link", ScType::VarPermPosArc, nrelIdtf);

  ScTemplateSearchResult result;
  EXPECT_TRUE(ctx.SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  ScAddr const linkAddr = result[0]["_link"];
  EXPECT_TRUE(linkAddr.IsValid());

  ScLink link(ctx, linkAddr);
  EXPECT_TRUE(link.IsType<std::string>());

  return link.Get<std::string>();
}

}  // namespace

TEST_F(ScBuilderTest, SystemIdentifierVisibility)
{
  LoadKB(m_ctx, {"visibility_first.scs", "visibility_second.scs"});

  ScAddr const visFirst = m_ctx->ResolveElementSystemIdentifier("visibility_first");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->ResolveElementSystemIdentifier("visibility_second");
  EXPECT_TRUE(visSecond.IsValid());

  ScAddr const element = m_ctx->ResolveElementSystemIdentifier("visibility_sys_idtf");
  EXPECT_TRUE(element.IsValid());

  ScTemplate templ;
  templ.Triple(visFirst, ScType::VarPermPosArc, element);

  templ.Triple(visSecond, ScType::VarPermPosArc, element);

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
}

TEST_F(ScBuilderTest, GlobalVisibility)
{
  LoadKB(m_ctx, {"visibility_first.scs", "visibility_second.scs"});

  ScAddr const visFirst = m_ctx->ResolveElementSystemIdentifier("visibility_first_global");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->ResolveElementSystemIdentifier("visibility_second_global");
  EXPECT_TRUE(visSecond.IsValid());

  ScTemplate templ;
  templ.Triple(visFirst, ScType::VarTempPosArc, ScType::Node >> ".visibility_global");

  templ.Triple(visSecond, ScType::VarTempPosArc, ".visibility_global");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);

  ScAddr const element = result[0][".visibility_global"];
  EXPECT_TRUE(element.IsValid());

  EXPECT_EQ(GetIdtf(*m_ctx, element), ".visibility_global");
}

TEST_F(ScBuilderTest, LocalVisibility)
{
  LoadKB(m_ctx, {"visibility_first.scs", "visibility_second.scs"});

  ScAddr const visFirst = m_ctx->ResolveElementSystemIdentifier("visibility_first_local");
  EXPECT_TRUE(visFirst.IsValid());

  ScAddr const visSecond = m_ctx->ResolveElementSystemIdentifier("visibility_second_local");
  EXPECT_TRUE(visSecond.IsValid());

  ScAddr const visLocal = m_ctx->ResolveElementSystemIdentifier("visibility_local");
  EXPECT_TRUE(visLocal.IsValid());

  ScTemplate templ;
  templ.Triple(visLocal, ScType::VarPermPosArc, ScType::VarNode >> "_local");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
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
