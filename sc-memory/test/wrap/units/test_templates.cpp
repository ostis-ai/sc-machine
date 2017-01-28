/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_debug.hpp"
#include "sc-memory/cpp/sc_template.hpp"
#include "sc-memory/cpp/sc_struct.hpp"
#include "sc-memory/cpp/utils/sc_progress.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

#include <glib.h>

#include <random>

namespace
{

struct TestTemplParams
{
  explicit TestTemplParams(ScMemoryContext & ctx)
    : mCtx(ctx)
  {
  }

  bool operator () (ScTemplateItemValue param1, ScTemplateItemValue param2, ScTemplateItemValue param3)
  {
    bool catched = false;
    try
    {
      ScTemplate testTempl;
      testTempl(param1, param2, param3);

      ScTemplateGenResult res;
      SC_CHECK(mCtx.HelperGenTemplate(testTempl, res), ());
    }
    catch (utils::ExceptionInvalidParams & e)
    {
      (void)e;
      catched = true;
    }

    return !catched;
  }

private:
  ScMemoryContext & mCtx;
};


bool HasAddr(ScAddrVector const & v, ScAddr const & addr)
{
  for (auto const & it : v)
  {
    if (it == addr)
      return true;
  }

  return false;
}

ScAddr ResolveKeynode(ScMemoryContext & ctx, ScType const & type, std::string const & idtf)
{
  ScAddr addr;
  ctx.HelperResolveSystemIdtf(idtf, addr, true);
  SC_CHECK(addr.IsValid(), ());
  return addr;
}

} // namespace


