#include <gtest/gtest.h>

#include "builder_test.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "tests/sc-memory/_test/dummy_file_interface.hpp"

TEST_F(ScBuilderTest, contours)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  std::string const scsTempl =
      "struct1 ="
      "[*"
      "=> nrel_idtf:"
      "[Identifier1] (* <- lang_en;; *);;"
      "=> nrel_relation_1:"
      "other;;"
      "*];;";

  EXPECT_TRUE(helper.GenerateBySCsText(scsTempl));
  auto links = m_ctx->FindLinksByContent("Identifier1");
  EXPECT_FALSE(links.empty());

  ScAddr const structAddr = m_ctx->HelperFindBySystemIdtf("struct1");
  ScTemplate templ;
  templ.TripleWithRelation(
      structAddr,
      ScType::EdgeDCommonVar,
      ScType::LinkVar >> "_link",
      ScType::EdgeAccessVarPosPerm,
      m_ctx->HelperFindBySystemIdtf("nrel_idtf"));

  ScTemplateSearchResult result;
  m_ctx->HelperSearchTemplate(templ, result);
  EXPECT_FALSE(result.IsEmpty());

  ScAddr found;
  for (auto const & item : links)
  {
    if (item == result[0]["_link"])
    {
      found = item;
      break;
    }
  }
  EXPECT_TRUE(found.IsValid());
  EXPECT_TRUE(m_ctx->HelperCheckEdge(m_ctx->HelperFindBySystemIdtf("lang_en"), found, ScType::EdgeAccessConstPosPerm));

  EXPECT_TRUE(m_ctx->HelperCheckEdge(structAddr, m_ctx->HelperFindBySystemIdtf("other"), ScType::EdgeDCommonConst));
}
