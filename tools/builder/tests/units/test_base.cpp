#include "../test_unit.hpp"

UNIT_TEST_CUSTOM(Builder_Base, BuilderTestUnit)
{
  ScMemoryContext ctx("Builder_Base");

  /*
  base 
    <- test_element;
    => nrel_main_idtf:
        [base]
        (* <- lang_en;; *);;
  */

  std::string const scsData =
    "base"
    " _<- test_element;"
    " _=> nrel_main_idtf::"
    "    _[]"
    "    (* _<- lang_en;; *);;";

  ScTemplate templ;
  SC_CHECK(ctx.HelperBuildTemplate(templ, scsData), ()); 

  ScTemplateSearchResult res;
  SC_CHECK(ctx.HelperSearchTemplate(templ, res), ());
}