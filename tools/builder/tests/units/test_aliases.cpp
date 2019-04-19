#include "../test_unit.hpp"

UNIT_TEST_CUSTOM(Builder_Aliases, BuilderTestUnit)
{
  ScMemoryContext ctx("Builder_Aliases");

  std::string const scsTempl = "test_element _-> _[] (* _<- alias_content;; *);;";
  
  ScTemplate templ;
  auto const res = ctx.HelperBuildTemplate(templ, scsTempl);

  SC_CHECK(res, (res.Msg()));

  ScTemplateSearchResult result;
  SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
  SC_CHECK_EQUAL(result.Size(), 1, ());
}