UNIT_TEST(templates_common)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "templates_common");
  ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(addr1.IsValid(), ());

  ScAddr const addr2 = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(addr2.IsValid(), ());

  ScAddr const addr3 = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(addr3.IsValid(), ());

  ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  SC_CHECK(edge1.IsValid(), ());

  ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr3, edge1);
  SC_CHECK(edge2.IsValid(), ());

  {
    ScTemplate templ;

    templ.Triple(
          addr1 >> "addr1",
          ScType::EdgeAccessVarPosPerm >> "edge1",
          ScType::NodeVar >> "addr2");
    templ.Triple(
          ScType::NodeVar >> "_addr1T2",
          ScType::EdgeAccessVarPosPerm >> "_addr2T2",
          "edge1");
    templ.Triple(
          "addr2",
          ScType::EdgeDCommonVar >> "_addr2T3",
          "edge1");

    ScTemplateGenResult result;
    SC_CHECK(ctx.HelperGenTemplate(templ, result), ());

    ScIterator5Ptr it5 = ctx.Iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);
    SC_CHECK(it5->Next(), ());
    SC_CHECK_EQUAL(it5->Get(0), result["addr1"], ());
    SC_CHECK_EQUAL(it5->Get(1), result["edge1"], ());
    SC_CHECK_EQUAL(it5->Get(2), result["addr2"], ());
    SC_CHECK_EQUAL(it5->Get(3), result["_addr2T2"], ());
    SC_CHECK_EQUAL(it5->Get(4), result["_addr1T2"], ());

    ScIterator3Ptr it3 = ctx.Iterator3(result["addr2"], sc_type_arc_common, sc_type_arc_pos_const_perm);
    SC_CHECK(it3->Next(), ());
    SC_CHECK_EQUAL(it3->Get(0), result["addr2"], ());
    SC_CHECK_EQUAL(it3->Get(1), result["_addr2T3"], ());
    SC_CHECK_EQUAL(it3->Get(2), result["edge1"], ());


    SUBTEST_START(template_search)
    {
      ScTemplateSearchResult searchResult;
      SC_CHECK(ctx.HelperSearchTemplate(templ, searchResult), ());

      SC_CHECK_EQUAL(searchResult.Size(), 1, ());

      ScTemplateSearchResultItem res = searchResult[0];

      SC_CHECK_EQUAL(it5->Get(0), res["addr1"], ());
      SC_CHECK_EQUAL(it5->Get(1), res["edge1"], ());
      SC_CHECK_EQUAL(it5->Get(2), res["addr2"], ());
      SC_CHECK_EQUAL(it5->Get(3), result["_addr2T2"], ());
      SC_CHECK_EQUAL(it5->Get(4), result["_addr1T2"], ());

      SC_CHECK_EQUAL(it3->Get(0), res["addr2"], ());
      SC_CHECK_EQUAL(it3->Get(1), result["_addr2T3"], ());
      SC_CHECK_EQUAL(it3->Get(2), res["edge1"], ());
    }
    SUBTEST_END()

    SUBTEST_START(template_search2)
    {
      size_t const testCount = 10;
      ScAddrVector nodes, edges;

      ScAddr addrSrc = ctx.CreateNode(sc_type_const);
      SC_CHECK(addrSrc.IsValid(), ());
      for (size_t i = 0; i < testCount; ++i)
      {
        ScAddr const addrTrg = ctx.CreateNode(sc_type_const);
        SC_CHECK(addrTrg.IsValid(), ());

        ScAddr const addrEdge = ctx.CreateEdge(sc_type_arc_pos_const_perm, addrSrc, addrTrg);
        SC_CHECK(addrEdge.IsValid(), ());

        nodes.push_back(addrTrg);
        edges.push_back(addrEdge);
      }

      ScTemplate templ2;

      templ2.Triple(addrSrc >> "addrSrc",
                    ScType::EdgeAccessVarPosPerm >> "edge",
                    ScType::NodeVar >> "addrTrg");

      ScTemplateSearchResult result2;
      SC_CHECK(ctx.HelperSearchTemplate(templ2, result2), ());

      size_t const count = result2.Size();
      for (size_t i = 0; i < count; ++i)
      {
        ScTemplateSearchResultItem r = result2[i];

        SC_CHECK_EQUAL(r["addrSrc"], addrSrc, ());

        SC_CHECK(HasAddr(edges, r["edge"]), ());
        SC_CHECK(HasAddr(nodes, r["addrTrg"]), ());
      }
    }
    SUBTEST_END()
  }

  SUBTEST_START(template_tripleWithRelation)
  {
    ScTemplate templ;

    templ.TripleWithRelation(
          addr1,
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVar,
          ScType::EdgeAccessVarPosPerm,
          addr3);

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
    SC_CHECK_GREAT(result.Size(), 0, ());
  }
  SUBTEST_END()

  SUBTEST_START(template_tripleWithRelation2)
  {
    ScTemplate templ;

    templ.TripleWithRelation(
          addr1 >> "1",
          ScType::EdgeAccessVarPosPerm >> "2",
          ScType::NodeVar >> "3",
          ScType::EdgeAccessVarPosPerm >> "4",
          addr3 >> "5");

    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
    SC_CHECK_GREAT(result.Size(), 0, ());

  }
  SUBTEST_END()

  SUBTEST_START(template_params_correct)
  {
    ScAddr const addrConst = ctx.CreateNode(*ScType::NodeConst);
    ScAddr const addrTest3 = ctx.CreateNode(*ScType::NodeConstTuple);
    ScAddr const addrTest6 = ctx.CreateNode(*ScType::NodeConstClass);

    ScTemplate templ;

    templ.Triple(
          addrConst >> "1",
          ScType::EdgeAccessVarPosPerm >> "_2",
          ScType::NodeVarTuple >> "_3");
    templ.Triple(
          "_3",
          ScType::EdgeAccessVarPosPerm >> "_5",
          ScType::NodeVarClass >> "_6");

    ScTemplateGenParams params;
    params.add("_3", addrTest3).add("_6", addrTest6);

    ScTemplateGenResult result;
    SC_CHECK(ctx.HelperGenTemplate(templ, result, params), ());

    ScTemplate searchTempl;
    searchTempl.Triple(
          addrConst >> "1",
          ScType::EdgeAccessVarPosPerm >> "_2",
          ScType::NodeVarTuple >> "_3");
    searchTempl.Triple(
          "_3",
          ScType::EdgeAccessVarPosPerm >> "_5",
          ScType::NodeVarClass >> "_6");

    ScTemplateSearchResult searchResult;
    SC_CHECK(ctx.HelperSearchTemplate(searchTempl, searchResult), ());
    SC_CHECK_EQUAL(searchResult.Size(), 1, ());
    SC_CHECK_EQUAL(searchResult[0]["_3"], addrTest3, ());
    SC_CHECK_EQUAL(searchResult[0]["_6"], addrTest6, ());
  }
  SUBTEST_END()

  SUBTEST_START(template_params_invalid)
  {
    ScAddr const addrConst = ctx.CreateNode(*ScType::NodeConst);
    ScAddr const addrTest3 = ctx.CreateNode(*ScType::NodeConstTuple);
    ScAddr const addrEdge2 = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, addrConst, addrTest3);

    ScTemplate templ;
    templ
        (addrConst >> "1",
         ScType::EdgeAccessVarPosPerm >> "_2", // can't be replaced by param in template generation
         ScType::NodeVar >> "_3");	// can't be replaced by param in template generation

    ScTemplateGenResult result;
    SC_CHECK(ctx.HelperGenTemplate(templ, result), ());

    // test edge
    {
      ScTemplateGenParams params;
      params.add("_2", addrEdge2);

      SC_CHECK(!ctx.HelperGenTemplate(templ, result, params), ());
    }

    // test invalid params
    {
      // target is const
      {
        SC_CHECK_EQUAL(false, TestTemplParams(ctx)(
                         addrConst >> "1",
                         ScType::EdgeAccessVarPosPerm >> "_2",
                         ScType::NodeConst >> "_3"), ());
      }

      // source is const
      {
        SC_CHECK_EQUAL(false, TestTemplParams(ctx)(
                         ScType::NodeConst >> "_1",
                         ScType::EdgeAccessVarPosPerm >> "_2",
                         addrConst >> "3"), ());
      }

      // edge is const
      {
        SC_CHECK_EQUAL(false, TestTemplParams(ctx)(
                         ScType::NodeVar >> "_1",
                         ScType::EdgeAccessConstPosPerm >> "_2",
                         addrConst >> "3"), ());
      }

    }
  }
  SUBTEST_END()

  SUBTEST_START(template_a_a_a)
  {
    /*
    _struct
    <- sc_node_struct;
    _-> rrel_location:: _apiai_location;
    _=> nrel_translation:: _apiai_speech
    (* _<- _lang;; *);;
    */

    const ScAddr _structAddr = ctx.CreateNode(ScType::NodeVarStruct);
    SC_CHECK(_structAddr.IsValid(), ());

    const ScAddr _apiai_locationAddr = ctx.CreateNode(ScType::NodeVar);
    SC_CHECK(_apiai_locationAddr.IsValid(), ());

    const ScAddr _apiai_speechAddr = ctx.CreateNode(ScType::NodeVar);
    SC_CHECK(_apiai_speechAddr.IsValid(), ());

    const ScAddr _langAddr = ctx.CreateNode(ScType::NodeVar);
    SC_CHECK(_langAddr.IsValid(), ());

    const ScAddr rrel_locationAddr = ctx.CreateNode(ScType::NodeConst);
    SC_CHECK(rrel_locationAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("rrel_location", rrel_locationAddr), ());

    const ScAddr nrel_translationAddr = ctx.CreateNode(ScType::NodeConst);
    SC_CHECK(nrel_translationAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("nrel_translation", nrel_translationAddr), ());

    const ScAddr _struct_locationEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _structAddr, _apiai_locationAddr);
    SC_CHECK(_struct_locationEdgeAddr.IsValid(), ());

    const ScAddr _rrel_locationEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, rrel_locationAddr, _struct_locationEdgeAddr);
    SC_CHECK(_rrel_locationEdgeAddr.IsValid(), ());

    const ScAddr _struct_speechEdgeAddr = ctx.CreateEdge(ScType::EdgeDCommonVar, _structAddr, _apiai_speechAddr);
    SC_CHECK(_struct_speechEdgeAddr.IsValid(), ());

    const ScAddr _nrel_translationEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, nrel_translationAddr, _struct_speechEdgeAddr);
    SC_CHECK(_nrel_translationEdgeAddr.IsValid(), ());

    const ScAddr _langEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _langAddr, _apiai_speechAddr);
    SC_CHECK(_langEdgeAddr.IsValid(), ());

    // create template
    const ScAddr templStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
    SC_CHECK(templStructAddr.IsValid(), ());
    ScStruct templStruct(&ctx, templStructAddr);

    templStruct
        << _structAddr
        << _apiai_locationAddr
        << _apiai_speechAddr
        << _langAddr
        << rrel_locationAddr
        << nrel_translationAddr
        << _struct_locationEdgeAddr
        << _rrel_locationEdgeAddr
        << _struct_speechEdgeAddr
        << _nrel_translationEdgeAddr
        << _langEdgeAddr;

    ScTemplate templ;
    SC_CHECK(ctx.HelperBuildTemplate(templ, templStructAddr), ());

    // check creation by this template
    {
      ScTemplateGenResult result;
      SC_CHECK(ctx.HelperGenTemplate(templ, result), ());
    }

    // check search by this template
    {
      ScTemplateSearchResult result;
      SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());
    }

  }
  SUBTEST_END()
}


