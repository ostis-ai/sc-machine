/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <sc-memory/sc_link.hpp>

TEST_F(ScBuilderTest, clean_global_idtfs)
{
  ScAddr const nrelSCsIdtf = m_ctx->ResolveElementSystemIdentifier("nrel_scs_global_idtf");
  EXPECT_TRUE(nrelSCsIdtf.IsValid());

  ScTemplate templ;
  templ.Quintuple(
      ScType::Unknown, ScType::VarCommonArc, ScType::NodeLink >> "_link", ScType::VarPermPosArc, nrelSCsIdtf);

  ScTemplateSearchResult res;
  EXPECT_FALSE(m_ctx->SearchByTemplate(templ, res));
  EXPECT_EQ(res.Size(), 0u);
}
