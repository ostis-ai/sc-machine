/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

TEST_F(ScBuilderTest, Aliases)
{
  LoadKB(m_ctx, {"aliases.scs"});

  std::string const scsTempl = "test_element _-> _[] (* _<- alias_content;; *);;";

  ScTemplate templ;
  m_ctx->BuildTemplate(templ, scsTempl);

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);
}
