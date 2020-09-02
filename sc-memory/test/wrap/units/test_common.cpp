/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

//#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

#include "sc-memory/cpp/utils/sc_test.hpp"

TEST_CASE("Elements", "[test common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "elements");

  ScAddr addr = ctx.CreateNode(ScType::Const);
  REQUIRE(addr.IsValid());

  ScAddr link = ctx.CreateLink();
  REQUIRE(link.IsValid());

  ScAddr arc = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr, link);
  REQUIRE(arc.IsValid());

  REQUIRE(ctx.IsElement(addr));
  REQUIRE(ctx.IsElement(link));
  REQUIRE(ctx.IsElement(arc));

  SC_CHECK_EQUAL(ctx.GetEdgeSource(arc), addr, ());
  SC_CHECK_EQUAL(ctx.GetEdgeTarget(arc), link, ());

  SC_CHECK_EQUAL(ctx.GetElementType(addr), ScType::NodeConst, ());
  //SC_CHECK_EQUAL(ctx.GetElementType(link), ScType::Link, ());
  SC_CHECK_EQUAL(ctx.GetElementType(arc), ScType::EdgeAccessConstPosPerm, ());

  REQUIRE(ctx.SetElementSubtype(addr, ScType::Var));
  SC_CHECK_EQUAL(ctx.GetElementType(addr), ScType::NodeVar, ());

  REQUIRE(ctx.EraseElement(addr));
  REQUIRE(!ctx.IsElement(addr));
  REQUIRE(!ctx.IsElement(arc));
  REQUIRE(ctx.IsElement(link));

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("StringUtils", "[test common]")
{
  SECTION("simple")
  {
    SUBTEST_START("simple")
    {
      std::string trimLeft = "  value";
      std::string trimRight = "value  ";
      std::string trim = " value  ";
      std::string empty = "     ";

      utils::StringUtils::TrimLeft(trimLeft);
      REQUIRE_THAT(trimLeft, Catch::Equals("value"));

      utils::StringUtils::TrimRight(trimRight);
      REQUIRE_THAT(trimRight, Catch::Equals("value"));

      utils::StringUtils::Trim(trim);
      REQUIRE_THAT(trim, Catch::Equals("value"));

      utils::StringUtils::Trim(empty);
      REQUIRE_THAT(empty, Catch::Equals(""));

      StringVector res;
      // Do not return empty item after ;
      utils::StringUtils::SplitString("begin;end;", ';', res);
      REQUIRE(res.size() == 2);
      REQUIRE_THAT("begin", Catch::Equals(res[0]));
      REQUIRE_THAT("end", Catch::Equals(res[1]));
    }
    SUBTEST_END()
  }

  SECTION("complex")
  {
    SUBTEST_START("complex")
    {
      std::string value = "  value value \t\n ";
      utils::StringUtils::Trim(value);

      REQUIRE_THAT(value, Catch::Equals("value value"));
    }
    SUBTEST_END()
  }
}