UNIT_TEST(templates_2)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "templates_2");

  /*			_y
  *			^
  *			|
  *			| <----- _z
  *			|
  *			x <----- _s
  *
  * scs: x _-> _z:: _y;; _s _-> x;;
  */
  ScAddr templateAddr = ctx.CreateNode(sc_type_const | sc_type_node_struct);
  SC_CHECK(templateAddr.IsValid(), ());

  ScStruct templStruct(&ctx, templateAddr);
  ScAddr xAddr;
  {

    ScAddr _yAddr, _zAddr, _sAddr;

    xAddr = ctx.CreateNode(sc_type_const | sc_type_node_material);
    SC_CHECK(xAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("x", xAddr), ());

    _yAddr = ctx.CreateNode(sc_type_var);
    SC_CHECK(_yAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_y", _yAddr), ());

    _zAddr = ctx.CreateNode(sc_type_var | sc_type_node_role);
    SC_CHECK(_zAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_z", _zAddr), ());

    _sAddr = ctx.CreateNode(sc_type_node_class | sc_type_var);
    SC_CHECK(_sAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_s", _sAddr), ());

    ScAddr xyAddr = ctx.CreateEdge(sc_type_arc_access | sc_type_var, xAddr, _yAddr);
    SC_CHECK(xyAddr.IsValid(), ());
    ScAddr zxyAddr = ctx.CreateEdge(sc_type_arc_access | sc_type_var, _zAddr, xyAddr);
    SC_CHECK(zxyAddr.IsValid(), ());
    ScAddr sxAddr = ctx.CreateEdge(sc_type_arc_access | sc_type_var, _sAddr, xAddr);
    SC_CHECK(sxAddr.IsValid(), ());

    // append created elements into struct
    templStruct << xAddr << _yAddr << _zAddr << xyAddr << zxyAddr << _sAddr << sxAddr;
  }

  ScTemplate templ;
  SC_CHECK(ctx.HelperBuildTemplate(templ, templateAddr), ());

  // create test structure that correspond to template
  {
    ScAddr yAddr, zAddr, sAddr;

    yAddr = ctx.CreateNode(sc_type_const);
    SC_CHECK(yAddr.IsValid(), ());

    zAddr = ctx.CreateNode(sc_type_const | sc_type_node_role);
    SC_CHECK(zAddr.IsValid(), ());

    sAddr = ctx.CreateNode(sc_type_node_class | sc_type_const);
    SC_CHECK(sAddr.IsValid(), ());

    ScAddr xyAddr = ctx.CreateEdge(sc_type_arc_pos_const_perm, xAddr, yAddr);
    SC_CHECK(xyAddr.IsValid(), ());
    ScAddr zxyAddr = ctx.CreateEdge(sc_type_arc_pos_const_perm, zAddr, xyAddr);
    SC_CHECK(zxyAddr.IsValid(), ());
    ScAddr sxAddr = ctx.CreateEdge(sc_type_arc_pos_const_perm, sAddr, xAddr);
    SC_CHECK(sxAddr.IsValid(), ());


    // test search by template
    {
      ScTemplateSearchResult result;
      SC_CHECK(ctx.HelperSearchTemplate(templ, result), ());

      SC_CHECK_EQUAL(result.Size(), 1, ());
      ScTemplateSearchResultItem item = result[0];

      SC_CHECK_EQUAL(item["x"], xAddr, ());
      SC_CHECK_EQUAL(item["_y"], yAddr, ());
      SC_CHECK_EQUAL(item["_z"], zAddr, ());
      SC_CHECK_EQUAL(item["_s"], sAddr, ());
    }
  }
}

UNIT_TEST(template_search_in_struct)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_search_in_struct");

  /*
  *   _y
  *    ^
  *    | <--- _z
  *    x
  * scs: x _-> _z:: _y;;
  */

  ScAddr templateAddr = ctx.CreateNode(*ScType::NodeConstStruct);
  SC_CHECK(templateAddr.IsValid(), ());

  ScAddr xAddr, _yAddr, _zAddr, _xyEdgeAddr, _zxyEdgeAddr;
  ScStruct templStruct(&ctx, templateAddr);
  {
    xAddr = ctx.CreateNode(*ScType::NodeConst);
    SC_CHECK(xAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("x", xAddr), ());

    _yAddr = ctx.CreateNode(*ScType::NodeVar);
    SC_CHECK(_yAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_y", _yAddr), ());

    _zAddr = ctx.CreateNode(*ScType::NodeVar);
    SC_CHECK(_zAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_z", _zAddr), ());

    _xyEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessVarPosPerm, xAddr, _yAddr);
    SC_CHECK(_xyEdgeAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_xyEdge", _xyEdgeAddr), ());

    _zxyEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessVarPosPerm, _zAddr, _xyEdgeAddr);
    SC_CHECK(_zxyEdgeAddr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("_zxyEdge", _zxyEdgeAddr), ());

    templStruct << xAddr << _yAddr << _zAddr << _xyEdgeAddr << _zxyEdgeAddr;
  }

  ScTemplate templ;
  SC_CHECK(ctx.HelperBuildTemplate(templ, templateAddr), ());

  // create text struct
  ScAddr testStructAddr = ctx.CreateNode(*ScType::NodeConstStruct);
  SC_CHECK(testStructAddr.IsValid(), ());

  /*   y ---> u
  *   ^
  *   | <--- z
  *   x
  *   | <--- s
  *   v
  *   g
  * scs: x -> z: y; s: g;; y -> u;;
  */
  ScAddr tyAddr, txAddr, tgAddr, tuAddr, tzAddr, tsAddr,
      tyuEdgeAddr, txyEdgeAddr, txgEdgeAddr, tzxyEdgeAddr, tsxgEdgeAddr;

  ScStruct testStruct(&ctx, testStructAddr);
  {
    txAddr = xAddr;

    tyAddr = ctx.CreateNode(*ScType::NodeConst);
    SC_CHECK(tyAddr.IsValid(), ());

    tgAddr = ctx.CreateNode(*ScType::NodeConst);
    SC_CHECK(tgAddr.IsValid(), ());

    tuAddr = ctx.CreateNode(*ScType::NodeConst);
    SC_CHECK(tuAddr.IsValid(), ());

    tzAddr = ctx.CreateNode(*ScType::NodeConst);
    SC_CHECK(tzAddr.IsValid(), ());

    tsAddr = ctx.CreateNode(*ScType::NodeConst);
    SC_CHECK(tsAddr.IsValid(), ());

    tyuEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, tyAddr, tuAddr);
    SC_CHECK(tyuEdgeAddr.IsValid(), ());

    txyEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
    SC_CHECK(txyEdgeAddr.IsValid(), ());

    txgEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, txAddr, tgAddr);
    SC_CHECK(txgEdgeAddr.IsValid(), ());

    tzxyEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, tzAddr, txyEdgeAddr);
    SC_CHECK(tzxyEdgeAddr.IsValid(), ());

    tsxgEdgeAddr = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, tsAddr, txgEdgeAddr);
    SC_CHECK(tsxgEdgeAddr.IsValid(), ());

    testStruct << tyAddr << txAddr << tgAddr
               << tuAddr << tzAddr << tsAddr
               << tyuEdgeAddr << txyEdgeAddr
               << txgEdgeAddr << tzxyEdgeAddr
               << tsxgEdgeAddr;
  }

  // add extra edges that not included into struct
  // scs: x -> t: y;;
  ScAddr edge1 = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
  SC_CHECK(edge1.IsValid(), ());
  ScAddr edge2 = ctx.CreateEdge(*ScType::EdgeAccessConstPosPerm, tzAddr, edge1);
  SC_CHECK(edge2.IsValid(), ());

  {
    ScTemplateSearchResult result;
    SC_CHECK(ctx.HelperSearchTemplateInStruct(templ, *testStruct, result), ());

    SC_CHECK_EQUAL(result.Size(), 2, ());
    for (uint32_t i = 0; i < result.Size(); ++i)
    {
      ScTemplateSearchResultItem res1 = result[i];
      SC_CHECK_EQUAL(res1["x"], xAddr, ());
      SC_CHECK(res1["_y"] == tyAddr || res1["_y"] == tgAddr, ());
      SC_CHECK(res1["_z"] == tzAddr || res1["_z"] == tsAddr, ());
      SC_CHECK(res1["_xyEdge"] == txyEdgeAddr || res1["_xyEdge"] == txgEdgeAddr, ());
      SC_CHECK(res1["_zxyEdge"] == tsxgEdgeAddr || res1["_zxyEdge"] == tzxyEdgeAddr, ());
    }

  }
}

