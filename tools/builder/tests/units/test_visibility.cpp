#include "../test_unit.hpp"

#include "sc-memory/sc_link.hpp"

UNIT_TEST_CUSTOM(Builder_Visibility, BuilderTestUnit)
{
  ScMemoryContext ctx("Builder_Visibility");

  ScAddr const visFirst = ctx.HelperResolveSystemIdtf("visibility_first");
  SC_CHECK(visFirst.IsValid(), ());

  ScAddr const visSecond = ctx.HelperResolveSystemIdtf("visibility_second");
  SC_CHECK(visSecond.IsValid(), ());

  ScAddr const nrelIdtf = ctx.HelperResolveSystemIdtf("nrel_idtf");
  SC_CHECK(nrelIdtf.IsValid(), ());

  auto const GetIdtf = [&nrelIdtf, &ctx](ScAddr const& addr) -> std::string
  {
    ScTemplate templ;

    templ.TripleWithRelation(
      addr,
      ScType::EdgeDCommonVar,
      ScType::Link >> "_link",
      ScType::EdgeAccessVarPosPerm,
      nrelIdtf);

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
    SC_CHECK_EQUAL(result.Size(), 1, ());

    ScAddr const linkAddr = result[0]["_link"];
    SC_CHECK(linkAddr.IsValid(), ());

    ScLink link(ctx, linkAddr);
    SC_CHECK(link.IsType<std::string>(), ());

    return link.Get<std::string>();
  };

  SUBTEST_START(system_idtf)
  {
    ScAddr const element = ctx.HelperResolveSystemIdtf("visibility_sys_idtf");
    SC_CHECK(element.IsValid(), ());

    ScTemplate templ;

    templ.Triple(
      visFirst,
      ScType::EdgeAccessVarPosPerm,
      element);

    templ.Triple(
      visSecond,
      ScType::EdgeAccessVarPosPerm,
      element);

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
  }
  SUBTEST_END()

  SUBTEST_START(global)
  {
    ScTemplate templ;

    templ.Triple(
      visFirst,
      ScType::EdgeAccessVarPosPerm,
      ScType::Node >> ".visibility_global");

    templ.Triple(
      visSecond,
      ScType::EdgeAccessVarPosPerm,
      ".visibility_global");

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
    SC_CHECK_EQUAL(result.Size(), 1, ());

    ScAddr const element = result[0][".visibility_global"];
    SC_CHECK(element.IsValid(), ());

    SC_CHECK_EQUAL(GetIdtf(element), ".visibility_global", ());
  }
  SUBTEST_END()

  SUBTEST_START(local)
  {
    ScAddr const visLocal = ctx.HelperResolveSystemIdtf("visibility_local");
    SC_CHECK(visLocal.IsValid(), ());

    ScTemplate templ;

    templ.Triple(
      visLocal,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> "_local");

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());

    SC_CHECK_EQUAL(result.Size(), 2, ());

    std::map<std::string, ScAddr> elements;
    for (size_t i = 0; i < result.Size(); ++i)
    {
      auto const & item = result[i];
      ScAddr const a = item["_local"];

      SC_CHECK(a.IsValid(), ());

      std::string const idtf = GetIdtf(a);
      SC_CHECK(!idtf.empty(), ());

      elements[idtf] = a;
    }

    SC_CHECK_EQUAL(elements.size(), 2, ());

    ScAddr const localFirst = elements["..visibility_local_first"];
    SC_CHECK(localFirst.IsValid(), ());

    ScAddr const localSecond = elements["..visibility_local_second"];
    SC_CHECK(localSecond.IsValid(), ());

    SC_CHECK(localFirst != localSecond, ());
  }
  SUBTEST_END()
}