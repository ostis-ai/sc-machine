#include <gtest/gtest.h>

#include "builder_test.hpp"

TEST_F(ScBuilderTest, Smoke)
{
  ScMemoryContext ctx("Builder_Base");

  std::string const scsData =
    "base"
    " _<- test_element;"
    " _=> nrel_main_idtf::"
    "    _[]"
    "    (* _<- lang_en;; *);;";

  ScTemplate templ;
  EXPECT_TRUE(ctx.HelperBuildTemplate(templ, scsData));

  ScTemplateSearchResult res;
  EXPECT_TRUE(ctx.HelperSearchTemplate(templ, res));
}
