/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"

#include "builder_test.hpp"

TEST_F(ScBuilderTest, clean_global_idtfs)
{
  ScAddr const nrelSCsIdtf = m_ctx->HelperResolveSystemIdtf("nrel_scs_global_idtf");
  EXPECT_TRUE(nrelSCsIdtf.IsValid());

  ScTemplate templ;
  templ.Quintuple(
      ScType::Unknown, ScType::EdgeDCommonVar, ScType::Link >> "_link", ScType::EdgeAccessVarPosPerm, nrelSCsIdtf);

  ScTemplateSearchResult res;
  EXPECT_FALSE(m_ctx->HelperSearchTemplate(templ, res));
  EXPECT_EQ(res.Size(), 0u);
}
