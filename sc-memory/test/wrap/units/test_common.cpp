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
  SC_CHECK_EQUAL(ctx.GetElementType(link), ScType::LinkConst, ());
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

  SUBTEST_START(ParseNumbers)
  {
    auto const resultFloat = utils::StringUtils::ParseNumber<float>("7.56");
    SC_CHECK(resultFloat, ());
    SC_CHECK_EQUAL(*resultFloat, 7.56f, ());

    auto const resultDouble = utils::StringUtils::ParseNumber<double>("8.56");
    SC_CHECK(resultDouble, ());
    SC_CHECK_EQUAL(*resultDouble, 8.56, ());

    auto const resultInt8 = utils::StringUtils::ParseNumber<int8_t>("8");
    SC_CHECK(resultInt8, ())
    SC_CHECK_EQUAL(*resultInt8, 8, ());

    auto const resultInt16 = utils::StringUtils::ParseNumber<int16_t>("16");
    SC_CHECK(resultInt16, ())
    SC_CHECK_EQUAL(*resultInt16, 16, ());

    auto const resultInt32 = utils::StringUtils::ParseNumber<int32_t>("32");
    SC_CHECK(resultInt32, ())
    SC_CHECK_EQUAL(*resultInt32, 32, ());

    auto const resultInt64 = utils::StringUtils::ParseNumber<int64_t>("64");
    SC_CHECK(resultInt64, ());
    SC_CHECK_EQUAL(*resultInt64, 64, ());

    auto const resultUint8 = utils::StringUtils::ParseNumber<uint8_t>("8");
    SC_CHECK(resultUint8, ());
    SC_CHECK_EQUAL(*resultUint8, 8, ());

    auto const resultUint16 = utils::StringUtils::ParseNumber<uint16_t>("16");
    SC_CHECK(resultUint16, ());
    SC_CHECK_EQUAL(*resultUint16, 16, ());

    auto const resultUint32 = utils::StringUtils::ParseNumber<uint32_t>("32");
    SC_CHECK(resultUint32, ());
    SC_CHECK_EQUAL(*resultUint32, 32, ());

    auto const resultUint64 = utils::StringUtils::ParseNumber<uint64_t>("64");
    SC_CHECK(resultUint64, ());
    SC_CHECK_EQUAL(*resultUint64, 64, ());
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

UNIT_TEST(ScType)
{
  SUBTEST_START(nodes)
  {
    SC_CHECK(ScType::Node.IsNode(), ());

    SC_CHECK(ScType::NodeConst.IsNode(), ());
    SC_CHECK(ScType::NodeConstAbstract.IsNode(), ());
    SC_CHECK(ScType::NodeConstClass.IsNode(), ());
    SC_CHECK(ScType::NodeConstMaterial.IsNode(), ());
    SC_CHECK(ScType::NodeConstNoRole.IsNode(), ());
    SC_CHECK(ScType::NodeConstRole.IsNode(), ());
    SC_CHECK(ScType::NodeConstStruct.IsNode(), ());
    SC_CHECK(ScType::NodeConstTuple.IsNode(), ());

    SC_CHECK(ScType::NodeVar.IsNode(), ());
    SC_CHECK(ScType::NodeVarAbstract.IsNode(), ());
    SC_CHECK(ScType::NodeVarClass.IsNode(), ());
    SC_CHECK(ScType::NodeVarMaterial.IsNode(), ());
    SC_CHECK(ScType::NodeVarNoRole.IsNode(), ());
    SC_CHECK(ScType::NodeVarRole.IsNode(), ());
    SC_CHECK(ScType::NodeVarStruct.IsNode(), ());
    SC_CHECK(ScType::NodeVarTuple.IsNode(), ());
  }
  SUBTEST_END()

  SUBTEST_START(constancy)
  {
    SC_CHECK_NOT(ScType::Node.IsConst(), ());
    SC_CHECK_NOT(ScType::Node.IsVar(), ());

    SC_CHECK(ScType::LinkConst.IsConst(), ());

    SC_CHECK(ScType::NodeConst.IsConst(), ());
    SC_CHECK(ScType::NodeConstAbstract.IsConst(), ());
    SC_CHECK(ScType::NodeConstClass.IsConst(), ());
    SC_CHECK(ScType::NodeConstMaterial.IsConst(), ());
    SC_CHECK(ScType::NodeConstNoRole.IsConst(), ());
    SC_CHECK(ScType::NodeConstRole.IsConst(), ());
    SC_CHECK(ScType::NodeConstStruct.IsConst(), ());
    SC_CHECK(ScType::NodeConstTuple.IsConst(), ());
    SC_CHECK(ScType::EdgeDCommonConst.IsConst(), ());
    SC_CHECK(ScType::EdgeUCommonConst.IsConst(), ());
    SC_CHECK(ScType::EdgeAccessConstFuzPerm.IsConst(), ());
    SC_CHECK(ScType::EdgeAccessConstFuzTemp.IsConst(), ());
    SC_CHECK(ScType::EdgeAccessConstNegPerm.IsConst(), ());
    SC_CHECK(ScType::EdgeAccessConstNegTemp.IsConst(), ());
    SC_CHECK(ScType::EdgeAccessConstPosPerm.IsConst(), ());
    SC_CHECK(ScType::EdgeAccessConstPosTemp.IsConst(), ());

    SC_CHECK(ScType::NodeVar.IsVar(), ());
    SC_CHECK(ScType::LinkVar.IsVar(), ());
    SC_CHECK(ScType::NodeVarAbstract.IsVar(), ());
    SC_CHECK(ScType::NodeVarClass.IsVar(), ());
    SC_CHECK(ScType::NodeVarMaterial.IsVar(), ());
    SC_CHECK(ScType::NodeVarNoRole.IsVar(), ());
    SC_CHECK(ScType::NodeVarRole.IsVar(), ());
    SC_CHECK(ScType::NodeVarStruct.IsVar(), ());
    SC_CHECK(ScType::NodeVarTuple.IsVar(), ());
    SC_CHECK(ScType::EdgeDCommonVar.IsVar(), ());
    SC_CHECK(ScType::EdgeUCommonVar.IsVar(), ());
    SC_CHECK(ScType::EdgeAccessVarFuzPerm.IsVar(), ());
    SC_CHECK(ScType::EdgeAccessVarFuzTemp.IsVar(), ());
    SC_CHECK(ScType::EdgeAccessVarNegPerm.IsVar(), ());
    SC_CHECK(ScType::EdgeAccessVarNegTemp.IsVar(), ());
    SC_CHECK(ScType::EdgeAccessVarPosPerm.IsVar(), ());
    SC_CHECK(ScType::EdgeAccessVarPosTemp.IsVar(), ());
  }
  SUBTEST_END()

  SUBTEST_START(edges)
  {
    SC_CHECK(ScType::EdgeAccess.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessConstFuzPerm.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessConstFuzTemp.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessConstNegPerm.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessConstNegTemp.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessConstPosPerm.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessConstPosTemp.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessVarFuzPerm.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessVarFuzTemp.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessVarNegPerm.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessVarNegTemp.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessVarPosPerm.IsEdge(), ());
    SC_CHECK(ScType::EdgeAccessVarPosTemp.IsEdge(), ());
    SC_CHECK(ScType::EdgeDCommon.IsEdge(), ());
    SC_CHECK(ScType::EdgeDCommonConst.IsEdge(), ());
    SC_CHECK(ScType::EdgeDCommonVar.IsEdge(), ());
    SC_CHECK(ScType::EdgeUCommon.IsEdge(), ());
    SC_CHECK(ScType::EdgeUCommonConst.IsEdge(), ());
    SC_CHECK(ScType::EdgeUCommonVar.IsEdge(), ());
  }
  SUBTEST_END()

  SUBTEST_START(extend)
  {
    SC_CHECK(ScType::Node.CanExtendTo(ScType::Node), ());
    SC_CHECK(ScType::Node.CanExtendTo(ScType::NodeConst), ());
    SC_CHECK(ScType::Node.CanExtendTo(ScType::NodeVar), ());
    SC_CHECK(ScType::Node.CanExtendTo(ScType::NodeMaterial), ());
    SC_CHECK(ScType::Node.CanExtendTo(ScType::NodeConstMaterial), ());

    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::Node), ());
    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::NodeConst), ());
    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::EdgeAccessConstFuzPerm), ());
    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::Link), ());
    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::EdgeAccess), ());
    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::EdgeUCommon), ());
    SC_CHECK(ScType::Unknown.CanExtendTo(ScType::EdgeDCommon), ());

    SC_CHECK_NOT(ScType::Node.CanExtendTo(ScType::EdgeAccess), ());
    SC_CHECK_NOT(ScType::NodeAbstract.CanExtendTo(ScType::NodeConstMaterial), ());
    SC_CHECK_NOT(ScType::Link.CanExtendTo(ScType::Node), ());
    SC_CHECK_NOT(ScType::EdgeAccess.CanExtendTo(ScType::EdgeDCommon), ());
    SC_CHECK_NOT(ScType::EdgeAccess.CanExtendTo(ScType::EdgeUCommon), ());
    SC_CHECK_NOT(ScType::EdgeAccess.CanExtendTo(ScType::Link), ());
    SC_CHECK_NOT(ScType::Const.CanExtendTo(ScType::Var), ());
    SC_CHECK_NOT(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessConstFuzTemp), ());
    SC_CHECK_NOT(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessConstNegPerm), ());
    SC_CHECK_NOT(ScType::EdgeAccessConstFuzPerm.CanExtendTo(ScType::EdgeAccessVarFuzPerm), ());
  }
  SUBTEST_END()
}