UNIT_TEST(template_performance)
{
  ScAddr node1, node2, node3, node4, edge1, edge2, edge3;
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_performance");

  {
    // preapre test
    ScTemplate templ;
    templ.Triple(
          ScType::NodeVar >> "Node1",
          ScType::EdgeAccessVarPosPerm >> "Edge1",
          ScType::NodeVar >> "Node2");
    templ.Triple(
          ScType::NodeVar >> "Node3",
          ScType::EdgeAccessVarPosPerm >> "Edge2",
          "Edge1");
    templ.Triple(
          ScType::NodeVar >> "Node4",
          ScType::EdgeAccessVarPosPerm >> "Edge3",
          "Node1");

    ScTemplateGenResult result;
    if (ctx.HelperGenTemplate(templ, result))
    {
      node1 = result["Node1"];
      node2 = result["Node2"];
      node3 = result["Node3"];
      node4 = result["Node4"];
      edge1 = result["Edge1"];
      edge2 = result["Edge2"];
      edge3 = result["Edge3"];
    }
    else
    {
      SC_CHECK(false, ("Can't create template"));
    }
  }

  double sum_time = 0;
  static size_t const iterCount = 1000;
  SC_LOG_INFO("Search (template)");

  for (size_t i = 0; i < iterCount; ++i)
  {
    g_test_timer_start();

    ScTemplate templ;
    templ.Triple(
          node1,
          ScType::EdgeAccessVarPosPerm >> "Edge1",
          ScType::NodeVar);

    templ.Triple(
          node3,
          ScType::EdgeAccessVarPosPerm,
          "Edge1");

    templ.Triple(
          node4,
          ScType::EdgeAccessVarPosPerm,
          node1);

    ScTemplateSearchResult result;
    bool const res = ctx.HelperSearchTemplate(templ, result);

    sum_time += g_test_timer_elapsed();
    SC_CHECK(res, ());
  }

  SC_LOG_INFO("Time: " << sum_time);
  SC_LOG_INFO("Time per search: " << (sum_time / iterCount));
  SC_LOG_INFO("Search per second: " << (iterCount / sum_time) << " search/sec \n");
}

