/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_struct.hpp"
#include "sc-memory/cpp/utils/sc_progress.hpp"

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
      SC_CHECK(mCtx.helperGenTemplate(testTempl, res), ());
    }
    catch (ScExceptionInvalidParams & e)
    {
      (void)e;
      catched = true;
    }

    return !catched;
  }

private:
  ScMemoryContext & mCtx;
};


bool hasAddr(tAddrVector const & v, ScAddr const & addr)
{
  for (tAddrVector::const_iterator it = v.begin(); it != v.end(); ++it)
  {
    if (*it == addr)
      return true;
  }

  return false;
}

ScAddr resolveKeynode(ScMemoryContext & ctx, ScType const & type, std::string const & idtf)
{
  ScAddr addr;
  ctx.helperResolveSystemIdtf(idtf, addr, true);
  SC_CHECK(addr.isValid(), ());
  return addr;
}

} // namespace


UNIT_TEST(templates_common)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "templates_common");
  ScAddr const addr1 = ctx.createNode(ScType::NODE_CONST);
  SC_CHECK(addr1.isValid(), ());

  ScAddr const addr2 = ctx.createNode(ScType::NODE_CONST);
  SC_CHECK(addr2.isValid(), ());

  ScAddr const addr3 = ctx.createNode(ScType::NODE_CONST);
  SC_CHECK(addr3.isValid(), ());

  ScAddr const edge1 = ctx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, addr1, addr2);
  SC_CHECK(edge1.isValid(), ());

  ScAddr const edge2 = ctx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, addr3, edge1);
  SC_CHECK(edge2.isValid(), ());

  {
    ScTemplate templ;

    templ.triple(
          addr1 >> "addr1",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "edge1",
          ScType::NODE_VAR >> "addr2");
    templ.triple(
          ScType::NODE_VAR >> "_addr1T2",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "_addr2T2",
          "edge1");
    templ.triple(
          "addr2",
          ScType::EDGE_DCOMMON_VAR >> "_addr2T3",
          "edge1");

    ScTemplateGenResult result;
    SC_CHECK(ctx.helperGenTemplate(templ, result), ());

    ScIterator5Ptr it5 = ctx.iterator5(addr1, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);
    SC_CHECK(it5->next(), ());
    SC_CHECK_EQUAL(it5->value(0), result["addr1"], ());
    SC_CHECK_EQUAL(it5->value(1), result["edge1"], ());
    SC_CHECK_EQUAL(it5->value(2), result["addr2"], ());
    SC_CHECK_EQUAL(it5->value(3), result["_addr2T2"], ());
    SC_CHECK_EQUAL(it5->value(4), result["_addr1T2"], ());

    ScIterator3Ptr it3 = ctx.iterator3(result["addr2"], sc_type_arc_common, sc_type_arc_pos_const_perm);
    SC_CHECK(it3->next(), ());
    SC_CHECK_EQUAL(it3->value(0), result["addr2"], ());
    SC_CHECK_EQUAL(it3->value(1), result["_addr2T3"], ());
    SC_CHECK_EQUAL(it3->value(2), result["edge1"], ());


    SUBTEST_START(template_search)
    {
      ScTemplateSearchResult searchResult;
      SC_CHECK(ctx.helperSearchTemplate(templ, searchResult), ());

      SC_CHECK_EQUAL(searchResult.getSize(), 1, ());

      ScTemplateSearchResultItem res = searchResult[0];

      SC_CHECK_EQUAL(it5->value(0), res["addr1"], ());
      SC_CHECK_EQUAL(it5->value(1), res["edge1"], ());
      SC_CHECK_EQUAL(it5->value(2), res["addr2"], ());
      SC_CHECK_EQUAL(it5->value(3), result["_addr2T2"], ());
      SC_CHECK_EQUAL(it5->value(4), result["_addr1T2"], ());

      SC_CHECK_EQUAL(it3->value(0), res["addr2"], ());
      SC_CHECK_EQUAL(it3->value(1), result["_addr2T3"], ());
      SC_CHECK_EQUAL(it3->value(2), res["edge1"], ());
    }
    SUBTEST_END

    SUBTEST_START(template_search2)
    {
      size_t const testCount = 10;
      tAddrVector nodes, edges;

      ScAddr addrSrc = ctx.createNode(sc_type_const);
      SC_CHECK(addrSrc.isValid(), ());
      for (size_t i = 0; i < testCount; ++i)
      {
        ScAddr const addrTrg = ctx.createNode(sc_type_const);
        SC_CHECK(addrTrg.isValid(), ());

        ScAddr const addrEdge = ctx.createEdge(sc_type_arc_pos_const_perm, addrSrc, addrTrg);
        SC_CHECK(addrEdge.isValid(), ());

        nodes.push_back(addrTrg);
        edges.push_back(addrEdge);
      }

      ScTemplate templ2;

      templ2.triple(addrSrc >> "addrSrc",
                    ScType::EDGE_ACCESS_VAR_POS_PERM >> "edge",
                    ScType::NODE_VAR >> "addrTrg");

      ScTemplateSearchResult result2;
      SC_CHECK(ctx.helperSearchTemplate(templ2, result2), ());

      size_t const count = result2.getSize();
      for (size_t i = 0; i < count; ++i)
      {
        ScTemplateSearchResultItem r = result2[i];

        SC_CHECK_EQUAL(r["addrSrc"], addrSrc, ());

        SC_CHECK(hasAddr(edges, r["edge"]), ());
        SC_CHECK(hasAddr(nodes, r["addrTrg"]), ());
      }
    }
    SUBTEST_END
  }

  SUBTEST_START(template_tripleWithRelation)
  {
    ScTemplate templ;

    templ.tripleWithRelation(
          addr1,
          ScType::EDGE_ACCESS_VAR_POS_PERM,
          ScType::NODE_VAR,
          ScType::EDGE_ACCESS_VAR_POS_PERM,
          addr3);

    ScTemplateSearchResult result;
    SC_CHECK(ctx.helperSearchTemplate(templ, result), ());
    SC_CHECK_GREAT(result.getSize(), 0, ());
  }
  SUBTEST_END

  SUBTEST_START(template_tripleWithRelation2)
  {
    ScTemplate templ;

    templ.tripleWithRelation(
          addr1 >> "1",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "2",
          ScType::NODE_VAR >> "3",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "4",
          addr3 >> "5");

    ScTemplateSearchResult result;
    SC_CHECK(ctx.helperSearchTemplate(templ, result), ());
    SC_CHECK_GREAT(result.getSize(), 0, ());

  }
  SUBTEST_END

  SUBTEST_START(template_params_correct)
  {
    ScAddr const addrConst = ctx.createNode(*ScType::NODE_CONST);
    ScAddr const addrTest3 = ctx.createNode(*ScType::NODE_CONST_TUPLE);
    ScAddr const addrTest6 = ctx.createNode(*ScType::NODE_CONST_CLASS);

    ScTemplate templ;

    templ.triple(
          addrConst >> "1",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "_2",
          ScType::NODE_VAR_TUPLE >> "_3");
    templ.triple(
          "_3",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "_5",
          ScType::NODE_VAR_CLASS >> "_6");

    ScTemplateGenParams params;
    params.add("_3", addrTest3).add("_6", addrTest6);

    ScTemplateGenResult result;
    SC_CHECK(ctx.helperGenTemplate(templ, result, params), ());

    ScTemplate searchTempl;
    searchTempl.triple(
          addrConst >> "1",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "_2",
          ScType::NODE_VAR_TUPLE >> "_3");
    searchTempl.triple(
          "_3",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "_5",
          ScType::NODE_VAR_CLASS >> "_6");

    ScTemplateSearchResult searchResult;
    SC_CHECK(ctx.helperSearchTemplate(searchTempl, searchResult), ());
    SC_CHECK_EQUAL(searchResult.getSize(), 1, ());
    SC_CHECK_EQUAL(searchResult[0]["_3"], addrTest3, ());
    SC_CHECK_EQUAL(searchResult[0]["_6"], addrTest6, ());
  }
  SUBTEST_END

  SUBTEST_START(template_params_invalid)
  {
    ScAddr const addrConst = ctx.createNode(*ScType::NODE_CONST);
    ScAddr const addrTest3 = ctx.createNode(*ScType::NODE_CONST_TUPLE);
    ScAddr const addrEdge2 = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, addrConst, addrTest3);

    ScTemplate templ;
    templ
        (addrConst >> "1",
         ScType::EDGE_ACCESS_VAR_POS_PERM >> "_2", // can't be replaced by param in template generation
         ScType::NODE_VAR >> "_3");	// can't be replaced by param in template generation

    ScTemplateGenResult result;
    SC_CHECK(ctx.helperGenTemplate(templ, result), ());

    // test edge
    {
      ScTemplateGenParams params;
      params.add("_2", addrEdge2);

      SC_CHECK(!ctx.helperGenTemplate(templ, result, params), ());
    }

    // test invalid params
    {
      // target is const
      {
        SC_CHECK_EQUAL(false, TestTemplParams(ctx)(
                         addrConst >> "1",
                         ScType::EDGE_ACCESS_VAR_POS_PERM >> "_2",
                         ScType::NODE_CONST >> "_3"), ());
      }

      // source is const
      {
        SC_CHECK_EQUAL(false, TestTemplParams(ctx)(
                         ScType::NODE_CONST >> "_1",
                         ScType::EDGE_ACCESS_VAR_POS_PERM >> "_2",
                         addrConst >> "3"), ());
      }

      // edge is const
      {
        SC_CHECK_EQUAL(false, TestTemplParams(ctx)(
                         ScType::NODE_VAR >> "_1",
                         ScType::EDGE_ACCESS_CONST_POS_PERM >> "_2",
                         addrConst >> "3"), ());
      }

    }
  }
  SUBTEST_END

  SUBTEST_START(template_a_a_a)
  {
    /*
    _struct
    <- sc_node_struct;
    _-> rrel_location:: _apiai_location;
    _=> nrel_translation:: _apiai_speech
    (* _<- _lang;; *);;
    */

    const ScAddr _structAddr = ctx.createNode(ScType::NODE_VAR_STRUCT);
    SC_CHECK(_structAddr.isValid(), ());

    const ScAddr _apiai_locationAddr = ctx.createNode(ScType::NODE_VAR);
    SC_CHECK(_apiai_locationAddr.isValid(), ());

    const ScAddr _apiai_speechAddr = ctx.createNode(ScType::NODE_VAR);
    SC_CHECK(_apiai_speechAddr.isValid(), ());

    const ScAddr _langAddr = ctx.createNode(ScType::NODE_VAR);
    SC_CHECK(_langAddr.isValid(), ());

    const ScAddr rrel_locationAddr = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(rrel_locationAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("rrel_location", rrel_locationAddr), ());

    const ScAddr nrel_translationAddr = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(nrel_translationAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("nrel_translation", nrel_translationAddr), ());

    const ScAddr _struct_locationEdgeAddr = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, _structAddr, _apiai_locationAddr);
    SC_CHECK(_struct_locationEdgeAddr.isValid(), ());

    const ScAddr _rrel_locationEdgeAddr = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, rrel_locationAddr, _struct_locationEdgeAddr);
    SC_CHECK(_rrel_locationEdgeAddr.isValid(), ());

    const ScAddr _struct_speechEdgeAddr = ctx.createEdge(ScType::EDGE_DCOMMON_VAR, _structAddr, _apiai_speechAddr);
    SC_CHECK(_struct_speechEdgeAddr.isValid(), ());

    const ScAddr _nrel_translationEdgeAddr = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, nrel_translationAddr, _struct_speechEdgeAddr);
    SC_CHECK(_nrel_translationEdgeAddr.isValid(), ());

    const ScAddr _langEdgeAddr = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, _langAddr, _apiai_speechAddr);
    SC_CHECK(_langEdgeAddr.isValid(), ());

    // create template
    const ScAddr templStructAddr = ctx.createNode(ScType::NODE_CONST_STRUCT);
    SC_CHECK(templStructAddr.isValid(), ());
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
    SC_CHECK(ctx.helperBuildTemplate(templ, templStructAddr), ());

    // check creation by this template
    {
      ScTemplateGenResult result;
      SC_CHECK(ctx.helperGenTemplate(templ, result), ());
    }

    // check search by this template
    {
      ScTemplateSearchResult result;
      SC_CHECK(ctx.helperSearchTemplate(templ, result), ());
    }

  }
  SUBTEST_END
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
  ScAddr templateAddr = ctx.createNode(sc_type_const | sc_type_node_struct);
  SC_CHECK(templateAddr.isValid(), ());

  ScStruct templStruct(&ctx, templateAddr);
  ScAddr xAddr;
  {

    ScAddr _yAddr, _zAddr, _sAddr;

    xAddr = ctx.createNode(sc_type_const | sc_type_node_material);
    SC_CHECK(xAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("x", xAddr), ());

    _yAddr = ctx.createNode(sc_type_var);
    SC_CHECK(_yAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_y", _yAddr), ());

    _zAddr = ctx.createNode(sc_type_var | sc_type_node_role);
    SC_CHECK(_zAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_z", _zAddr), ());

    _sAddr = ctx.createNode(sc_type_node_class | sc_type_var);
    SC_CHECK(_sAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_s", _sAddr), ());

    ScAddr xyAddr = ctx.createEdge(sc_type_arc_access | sc_type_var, xAddr, _yAddr);
    SC_CHECK(xyAddr.isValid(), ());
    ScAddr zxyAddr = ctx.createEdge(sc_type_arc_access | sc_type_var, _zAddr, xyAddr);
    SC_CHECK(zxyAddr.isValid(), ());
    ScAddr sxAddr = ctx.createEdge(sc_type_arc_access | sc_type_var, _sAddr, xAddr);
    SC_CHECK(sxAddr.isValid(), ());

    // append created elements into struct
    templStruct << xAddr << _yAddr << _zAddr << xyAddr << zxyAddr << _sAddr << sxAddr;
  }

  ScTemplate templ;
  SC_CHECK(ctx.helperBuildTemplate(templ, templateAddr), ());

  // create test structure that correspond to template
  {
    ScAddr yAddr, zAddr, sAddr;

    yAddr = ctx.createNode(sc_type_const);
    SC_CHECK(yAddr.isValid(), ());

    zAddr = ctx.createNode(sc_type_const | sc_type_node_role);
    SC_CHECK(zAddr.isValid(), ());

    sAddr = ctx.createNode(sc_type_node_class | sc_type_const);
    SC_CHECK(sAddr.isValid(), ());

    ScAddr xyAddr = ctx.createEdge(sc_type_arc_pos_const_perm, xAddr, yAddr);
    SC_CHECK(xyAddr.isValid(), ());
    ScAddr zxyAddr = ctx.createEdge(sc_type_arc_pos_const_perm, zAddr, xyAddr);
    SC_CHECK(zxyAddr.isValid(), ());
    ScAddr sxAddr = ctx.createEdge(sc_type_arc_pos_const_perm, sAddr, xAddr);
    SC_CHECK(sxAddr.isValid(), ());


    // test search by template
    {
      ScTemplateSearchResult result;
      SC_CHECK(ctx.helperSearchTemplate(templ, result), ());

      SC_CHECK_EQUAL(result.getSize(), 1, ());
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

  ScAddr templateAddr = ctx.createNode(*ScType::NODE_CONST_STRUCT);
  SC_CHECK(templateAddr.isValid(), ());

  ScAddr xAddr, _yAddr, _zAddr, _xyEdgeAddr, _zxyEdgeAddr;
  ScStruct templStruct(&ctx, templateAddr);
  {
    xAddr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(xAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("x", xAddr), ());

    _yAddr = ctx.createNode(*ScType::NODE_VAR);
    SC_CHECK(_yAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_y", _yAddr), ());

    _zAddr = ctx.createNode(*ScType::NODE_VAR);
    SC_CHECK(_zAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_z", _zAddr), ());

    _xyEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_VAR_POS_PERM, xAddr, _yAddr);
    SC_CHECK(_xyEdgeAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_xyEdge", _xyEdgeAddr), ());

    _zxyEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_VAR_POS_PERM, _zAddr, _xyEdgeAddr);
    SC_CHECK(_zxyEdgeAddr.isValid(), ());
    SC_CHECK(ctx.helperSetSystemIdtf("_zxyEdge", _zxyEdgeAddr), ());

    templStruct << xAddr << _yAddr << _zAddr << _xyEdgeAddr << _zxyEdgeAddr;
  }

  ScTemplate templ;
  SC_CHECK(ctx.helperBuildTemplate(templ, templateAddr), ());

  // create text struct
  ScAddr testStructAddr = ctx.createNode(*ScType::NODE_CONST_STRUCT);
  SC_CHECK(testStructAddr.isValid(), ());

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

    tyAddr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(tyAddr.isValid(), ());

    tgAddr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(tgAddr.isValid(), ());

    tuAddr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(tuAddr.isValid(), ());

    tzAddr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(tzAddr.isValid(), ());

    tsAddr = ctx.createNode(*ScType::NODE_CONST);
    SC_CHECK(tsAddr.isValid(), ());

    tyuEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, tyAddr, tuAddr);
    SC_CHECK(tyuEdgeAddr.isValid(), ());

    txyEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, txAddr, tyAddr);
    SC_CHECK(txyEdgeAddr.isValid(), ());

    txgEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, txAddr, tgAddr);
    SC_CHECK(txgEdgeAddr.isValid(), ());

    tzxyEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, tzAddr, txyEdgeAddr);
    SC_CHECK(tzxyEdgeAddr.isValid(), ());

    tsxgEdgeAddr = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, tsAddr, txgEdgeAddr);
    SC_CHECK(tsxgEdgeAddr.isValid(), ());

    testStruct << tyAddr << txAddr << tgAddr
               << tuAddr << tzAddr << tsAddr
               << tyuEdgeAddr << txyEdgeAddr
               << txgEdgeAddr << tzxyEdgeAddr
               << tsxgEdgeAddr;
  }

  // add extra edges that not included into struct
  // scs: x -> t: y;;
  ScAddr edge1 = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, txAddr, tyAddr);
  SC_CHECK(edge1.isValid(), ());
  ScAddr edge2 = ctx.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, tzAddr, edge1);
  SC_CHECK(edge2.isValid(), ());

  {
    ScTemplateSearchResult result;
    SC_CHECK(ctx.helperSearchTemplateInStruct(templ, *testStruct, result), ());

    SC_CHECK_EQUAL(result.getSize(), 2, ());
    for (uint32_t i = 0; i < result.getSize(); ++i)
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
    templ.triple(
          ScType::NODE_VAR >> "Node1",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "Edge1",
          ScType::NODE_VAR >> "Node2");
    templ.triple(
          ScType::NODE_VAR >> "Node3",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "Edge2",
          "Edge1");
    templ.triple(
          ScType::NODE_VAR >> "Node4",
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "Edge3",
          "Node1");

    ScTemplateGenResult result;
    if (ctx.helperGenTemplate(templ, result))
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
    templ.triple(
          node1,
          ScType::EDGE_ACCESS_VAR_POS_PERM >> "Edge1",
          ScType::NODE_VAR);

    templ.triple(
          node3,
          ScType::EDGE_ACCESS_VAR_POS_PERM,
          "Edge1");

    templ.triple(
          node4,
          ScType::EDGE_ACCESS_VAR_POS_PERM,
          node1);

    ScTemplateSearchResult result;
    bool const res = ctx.helperSearchTemplate(templ, result);

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
    ScAddr const addr1 = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(addr1.isValid(), ());
    ScAddr const addr2 = ctx.createNode(ScType::NODE_VAR);
    SC_CHECK(addr2.isValid(), ());
    ScAddr const addr3 = ctx.createNode(ScType::NODE_CONST_ROLE);
    SC_CHECK(addr3.isValid(), ());
    ScAddr const addr4 = ctx.createNode(ScType::NODE_CONST_ROLE);
    SC_CHECK(addr4.isValid(), ());

    ScAddr const edge1 = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, addr1, addr2);
    SC_CHECK(edge1.isValid(), ());
    ScAddr const edge2 = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, addr3, edge1);
    SC_CHECK(edge2.isValid(), ());
    ScAddr const edge3 = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, addr4, edge1);
    SC_CHECK(edge3.isValid(), ());

    auto const testOrder = [&ctx](std::vector<ScAddr> const & addrs)
    {
      ScAddr const structAddr = ctx.createNode(ScType::NODE_CONST_STRUCT);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      SC_CHECK(ctx.helperBuildTemplate(templ, structAddr), ());
    };

    SUBTEST_START(forwadr_order)
    {
      testOrder({ addr1, addr2, addr3, addr4, edge1, edge2, edge3 });
    }
    SUBTEST_END

    SUBTEST_START(backward_order)
    {
      testOrder({ edge3, edge2, edge1, addr4, addr3, addr2, addr1 });
    }
    SUBTEST_END

    SUBTEST_START(test_order)
    {
      testOrder({ addr1, addr2, addr3, addr4, edge2, edge1, edge3 });
    }
    SUBTEST_END
  }
}

