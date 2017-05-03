/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

UNIT_TEST(elements)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "elements");

  ScAddr addr = ctx.CreateNode(ScType::Const);
  SC_CHECK(addr.IsValid(), ());

  ScAddr link = ctx.CreateLink();
  SC_CHECK(link.IsValid(), ());

  ScAddr arc = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr, link);
  SC_CHECK(arc.IsValid(), ());

  SC_CHECK(ctx.IsElement(addr), ());
  SC_CHECK(ctx.IsElement(link), ());
  SC_CHECK(ctx.IsElement(arc), ());

  SC_CHECK_EQUAL(ctx.GetEdgeSource(arc), addr, ());
  SC_CHECK_EQUAL(ctx.GetEdgeTarget(arc), link, ());

  SC_CHECK_EQUAL(ctx.GetElementType(addr), ScType::NodeConst, ());
  SC_CHECK_EQUAL(ctx.GetElementType(link), ScType::Link, ());
  SC_CHECK_EQUAL(ctx.GetElementType(arc), ScType::EdgeAccessConstPosPerm, ());

  SC_CHECK(ctx.SetElementSubtype(addr, ScType::Var), ());
  SC_CHECK_EQUAL(ctx.GetElementType(addr), ScType::NodeVar, ());

  SC_CHECK(ctx.EraseElement(addr), ());
  SC_CHECK(!ctx.IsElement(addr), ());
  SC_CHECK(!ctx.IsElement(arc), ());
  SC_CHECK(ctx.IsElement(link), ());
}

UNIT_TEST(StringUtils)
{
  SUBTEST_START(simple)
  {
    std::string trimLeft = "  value";
    std::string trimRight = "value  ";
    std::string trim = " value  ";
    std::string empty = "     ";

    utils::StringUtils::TrimLeft(trimLeft);
    SC_CHECK_EQUAL(trimLeft, "value", ());

    utils::StringUtils::TrimRight(trimRight);
    SC_CHECK_EQUAL(trimRight, "value", ());

    utils::StringUtils::Trim(trim);
    SC_CHECK_EQUAL(trim, "value", ());

    utils::StringUtils::Trim(empty);
    SC_CHECK_EQUAL(empty, "", ());

    StringVector res;
    // Do not return empty item after ;
    utils::StringUtils::SplitString("begin;end;", ';', res);
    SC_CHECK_EQUAL(res.size(), 2, ());
    SC_CHECK_EQUAL("begin", res[0], ());
    SC_CHECK_EQUAL("end", res[1], ());
  }
  SUBTEST_END()

  SUBTEST_START(complex)
  {
    std::string value = "  value value \t\n ";
    utils::StringUtils::Trim(value);

    SC_CHECK_EQUAL(value, "value value", ());
  }
  SUBTEST_END()
}

UNIT_TEST(ScAddr)
{
  SUBTEST_START(hash)
  {
    sc_addr a;
    a.offset = 123;
    a.seg = 654;

    ScAddr const addr1(a);
    auto const hash = addr1.Hash();
    ScAddr const addr2(hash);
    SC_CHECK_EQUAL(addr1, addr2, ());
  }
  SUBTEST_END()
}