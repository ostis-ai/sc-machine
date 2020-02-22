#include "../test_unit.hpp"

#include "sc-memory/sc_link.hpp"

UNIT_TEST_CUSTOM(Builder_CheckClean, BuilderTestUnit)
{
  ScMemoryContext ctx("Builder_CheckClean");

  SUBTEST_START(clean_global_idtfs)
  {
    ScAddr const nrelSCsIdtf = ctx.HelperResolveSystemIdtf("nrel_scs_global_idtf");
    SC_CHECK(nrelSCsIdtf.IsValid(), ());

    ScTemplate templ;

    templ.TripleWithRelation(
      ScType::Unknown,
      ScType::EdgeDCommonVar,
      ScType::Link >> "_link",
      ScType::EdgeAccessVarPosPerm,
      nrelSCsIdtf);

    ScTemplateSearchResult res;
    SC_CHECK(!ctx.HelperSearchTemplate(templ, res), ());
    SC_CHECK_EQUAL(res.Size(), 0, ());
  }
  SUBTEST_END()
}