UNIT_TEST(template_edge_from_edge)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_edge_from_edge");
  {
    ScAddr const addr1 = ctx.createNode(ScType::NODE_CONST);
    SC_CHECK(addr1.isValid(), ());
    ScAddr const addr2 = ctx.createNode(ScType::NODE_VAR);
    SC_CHECK(addr2.isValid(), ());
    ScAddr const addr3 = ctx.createNode(ScType::NODE_CONST_ROLE);
    SC_CHECK(addr3.isValid(), ());

    ScAddr const edge1 = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, addr1, addr2);
    SC_CHECK(edge1.isValid(), ());
    ScAddr const edge2 = ctx.createEdge(ScType::EDGE_ACCESS_VAR_POS_PERM, edge1, addr3);
    SC_CHECK(edge2.isValid(), ());

    auto const testOrder = [&ctx](std::vector<ScAddr> const & addrs)
    {
      ScAddr const structAddr = ctx.createNode(ScType::NODE_CONST_STRUCT);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      SC_CHECK(ctx.helperBuildTemplate(templ, structAddr), ());
    };

    SUBTEST_START(forwadr_order)
    {
      testOrder({ addr1, addr2, addr3, edge1, edge2 });
    }
    SUBTEST_END

    SUBTEST_START(backward_order)
    {
      testOrder({ edge2, edge1, addr3, addr2, addr1 });
    }
    SUBTEST_END

    SUBTEST_START(test_order)
    {
      testOrder({ addr1, addr2, addr3, edge2, edge1});
    }
    SUBTEST_END
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
        ScAddr const addr = ctx.createNode(type);
        SC_CHECK(addr.isValid(), ());
        contourItems.push_back(addr);
        return addr;
      };

      auto testCreateNodeIdtf = [&ctx, &contourItems](ScType const & type, std::string const & idtf)
      {
        ScAddr const addr = resolveKeynode(ctx, type, idtf);
        SC_CHECK(addr.isValid(), ());
        contourItems.push_back(addr);
        return addr;
      };

      auto testCreateEdge = [&ctx, &contourItems](ScType const & type, ScAddr const & src, ScAddr const & trg)
      {
        ScAddr const edge = ctx.createEdge(type, src, trg);
        SC_CHECK(edge.isValid(), ());
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
      ScAddr const _set1Addr = testCreateNodeIdtf(ScType::NODE_VAR, "_set1");
      ScAddr const _element1Addr = testCreateNodeIdtf(ScType::NODE_VAR, "_element1");
      ScAddr const _arc1Addr = testCreateNodeIdtf(ScType::NODE_VAR, "_arc1");
      ScAddr const _operator1Addr = testCreateNodeIdtf(ScType::NODE_VAR, "_operator1");
      ScAddr const _operator2Addr = testCreateNodeIdtf(ScType::NODE_VAR, "_operator2");
      ScAddr const _operator3Addr = testCreateNodeIdtf(ScType::NODE_VAR, "_operator3");

      ScAddr const scpProcessAddr = testCreateNodeIdtf(ScType::NODE_CONST, "scp_process");
      ScAddr const rrel_1Addr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_1");
      ScAddr const rrel_2Addr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_2");
      ScAddr const rrel_3Addr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_3");
      ScAddr const rrel_inAddr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_in");
      ScAddr const rrel_arc_const_pos_perm = testCreateNodeIdtf(ScType::NODE_VAR_ROLE, "rrel_arc_const_pos_perm");
      ScAddr const rrel_fixedAddr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_fixed");
      ScAddr const rrel_assignAddr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_assign");
      ScAddr const rrel_scp_varAddr = testCreateNodeIdtf(ScType::NODE_CONST_ROLE, "rrel_scp_var");
      ScAddr const searchElStr3Addr = testCreateNodeIdtf(ScType::NODE_CONST_CLASS, "searchElStr3");
      ScAddr const genElStr3Addr = testCreateNodeIdtf(ScType::NODE_CONST_CLASS, "geElStr3");
      ScAddr const returnAddr = testCreateNodeIdtf(ScType::NODE_CONST_CLASS, "return");
      ScAddr const nrel_decompoisition_of_actionAddr = testCreateNodeIdtf(ScType::NODE_VAR_NOROLE, "nrel_decomposition_of_action");
      ScAddr const nrel_thenAddr = testCreateNodeIdtf(ScType::NODE_CONST_NOROLE, "nrel_then");
      ScAddr const nrel_elseAddr = testCreateNodeIdtf(ScType::NODE_CONST_NOROLE, "nrel_else");
      ScAddr const nrel_gotoAddr = testCreateNodeIdtf(ScType::NODE_CONST_NOROLE, "nrel_goto");

      {
        // scp_process _-> ..process_instance;;
        ScAddr const __procInstanceAddr = testCreateNode(ScType::NODE_VAR);
        testCreateEdge(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              scpProcessAddr,
              __procInstanceAddr);

        // ..process_instance _-> rrel_1:: rrel_in:: _set1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              __procInstanceAddr,
              _set1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_1Addr, rrel_inAddr });

        // ..process_instance _-> rrel_1:: rrel_in::_element1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              __procInstanceAddr,
              _element1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_1Addr, rrel_inAddr });

        // ..process_instance _<= nrel_decomposition_of_action:: ..proc_decomp_instance;;
        ScAddr const __procDecompInstanceAddr = testCreateNode(ScType::NODE_VAR);
        testCreateEdgeAttrs(
              ScType::EDGE_DCOMMON_VAR,
              __procDecompInstanceAddr,
              __procInstanceAddr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { nrel_decompoisition_of_actionAddr });

        // ..proc_decomp_instance _-> rrel_1:: _operator1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              __procDecompInstanceAddr,
              _operator1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_1Addr });

        // ..proc_decomp_instance _->_operator2;;
        testCreateEdge(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              __procDecompInstanceAddr,
              _operator2Addr);

        // ..proc_decomp_instance _-> _operator3;;
        testCreateEdge(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              __procDecompInstanceAddr,
              _operator3Addr);

        // _operator1 _<- searchElStr3;;
        testCreateEdge(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              searchElStr3Addr,
              _operator1Addr);

        // _operator1 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator1Addr,
              _set1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_1Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator1 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator1Addr,
              _arc1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_assignAddr, rrel_arc_const_pos_perm, rrel_scp_varAddr });

        // _operator1 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator1Addr,
              _element1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator1 _=> nrel_then:: _operator3;;
        testCreateEdgeAttrs(
              ScType::EDGE_DCOMMON_VAR,
              _operator1Addr,
              _operator3Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { nrel_thenAddr });

        // _operator1 _=> nrel_else:: _operator2;;
        testCreateEdgeAttrs(
              ScType::EDGE_DCOMMON_VAR,
              _operator1Addr,
              _operator2Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { nrel_elseAddr });

        // _operator2 _<- genElStr3;;
        testCreateEdge(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              genElStr3Addr,
              _operator2Addr);

        // _operator2 _-> rrel_1:: rrel_fixed:: rrel_scp_var:: _set1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator2Addr,
              _set1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_1Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator2 _-> rrel_2:: rrel_assign:: rrel_arc_const_pos_perm:: rrel_scp_var:: _arc1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator2Addr,
              _arc1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_2Addr, rrel_assignAddr, rrel_arc_const_pos_perm, rrel_scp_varAddr });

        // _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator2Addr,
              _element1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator2 _-> rrel_3:: rrel_fixed:: rrel_scp_var:: _element1;;
        testCreateEdgeAttrs(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              _operator2Addr,
              _element1Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { rrel_3Addr, rrel_fixedAddr, rrel_scp_varAddr });

        // _operator2 _=> nrel_goto:: _operator3;;
        testCreateEdgeAttrs(
              ScType::EDGE_DCOMMON_VAR,
              _operator2Addr,
              _operator3Addr,
              ScType::EDGE_ACCESS_VAR_POS_PERM,
              { nrel_gotoAddr });

        // _operator3 _<- return;;
        testCreateEdge(
              ScType::EDGE_ACCESS_VAR_POS_PERM,
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
        ScAddr const structAddr = resolveKeynode(ctx, ScType::NODE_CONST_STRUCT, "test_program" + std::to_string(i));
        ScStruct contour(&ctx, structAddr);

        for (auto const & a : contourItems)
          contour << a;

        ScTemplate templ;
        SC_CHECK(ctx.helperBuildTemplate(templ, structAddr), ());

        progress.PrintStatus(i);
      }
    }
  }
}
