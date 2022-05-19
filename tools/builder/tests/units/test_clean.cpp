#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"

#include "builder_test.hpp"

TEST_F(ScBuilderTest, clean_global_idtfs)
{
  ScAddr const nrelSCsIdtf = m_ctx->HelperResolveSystemIdtf("nrel_scs_global_idtf");
  EXPECT_TRUE(nrelSCsIdtf.IsValid());

  ScTemplate templ;
  templ.TripleWithRelation(
    ScType::Unknown,
    ScType::EdgeDCommonVar,
    ScType::Link >> "_link",
    ScType::EdgeAccessVarPosPerm,
    nrelSCsIdtf);

  ScTemplateSearchResult res;
  EXPECT_FALSE(m_ctx->HelperSearchTemplate(templ, res));
  EXPECT_EQ(res.Size(), 0u);
}