TEST_CASE("ScAddr", "[test common]")
{
  SECTION("hash")
  {
    SUBTEST_START("hash")
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
}

TEST_CASE("ScType", "[test common]")
{
  SECTION("nodes")
  {
    SUBTEST_START("nodes")
    {
      REQUIRE(ScType::Node.IsNode());

      REQUIRE(ScType::NodeConst.IsNode());
      REQUIRE(ScType::NodeConstAbstract.IsNode());
      REQUIRE(ScType::NodeConstClass.IsNode());
      REQUIRE(ScType::NodeConstMaterial.IsNode());
      REQUIRE(ScType::NodeConstNoRole.IsNode());
      REQUIRE(ScType::NodeConstRole.IsNode());
      REQUIRE(ScType::NodeConstStruct.IsNode());
      REQUIRE(ScType::NodeConstTuple.IsNode());

      REQUIRE(ScType::NodeVar.IsNode());
      REQUIRE(ScType::NodeVarAbstract.IsNode());
      REQUIRE(ScType::NodeVarClass.IsNode());
      REQUIRE(ScType::NodeVarMaterial.IsNode());
      REQUIRE(ScType::NodeVarNoRole.IsNode());
      REQUIRE(ScType::NodeVarRole.IsNode());
      REQUIRE(ScType::NodeVarStruct.IsNode());
      REQUIRE(ScType::NodeVarTuple.IsNode());
    }
    SUBTEST_END()
  }

  SECTION("constancy")
  {
    SUBTEST_START("constancy")
    {
      REQUIRE_FALSE(ScType::Node.IsConst());
      REQUIRE_FALSE(ScType::Node.IsVar());

      REQUIRE(ScType::LinkConst.IsConst());

      REQUIRE(ScType::NodeConst.IsConst());
      REQUIRE(ScType::NodeConstAbstract.IsConst());
      REQUIRE(ScType::NodeConstClass.IsConst());
      REQUIRE(ScType::NodeConstMaterial.IsConst());
      REQUIRE(ScType::NodeConstNoRole.IsConst());
      REQUIRE(ScType::NodeConstRole.IsConst());
      REQUIRE(ScType::NodeConstStruct.IsConst());
      REQUIRE(ScType::NodeConstTuple.IsConst());
      REQUIRE(ScType::EdgeDCommonConst.IsConst());
      REQUIRE(ScType::EdgeUCommonConst.IsConst());
      REQUIRE(ScType::EdgeAccessConstFuzPerm.IsConst());
      REQUIRE(ScType::EdgeAccessConstFuzTemp.IsConst());
      REQUIRE(ScType::EdgeAccessConstNegPerm.IsConst());
      REQUIRE(ScType::EdgeAccessConstNegTemp.IsConst());
      REQUIRE(ScType::EdgeAccessConstPosPerm.IsConst());
      REQUIRE(ScType::EdgeAccessConstPosTemp.IsConst());

      REQUIRE(ScType::NodeVar.IsVar());
      REQUIRE(ScType::LinkVar.IsVar());
      REQUIRE(ScType::NodeVarAbstract.IsVar());
      REQUIRE(ScType::NodeVarClass.IsVar());
      REQUIRE(ScType::NodeVarMaterial.IsVar());
      REQUIRE(ScType::NodeVarNoRole.IsVar());
      REQUIRE(ScType::NodeVarRole.IsVar());
      REQUIRE(ScType::NodeVarStruct.IsVar());
      REQUIRE(ScType::NodeVarTuple.IsVar());
      REQUIRE(ScType::EdgeDCommonVar.IsVar());
      REQUIRE(ScType::EdgeUCommonVar.IsVar());
      REQUIRE(ScType::EdgeAccessVarFuzPerm.IsVar());
      REQUIRE(ScType::EdgeAccessVarFuzTemp.IsVar());
      REQUIRE(ScType::EdgeAccessVarNegPerm.IsVar());
      REQUIRE(ScType::EdgeAccessVarNegTemp.IsVar());
      REQUIRE(ScType::EdgeAccessVarPosPerm.IsVar());
      REQUIRE(ScType::EdgeAccessVarPosTemp.IsVar());
    }
    SUBTEST_END()
  }

  SECTION("edges")
  {
    SUBTEST_START("edges")
    {
      REQUIRE(ScType::EdgeAccess.IsEdge());
      REQUIRE(ScType::EdgeAccessConstFuzPerm.IsEdge());
      REQUIRE(ScType::EdgeAccessConstFuzTemp.IsEdge());
      REQUIRE(ScType::EdgeAccessConstNegPerm.IsEdge());
      REQUIRE(ScType::EdgeAccessConstNegTemp.IsEdge());
      REQUIRE(ScType::EdgeAccessConstPosPerm.IsEdge());
      REQUIRE(ScType::EdgeAccessConstPosTemp.IsEdge());
      REQUIRE(ScType::EdgeAccessVarFuzPerm.IsEdge());
      REQUIRE(ScType::EdgeAccessVarFuzTemp.IsEdge());
      REQUIRE(ScType::EdgeAccessVarNegPerm.IsEdge());
      REQUIRE(ScType::EdgeAccessVarNegTemp.IsEdge());
      REQUIRE(ScType::EdgeAccessVarPosPerm.IsEdge());
      REQUIRE(ScType::EdgeAccessVarPosTemp.IsEdge());
      REQUIRE(ScType::EdgeDCommon.IsEdge());
      REQUIRE(ScType::EdgeDCommonConst.IsEdge());
      REQUIRE(ScType::EdgeDCommonVar.IsEdge());
      REQUIRE(ScType::EdgeUCommon.IsEdge());
      REQUIRE(ScType::EdgeUCommonConst.IsEdge());
      REQUIRE(ScType::EdgeUCommonVar.IsEdge());
    }
    SUBTEST_END()
  }

  SECTION("extend")
  {
    SUBTEST_START("extend")
    {
      REQUIRE(ScType::Node.CanExtendTo(ScType::Node));
      REQUIRE(ScType::Node.CanExtendTo(ScType::NodeConst));
      REQUIRE(ScType::Node.CanExtendTo(ScType::NodeVar));
      REQUIRE(ScType::Node.CanExtendTo(ScType::NodeMaterial));
      REQUIRE(ScType::Node.CanExtendTo(ScType::NodeConstMaterial));

      REQUIRE(ScType::Unknown.CanExtendTo(ScType::Node));
      REQUIRE(ScType::Unknown.CanExtendTo(ScType::NodeConst));
      REQUIRE(ScType::Unknown.CanExtendTo(ScType::EdgeAccessConstFuzPerm));
      REQUIRE(ScType::Unknown.CanExtendTo(ScType::Link));
      REQUIRE(ScType::Unknown.CanExtendTo(ScType::EdgeAccess));
      REQUIRE(ScType::Unknown.CanExtendTo(ScType::EdgeUCommon));
      REQUIRE(ScType::Unknown.CanExtendTo(ScType::EdgeDCommon));

      REQUIRE_FALSE(ScType::Node.CanExtendTo(ScType::EdgeAccess));
      REQUIRE_FALSE(ScType::NodeAbstract.CanExtendTo(ScType::NodeConstMaterial));
      REQUIRE_FALSE(ScType::Link.CanExtendTo(ScType::Node));
      REQUIRE_FALSE(ScType::EdgeAccess.CanExtendTo(ScType::EdgeDCommon));
      REQUIRE_FALSE(ScType::EdgeAccess.CanExtendTo(ScType::EdgeUCommon));
      REQUIRE_FALSE(ScType::EdgeAccess.CanExtendTo(ScType::Link));
      REQUIRE_FALSE(ScType::Const.CanExtendTo(ScType::Var));
      REQUIRE_FALSE(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessConstFuzTemp));
      REQUIRE_FALSE(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessConstNegPerm));
      REQUIRE_FALSE(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessVarFuzPerm));
    }
    SUBTEST_END()
  }
}