UNIT_TEST(template_double_attrs)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_double_attrs");
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    SC_CHECK(addr1.IsValid(), ());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    SC_CHECK(addr2.IsValid(), ());
    ScAddr const addr3 = ctx.CreateNode(ScType::NodeConstRole);
    SC_CHECK(addr3.IsValid(), ());
    ScAddr const addr4 = ctx.CreateNode(ScType::NodeConstRole);
    SC_CHECK(addr4.IsValid(), ());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    SC_CHECK(edge1.IsValid(), ());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr3, edge1);
    SC_CHECK(edge2.IsValid(), ());
    ScAddr const edge3 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr4, edge1);
    SC_CHECK(edge3.IsValid(), ());

    auto const testOrder = [&ctx](std::vector<ScAddr> const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      SC_CHECK(ctx.HelperBuildTemplate(templ, structAddr), ());
    };

    SUBTEST_START(forwadr_order)
    {
      testOrder({ addr1, addr2, addr3, addr4, edge1, edge2, edge3 });
    }
    SUBTEST_END()

    SUBTEST_START(backward_order)
    {
      testOrder({ edge3, edge2, edge1, addr4, addr3, addr2, addr1 });
    }
    SUBTEST_END()

    SUBTEST_START(test_order)
    {
      testOrder({ addr1, addr2, addr3, addr4, edge2, edge1, edge3 });
    }
    SUBTEST_END()
  }
}

