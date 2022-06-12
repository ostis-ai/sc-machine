#include <gtest/gtest.h>

#include "builder_test.hpp"

TEST_F(ScBuilderTest, aliases)
{
  std::string const scsTempl = "test_element _-> _[] (* _<- alias_content;; *);;";
  
  ScTemplate templ;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, scsTempl));

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
  EXPECT_EQ(result.Size(), 1u);
}