UNIT_TEST(template_edge_from_edge)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_edge_from_edge");
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    SC_CHECK(addr1.IsValid(), ());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    SC_CHECK(addr2.IsValid(), ());
    ScAddr const addr3 = ctx.CreateNode(ScType::NodeConstRole);
    SC_CHECK(addr3.IsValid(), ());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    SC_CHECK(edge1.IsValid(), ());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, edge1, addr3);
    SC_CHECK(edge2.IsValid(), ());

    auto const testOrder = [&ctx](std::vector<ScAddr> const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      SC_CHECK(ctx.HelperBuildTemplate(templ, structAddr), ());
    };

    SUBTEST_START(forwadr_order)
    {
      testOrder({ addr1, addr2, addr3, edge1, edge2 });
    }
    SUBTEST_END()

    SUBTEST_START(backward_order)
    {
      testOrder({ edge2, edge1, addr3, addr2, addr1 });
    }
    SUBTEST_END()

    SUBTEST_START(test_order)
    {
      testOrder({ addr1, addr2, addr3, edge2, edge1});
    }
    SUBTEST_END()
  }
}

// https://github.com/ostis-dev/sc-machine/issues/224
UNIT_TEST(template_issue_224)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_issue_224");

  /// TODO: replace with scs string in future
  // create template in memory
  {
    std::vector<ScAddr> contourItems;
    {
      auto testCreateNode = [&ctx, &contourItems](ScType const & type)
      {
        ScAddr const addr = ctx.CreateNode(type);
        SC_CHECK(addr.IsValid(), ());
        contourItems.push_back(addr);
        return addr;
      };

      auto testCreateNodeIdtf = [&ctx, &contourItems](ScType const & type, std::string const & idtf)
      {
        ScAddr const addr = ResolveKeynode(ctx, type, idtf);
        SC_CHECK(addr.IsValid(), ());
        contourItems.push_back(addr);
        return addr;
      };

      auto testCreateEdge = [&ctx, &contourItems](ScType const & type, ScAddr const & src, ScAddr const & trg)
      {
        ScAddr const edge = ctx.CreateEdge(type, src, trg);
        SC_CHECK(edge.IsValid(), ());
        contourItems.push_back(edge);
        return edge;
      };

      auto testCreateEdgeAttrs = [&ctx, &testCreateEdge](ScType const & type,
          ScAddr const & src, ScAddr const & trg,
          ScType const & attrsEdgeType, std::vector<ScAddr> const & attrNodes)
      {
        ScAddr const edge = testCreateEdge(type, src, trg);
        for (ScAddr const & addr : attrNodes)
          testCreateEdge(attrsEdgeType, addr, edge);
      };

      // equal scs:
      /*
                scp_process _-> ..process_instance;;
                ..process_instance _-> rrel_1:: rrel_in:: _set1;;
                ..process_instance _-> rrel_1:: rrel_in:: _element1;;
                ..process_instance _<= nrel_decomposition_of_action:: ..proc_decomp_instance;;
                ..proc_decomp_instance _-> rrel_1:: _operator1;;
                ..proc_decomp_instance _-> _operator2;;
                ..proc_decomp_instance _-> _operator3;;
                _operator1 _<- searchElStr3;;
                _operator1 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
                _operator1 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
                _operator1 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
                _operator1 _=> nrel_then:: _operator3;;
                _operator1 _=> nrel_else:: _operator2;;
                _operator2 _<- genElStr3;;
                _operator2 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
                _operator2 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
                _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
                _operator2 _=> nrel_goto:: _operator3;;
                _operator3 _<- return;;
             */

      // create known nodes
      ScAddr const _set1Addr = testCreateNodeIdtf(ScType::NodeVar, "_set1");
      ScAddr const _element1Addr = testCreateNodeIdtf(ScType::NodeVar, "_element1");
      ScAddr const _arc1Addr = testCreateNodeIdtf(ScType::NodeVar, "_arc1");
      ScAddr const _operator1Addr = testCreateNodeIdtf(ScType::NodeVar, "_operator1");
      ScAddr const _operator2Addr = testCreateNodeIdtf(ScType::NodeVar, "_operator2");
      ScAddr const _operator3Addr = testCreateNodeIdtf(ScType::NodeVar, "_operator3");

      ScAddr const scpProcessAddr = testCreateNodeIdtf(ScType::NodeConst, "scp_process");
      ScAddr const rrel_1Addr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_1");
      ScAddr const rrel_2Addr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_2");
      ScAddr const rrel_3Addr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_3");
      ScAddr const rrel_inAddr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_in");
      ScAddr const rrel_arc_const_pos_perm = testCreateNodeIdtf(ScType::NodeVarRole, "rrel_arc_const_pos_perm");
      ScAddr const rrel_fixedAddr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_fixed");
      ScAddr const rrel_assignAddr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_assign");
      ScAddr const rrel_scp_varAddr = testCreateNodeIdtf(ScType::NodeConstRole, "rrel_scp_var");
      ScAddr const searchElStr3Addr = testCreateNodeIdtf(ScType::NodeConstClass, "searchElStr3");
      ScAddr const genElStr3Addr = testCreateNodeIdtf(ScType::NodeConstClass, "geElStr3");
      ScAddr const returnAddr = testCreateNodeIdtf(ScType::NodeConstClass, "return");
      ScAddr const nrel_decompoisition_of_actionAddr = testCreateNodeIdtf(ScType::NodeVarNoRole, "nrel_decomposition_of_action");
      ScAddr const nrel_thenAddr = testCreateNodeIdtf(ScType::NodeConstNoRole, "nrel_then");
      ScAddr const nrel_elseAddr = testCreateNodeIdtf(ScType::NodeConstNoRole, "nrel_else");
      ScAddr const nrel_gotoAddr = testCreateNodeIdtf(ScType::NodeConstNoRole, "nrel_goto");

      {
        // scp_process _-> ..process_instance;;
        ScAddr const __procInstanceAddr = testCreateNode(ScType::NodeVar);
        testCreateEdge(
              ScType::EdgeAccessVarPosPerm,
              scpProcessAddr,
              __procInstanceAddr);

        // ..process_instance _-> rrel_1:: rrel_in:: _set1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              __procInstanceAddr,
              _set1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_1Addr, rrel_inAddr });

        // ..process_instance _-> rrel_1:: rrel_in::_element1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              __procInstanceAddr,
              _element1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_1Addr, rrel_inAddr });

        // ..process_instance _<= nrel_decomposition_of_action:: ..proc_decomp_instance;;
        ScAddr const __procDecompInstanceAddr = testCreateNode(ScType::NodeVar);
        testCreateEdgeAttrs(
              ScType::EdgeDCommonVar,
              __procDecompInstanceAddr,
              __procInstanceAddr,
              ScType::EdgeAccessVarPosPerm,
              { nrel_decompoisition_of_actionAddr });

        // ..proc_decomp_instance _-> rrel_1:: _operator1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              __procDecompInstanceAddr,
              _operator1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_1Addr });

        // ..proc_decomp_instance _->_operator2;;
        testCreateEdge(
              ScType::EdgeAccessVarPosPerm,
              __procDecompInstanceAddr,
              _operator2Addr);

        // ..proc_decomp_instance _-> _operator3;;
        testCreateEdge(
              ScType::EdgeAccessVarPosPerm,
              __procDecompInstanceAddr,
              _operator3Addr);

        // _operator1 _<- searchElStr3;;
        testCreateEdge(
              ScType::EdgeAccessVarPosPerm,
              searchElStr3Addr,
              _operator1Addr);

        // _operator1 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator1Addr,
              _set1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_1Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator1 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator1Addr,
              _arc1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_assignAddr, rrel_arc_const_pos_perm, rrel_scp_varAddr });

        // _operator1 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator1Addr,
              _element1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator1 _=> nrel_then:: _operator3;;
        testCreateEdgeAttrs(
              ScType::EdgeDCommonVar,
              _operator1Addr,
              _operator3Addr,
              ScType::EdgeAccessVarPosPerm,
              { nrel_thenAddr });

        // _operator1 _=> nrel_else:: _operator2;;
        testCreateEdgeAttrs(
              ScType::EdgeDCommonVar,
              _operator1Addr,
              _operator2Addr,
              ScType::EdgeAccessVarPosPerm,
              { nrel_elseAddr });

        // _operator2 _<- genElStr3;;
        testCreateEdge(
              ScType::EdgeAccessVarPosPerm,
              genElStr3Addr,
              _operator2Addr);

        // _operator2 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator2Addr,
              _set1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_1Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator2 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator2Addr,
              _arc1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_2Addr, rrel_assignAddr, rrel_arc_const_pos_perm, rrel_scp_varAddr });

        // _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator2Addr,
              _element1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testCreateEdgeAttrs(
              ScType::EdgeAccessVarPosPerm,
              _operator2Addr,
              _element1Addr,
              ScType::EdgeAccessVarPosPerm,
              { rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator2 _=> nrel_goto:: _operator3;;
        testCreateEdgeAttrs(
              ScType::EdgeDCommonVar,
              _operator2Addr,
              _operator3Addr,
              ScType::EdgeAccessVarPosPerm,
              { nrel_gotoAddr });

        // _operator3 _<- return;;
        testCreateEdge(
              ScType::EdgeAccessVarPosPerm,
              returnAddr,
              _operator3Addr);
      }
    }

    // check helperBuildTemplate by random shuffles of contour items (to test order they returned by iterators)
    {
      auto shuffle = [&contourItems](int iterationsNum)
      {
        std::random_device rand_dev;
        std::mt19937 mt_rand(rand_dev());
        std::uniform_int_distribution<int> unif(0, static_cast<int>(contourItems.size() - 1));

        for (int i = 0; i < iterationsNum; ++i)
        {
          for (size_t j = 0; j < contourItems.size(); ++j)
          {
            size_t const newIndex = unif(mt_rand);
            std::swap(contourItems[j], contourItems[newIndex]);
          }
        }
      };

      static size_t const testNum = 300;
      utils::ScProgress progress("Random order", testNum);
      for (size_t i = 0; i < testNum; ++i)
      {
        shuffle(1);
        ScAddr const structAddr = ResolveKeynode(ctx, ScType::NodeConstStruct, "test_program" + std::to_string(i));
        ScStruct contour(&ctx, structAddr);

        for (auto const & a : contourItems)
          contour << a;

        ScTemplate templ;
        SC_CHECK(ctx.HelperBuildTemplate(templ, structAddr), ());

        progress.PrintStatus(i);
      }
    }
  }
}
