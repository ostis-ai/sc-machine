/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <glib.h>
#include <random>
#include <algorithm>

#include "catch2/catch.hpp"
#include "sc-memory/sc_debug.hpp"
#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_template.hpp"
#include "sc-memory/sc_struct.hpp"
#include "sc-memory/utils/sc_progress.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "dummy_file_interface.hpp"
extern "C"
{
#include "sc-core/sc-store/sc-string-tree/sc_string_tree.h"
}

namespace
{

struct TestTemplParams
{
  explicit TestTemplParams(ScMemoryContext & ctx)
        : mCtx(ctx)
  {
  }

  bool operator()(ScTemplateItemValue param1, ScTemplateItemValue param2, ScTemplateItemValue param3)
  {
    bool catched = false;
    try
    {
      ScTemplate testTempl;
      testTempl(param1, param2, param3);

      ScTemplateGenResult res;
      REQUIRE(mCtx.HelperGenTemplate(testTempl, res));
    }
    catch (utils::ExceptionInvalidParams & e)
    {
      (void) e;
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
  ScAddr const addr = ctx.HelperResolveSystemIdtf(idtf, ScType::NodeConst);
  REQUIRE(addr.IsValid());
  return addr;
}

} // namespace

TEST_CASE("template_result", "[test templates]")
{
  {
    ScTemplate::Result result(true);

    REQUIRE(result);
    REQUIRE_THAT(result.Msg(), Catch::Equals(""));
  }

  {
    ScTemplate::Result result(false, "msg");
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.Msg(), Catch::Equals("msg"));
  }
}

TEST_CASE("templates_common", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "templates_common");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr1.IsValid());

    ScAddr const addr2 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr2.IsValid());

    ScAddr const addr3 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr3.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
    REQUIRE(edge1.IsValid());

    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr3, edge1);
    REQUIRE(edge2.IsValid());
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
      REQUIRE(ctx.HelperGenTemplate(templ, result));

      ScIterator5Ptr it5 = ctx.Iterator5(
            addr1,
            sc_type_arc_pos_const_perm,
            sc_type_node,
            sc_type_arc_pos_const_perm,
            sc_type_node);

      REQUIRE(it5->Next());
      REQUIRE(it5->Get(0) == result["addr1"]);
      REQUIRE(it5->Get(1) == result["edge1"]);
      REQUIRE(it5->Get(2) == result["addr2"]);
      REQUIRE(it5->Get(3) == result["_addr2T2"]);
      REQUIRE(it5->Get(4) == result["_addr1T2"]);

      ScIterator3Ptr it3 = ctx.Iterator3(result["addr2"], sc_type_arc_common, sc_type_arc_pos_const_perm);
      REQUIRE(it3->Next());
      REQUIRE(it3->Get(0) == result["addr2"]);
      REQUIRE(it3->Get(1) == result["_addr2T3"]);
      REQUIRE(it3->Get(2) == result["edge1"]);

      SECTION("template_search")
      {
        SUBTEST_START("template_search")
        {
          try
          {
            ScTemplateSearchResult searchResult;
            REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

            REQUIRE(searchResult.Size() == 1);

            ScTemplateSearchResultItem res = searchResult[0];

            REQUIRE(it5->Get(0) == res["addr1"]);
            REQUIRE(it5->Get(1) == res["edge1"]);
            REQUIRE(it5->Get(2) == res["addr2"]);
            REQUIRE(it5->Get(3) == result["_addr2T2"]);
            REQUIRE(it5->Get(4) == result["_addr1T2"]);

            REQUIRE(it3->Get(0) == res["addr2"]);
            REQUIRE(it3->Get(1) == result["_addr2T3"]);
            REQUIRE(it3->Get(2) == res["edge1"]);
          } catch (...)
          {
            SC_LOG_ERROR("Test \"template_search\" failed")
          }
        }
        SUBTEST_END()
      }

      SECTION("template_search2")
      {
        SUBTEST_START("template_search2")
        {
          try
          {
            size_t const testCount = 10;
            ScAddrVector nodes, edges;

            ScAddr addrSrc = ctx.CreateNode(ScType::NodeConst);
            REQUIRE(addrSrc.IsValid());
            for (size_t i = 0; i < testCount; ++i)
            {
              ScAddr const addrTrg = ctx.CreateNode(ScType::NodeConst);
              REQUIRE(addrTrg.IsValid());

              ScAddr const addrEdge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addrSrc, addrTrg);
              REQUIRE(addrEdge.IsValid());

              nodes.push_back(addrTrg);
              edges.push_back(addrEdge);
            }

            ScTemplate templ2;

            templ2.Triple(addrSrc >> "addrSrc",
                          ScType::EdgeAccessVarPosPerm >> "edge",
                          ScType::NodeVar >> "addrTrg");

            ScTemplateSearchResult result2;
            REQUIRE(ctx.HelperSearchTemplate(templ2, result2));

            size_t const count = result2.Size();
            for (size_t i = 0; i < count; ++i)
            {
              ScTemplateSearchResultItem r = result2[i];

              REQUIRE(r["addrSrc"] == addrSrc);

              REQUIRE(HasAddr(edges, r["edge"]));
              REQUIRE(HasAddr(nodes, r["addrTrg"]));
            }
          } catch (...)
          {
            SC_LOG_ERROR("Test \"template_search2\" failed")
          }
        }
        SUBTEST_END()
      }
    }

    SECTION("template_tripleWithRelation")
    {
      SUBTEST_START("template_tripleWithRelation")
      {
        try
        {
          ScTemplate templ;

          templ.TripleWithRelation(
                addr1,
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVar,
                ScType::EdgeAccessVarPosPerm,
                addr3);

          ScTemplateSearchResult result;
          REQUIRE(ctx.HelperSearchTemplate(templ, result));
          REQUIRE(result.Size() > 0);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_tripleWithRelation\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_tripleWithRelation2")
    {
      SUBTEST_START("template_tripleWithRelation2")
      {
        try
        {
          ScTemplate templ;

          templ.TripleWithRelation(
                addr1 >> "1",
                ScType::EdgeAccessVarPosPerm >> "2",
                ScType::NodeVar >> "3",
                ScType::EdgeAccessVarPosPerm >> "4",
                addr3 >> "5");

          ScTemplateSearchResult result;
          REQUIRE(ctx.HelperSearchTemplate(templ, result));
          REQUIRE(result.Size() > 0);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_tripleWithRelation2\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_params_correct")
    {
      SUBTEST_START("template_params_correct")
      {
        try
        {
          ScAddr const addrConst = ctx.CreateNode(ScType::NodeConst);
          ScAddr const addrTest3 = ctx.CreateNode(ScType::NodeConstTuple);
          ScAddr const addrTest6 = ctx.CreateNode(ScType::NodeConstClass);

          ScTemplate templ;

          templ.Triple(
                addrConst >> "1",
                ScType::EdgeAccessVarPosPerm >> "_2",
                ScType::NodeVarTuple >> "_3");
          templ.Triple(
                "_3",
                ScType::EdgeAccessVarPosPerm >> "_5",
                ScType::NodeVarClass >> "_6");

          ScTemplateParams params;
          params.Add("_3", addrTest3).Add("_6", addrTest6);

          ScTemplateGenResult result;
          REQUIRE(ctx.HelperGenTemplate(templ, result, params));

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
          REQUIRE(ctx.HelperSearchTemplate(searchTempl, searchResult));
          REQUIRE(searchResult.Size() == 1);
          REQUIRE(searchResult[0]["_3"] == addrTest3);
          REQUIRE(searchResult[0]["_6"] == addrTest6);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_params_correct\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_params_invalid")
    {
      SUBTEST_START("template_params_invalid")
      {
        try
        {
          ScAddr const addrConst = ctx.CreateNode(ScType::NodeConst);
          ScAddr const addrTest3 = ctx.CreateNode(ScType::NodeConstTuple);
          ScAddr const addrEdge2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addrConst, addrTest3);

          ScTemplate templ;
          templ
                (addrConst >> "1",
                 ScType::EdgeAccessVarPosPerm >> "_2", // can't be replaced by param in template generation
                 ScType::NodeVar >> "_3");  // can't be replaced by param in template generation

          ScTemplateGenResult result;
          REQUIRE(ctx.HelperGenTemplate(templ, result));

          // test edge
          {
            ScTemplateParams params;
            params.Add("_2", addrEdge2);

            REQUIRE(!ctx.HelperGenTemplate(templ, result, params));
          }

          // test invalid params
          {
            // target is const
            {
              REQUIRE(TestTemplParams(ctx)(
                    addrConst >> "1",
                    ScType::EdgeAccessVarPosPerm >> "_2",
                            ScType::NodeConst >> "_3") == false);
            }

            // source is const
            {
              REQUIRE(TestTemplParams(ctx)(
                    ScType::NodeConst >> "_1",
                    ScType::EdgeAccessVarPosPerm >> "_2",
                            addrConst >> "3") == false);
            }

            // edge is const
            {
              REQUIRE(TestTemplParams(ctx)(
                    ScType::NodeVar >> "_1",
                    ScType::EdgeAccessConstPosPerm >> "_2",
                            addrConst >> "3") == false);
            }

          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_params_invalid\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_a_a_a")
    {
      SUBTEST_START("template_a_a_a")
      {
        try
        {
          /*
          _struct
          _-> rrel_location:: _apiai_location;
          _=> nrel_translation:: _apiai_speech
          (* _<- _lang;; *);;
          */

          const ScAddr _structAddr = ctx.CreateNode(ScType::NodeVarStruct);
          REQUIRE(_structAddr.IsValid());
          REQUIRE(ctx.HelperSetSystemIdtf("_struct", _structAddr));

          const ScAddr _apiai_locationAddr = ctx.CreateNode(ScType::NodeVar);
          REQUIRE(_apiai_locationAddr.IsValid());
          REQUIRE(ctx.HelperSetSystemIdtf("_apiai_location", _apiai_locationAddr));

          const ScAddr _apiai_speechAddr = ctx.CreateNode(ScType::NodeVar);
          REQUIRE(_apiai_speechAddr.IsValid());
          REQUIRE(ctx.HelperSetSystemIdtf("_apiai_speech", _apiai_speechAddr));

          const ScAddr _langAddr = ctx.CreateNode(ScType::NodeVar);
          REQUIRE(_langAddr.IsValid());
          REQUIRE(ctx.HelperSetSystemIdtf("_lang", _langAddr));

          const ScAddr rrel_locationAddr = ctx.CreateNode(ScType::NodeConst);
          REQUIRE(rrel_locationAddr.IsValid());
          REQUIRE(ctx.HelperSetSystemIdtf("rrel_location", rrel_locationAddr));

          const ScAddr nrel_translationAddr = ctx.CreateNode(ScType::NodeConst);
          REQUIRE(nrel_translationAddr.IsValid());
          REQUIRE(ctx.HelperSetSystemIdtf("nrel_translation", nrel_translationAddr));

          const ScAddr _struct_locationEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _structAddr,
                                                                 _apiai_locationAddr);
          REQUIRE(_struct_locationEdgeAddr.IsValid());

          const ScAddr _rrel_locationEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, rrel_locationAddr,
                                                               _struct_locationEdgeAddr);
          REQUIRE(_rrel_locationEdgeAddr.IsValid());

          const ScAddr _struct_speechEdgeAddr = ctx.CreateEdge(ScType::EdgeDCommonVar, _structAddr, _apiai_speechAddr);
          REQUIRE(_struct_speechEdgeAddr.IsValid());

          const ScAddr _nrel_translationEdgeAddr = ctx.CreateEdge(
                ScType::EdgeAccessVarPosPerm,
                nrel_translationAddr,
                _struct_speechEdgeAddr);
          REQUIRE(_nrel_translationEdgeAddr.IsValid());

          const ScAddr _langEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _langAddr, _apiai_speechAddr);
          REQUIRE(_langEdgeAddr.IsValid());

          // create template
          const ScAddr templStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templStructAddr.IsValid());
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
          REQUIRE(ctx.HelperBuildTemplate(templ, templStructAddr));

          // check creation by this template
          {
            ScTemplateGenResult result;
            REQUIRE(ctx.HelperGenTemplate(templ, result));
          }

          // check search by this template
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(templ, result));
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_params_invalid\" failed")
        }
      }
      SUBTEST_END()
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"templates_common\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_search_with_params", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_search_with_params");

  try
  {
    /*
    * scs: concept_test
    *               -> rrel_test: x_elem(* => nrel_test: y_elem;; *);
    *               -> y_elem;;
    */
    ScAddr const concept_test = ctx.CreateNode(ScType::NodeConstClass);
    REQUIRE(concept_test.IsValid());

    ScAddr const x_elem = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(x_elem.IsValid());

    ScAddr const y_elem = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(y_elem.IsValid());

    ScAddr const rrel_test = ctx.CreateNode(ScType::NodeConstRole);
    REQUIRE(rrel_test.IsValid());

    ScAddr const nrel_test = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(nrel_test.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, concept_test, x_elem);
    REQUIRE(edge1.IsValid());

    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, rrel_test, edge1);
    REQUIRE(edge2.IsValid());

    ScAddr const edge3 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, concept_test, y_elem);
    REQUIRE(edge3.IsValid());

    ScAddr const edge4 = ctx.CreateEdge(ScType::EdgeDCommonConst, x_elem, y_elem);
    REQUIRE(edge4.IsValid());

    ScAddr const edge5 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrel_test, edge4);
    REQUIRE(edge5.IsValid());

    // Create template vars
    ScAddr const _x = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(_x.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("_x", _x));

    ScAddr const _y = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(_y.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("_y", _y));

    ScAddr const _concept = ctx.CreateNode(ScType::NodeVarClass);
    REQUIRE(_concept.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("_concept", _concept));

    ScAddr const _rrel = ctx.CreateNode(ScType::NodeVarRole);
    REQUIRE(_rrel.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("_rrel", _rrel));

    ScAddr const _nrel = ctx.CreateNode(ScType::NodeVarNoRole);
    REQUIRE(_nrel.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("_nrel", _nrel));

    SECTION("template_search_with_params_1")
    {
      SUBTEST_START("template_search_with_params_1")
      {
        try
        {
          /*
          * scs of template: concept_test _-> _x;
          */
          ScAddr const templateEdge = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, concept_test, _x);
          REQUIRE(templateEdge.IsValid());

          ScAddr const templateStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templateStructAddr.IsValid());
          ScStruct templateStruct(&ctx, templateStructAddr);
          templateStruct
                << concept_test
                << templateEdge
                << _x;

          ScTemplateParams templateParams;
          templateParams.Add("_x", x_elem);

          ScTemplate scTemplate;
          REQUIRE(ctx.HelperBuildTemplate(scTemplate, templateStructAddr, templateParams));
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(scTemplate, result));
            {
              REQUIRE(result.Size() == 1);
            }
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_search_with_params_1\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_search_with_params_2")
    {
      SUBTEST_START("template_search_with_params_2")
      {
        try
        {
          /*
          * scs of template: _concept _-> x_elem; _-> y_elem;;
          */
          ScAddr const templateEdge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _concept, x_elem);
          REQUIRE(templateEdge1.IsValid());

          ScAddr const templateEdge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _concept, y_elem);
          REQUIRE(templateEdge2.IsValid());

          ScAddr const templateStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templateStructAddr.IsValid());
          ScStruct templateStruct(&ctx, templateStructAddr);
          templateStruct
                << _concept
                << templateEdge1
                << x_elem
                << templateEdge2
                << y_elem;

          ScTemplateParams templateParams;
          templateParams.Add("_concept", concept_test);

          ScTemplate scTemplate;
          REQUIRE(ctx.HelperBuildTemplate(scTemplate, templateStructAddr, templateParams));
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(scTemplate, result));
            {
              REQUIRE(result.Size() == 1);
            }
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_search_with_params_2\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_search_with_params_3")
    {
      SUBTEST_START("template_search_with_params_3")
      {
        try
        {

          /*
          * scs of template: concept_test _-> rrel_test:: _x;
          */
          ScAddr const templateEdge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, concept_test, _x);
          REQUIRE(templateEdge1.IsValid());

          ScAddr const templateEdge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, rrel_test, templateEdge1);
          REQUIRE(templateEdge2.IsValid());

          ScAddr const templateStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templateStructAddr.IsValid());
          ScStruct templateStruct(&ctx, templateStructAddr);
          templateStruct
                << concept_test
                << templateEdge1
                << _x
                << templateEdge2
                << rrel_test;

          ScTemplateParams templateParams;
          templateParams.Add("_x", x_elem);

          ScTemplate scTemplate;
          REQUIRE(ctx.HelperBuildTemplate(scTemplate, templateStructAddr, templateParams));
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(scTemplate, result));
            {
              REQUIRE(result.Size() == 1);
            }
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_search_with_params_3\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_search_with_params_4")
    {
      SUBTEST_START("template_search_with_params_4")
      {
        try
        {
          /*
          * scs of template: concept_test _-> _rrel:: _x;
          */
          ScAddr const templateEdge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, concept_test, _x);
          REQUIRE(templateEdge1.IsValid());

          ScAddr const templateEdge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _rrel, templateEdge1);
          REQUIRE(templateEdge2.IsValid());

          ScAddr const templateStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templateStructAddr.IsValid());
          ScStruct templateStruct(&ctx, templateStructAddr);
          templateStruct
                << concept_test
                << templateEdge1
                << _x
                << templateEdge2
                << _rrel;

          ScTemplateParams templateParams;
          templateParams.Add("_x", x_elem);
          templateParams.Add("_rrel", rrel_test);

          ScTemplate scTemplate;
          REQUIRE(ctx.HelperBuildTemplate(scTemplate, templateStructAddr, templateParams));
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(scTemplate, result));
            {
              REQUIRE(result.Size() == 1);
            }
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_search_with_params_4\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_search_with_params_5")
    {
      SUBTEST_START("template_search_with_params_5")
      {
        try
        {
          /*
          * scs of template: _x _=> nrel_test:: _y;
          */
          ScAddr const templateEdge1 = ctx.CreateEdge(ScType::EdgeDCommonVar, _x, _y);
          REQUIRE(templateEdge1.IsValid());

          ScAddr const templateEdge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, nrel_test, templateEdge1);
          REQUIRE(templateEdge2.IsValid());

          ScAddr const templateStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templateStructAddr.IsValid());
          ScStruct templateStruct(&ctx, templateStructAddr);
          templateStruct
                << _x
                << templateEdge1
                << _y
                << templateEdge2
                << nrel_test;

          ScTemplateParams templateParams;
          templateParams.Add("_x", x_elem);
          templateParams.Add("_y", y_elem);

          ScTemplate scTemplate;
          REQUIRE(ctx.HelperBuildTemplate(scTemplate, templateStructAddr, templateParams));
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(scTemplate, result));
            {
              REQUIRE(result.Size() == 1);
            }
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_search_with_params_5\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("template_search_with_params_6")
    {
      SUBTEST_START("template_search_with_params_6")
      {
        try
        {
          /*
          * scs of template: x_elem _=> _nrel:: _y;
          */
          ScAddr const templateEdge1 = ctx.CreateEdge(ScType::EdgeDCommonVar, x_elem, _y);
          REQUIRE(templateEdge1.IsValid());

          ScAddr const templateEdge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _nrel, templateEdge1);
          REQUIRE(templateEdge2.IsValid());

          ScAddr const templateStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
          REQUIRE(templateStructAddr.IsValid());
          ScStruct templateStruct(&ctx, templateStructAddr);
          templateStruct
                << x_elem
                << templateEdge1
                << _y
                << templateEdge2
                << _nrel;

          ScTemplateParams templateParams;
          templateParams.Add("_y", y_elem);
          templateParams.Add("_nrel", nrel_test);

          ScTemplate scTemplate;
          REQUIRE(ctx.HelperBuildTemplate(scTemplate, templateStructAddr, templateParams));
          {
            ScTemplateSearchResult result;
            REQUIRE(ctx.HelperSearchTemplate(scTemplate, result));
            {
              REQUIRE(result.Size() == 1);
            }
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_search_with_params_6\" failed")
        }
      }
      SUBTEST_END()
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_search_with_params\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("templates_2", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
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
  try
  {
    ScAddr templateAddr = ctx.CreateNode(ScType::NodeConstStruct);
    REQUIRE(templateAddr.IsValid());

    ScStruct templStruct(&ctx, templateAddr);
    ScAddr xAddr;
    {

      ScAddr _yAddr, _zAddr, _sAddr;

      xAddr = ctx.CreateNode(ScType::NodeConstMaterial);
      REQUIRE(xAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("x", xAddr));

      _yAddr = ctx.CreateNode(ScType::Var);
      REQUIRE(_yAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_y", _yAddr));

      _zAddr = ctx.CreateNode(ScType::NodeVarRole);
      REQUIRE(_zAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_z", _zAddr));

      _sAddr = ctx.CreateNode(ScType::NodeVarClass);
      REQUIRE(_sAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_s", _sAddr));

      ScAddr xyAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, xAddr, _yAddr);
      REQUIRE(xyAddr.IsValid());
      ScAddr zxyAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _zAddr, xyAddr);
      REQUIRE(zxyAddr.IsValid());
      ScAddr sxAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _sAddr, xAddr);
      REQUIRE(sxAddr.IsValid());

      // append created elements into struct
      templStruct << xAddr << _yAddr << _zAddr << xyAddr << zxyAddr << _sAddr << sxAddr;
    }

    ScTemplate templ;
    REQUIRE(ctx.HelperBuildTemplate(templ, templateAddr));

    // create test structure that correspond to template
    {
      ScAddr yAddr, zAddr, sAddr;

      yAddr = ctx.CreateNode(ScType::Const);
      REQUIRE(yAddr.IsValid());

      zAddr = ctx.CreateNode(ScType::NodeConstRole);
      REQUIRE(zAddr.IsValid());

      sAddr = ctx.CreateNode(ScType::NodeConstClass);
      REQUIRE(sAddr.IsValid());

      ScAddr xyAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, xAddr, yAddr);
      REQUIRE(xyAddr.IsValid());
      ScAddr zxyAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, zAddr, xyAddr);
      REQUIRE(zxyAddr.IsValid());
      ScAddr sxAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, sAddr, xAddr);
      REQUIRE(sxAddr.IsValid());


      // test search by template
      {
        ScTemplateSearchResult result;
        REQUIRE(ctx.HelperSearchTemplate(templ, result));

        REQUIRE(result.Size() == 1);
        ScTemplateSearchResultItem item = result[0];

        REQUIRE(item["x"] == xAddr);
        REQUIRE(item["_y"] == yAddr);
        REQUIRE(item["_z"] == zAddr);
        REQUIRE(item["_s"] == sAddr);
      }
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"templates_2\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("templates_3", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "templates_3");

  /** SCs:
   * addr => nrel_main_idtf: [] (* <- lang;; *);;
   */

  try
  {
    ScAddr const addr = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr.IsValid());
    ScAddr const nrelMainIdtf = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(nrelMainIdtf.IsValid());
    ScAddr const lang = ctx.CreateNode(ScType::NodeConstClass);
    REQUIRE(lang.IsValid());
    ScAddr const link = ctx.CreateLink();
    REQUIRE(link.IsValid());

    ScAddr const edgeCommon = ctx.CreateEdge(ScType::EdgeDCommonConst, addr, link);
    REQUIRE(edgeCommon.IsValid());
    ScAddr const edgeAttr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelMainIdtf, edgeCommon);
    REQUIRE(edgeAttr.IsValid());
    ScAddr const edgeLang = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, lang, link);
    REQUIRE(edgeLang.IsValid());

    // now check search
    ScTemplate templ;
    templ.TripleWithRelation(
          addr >> "_addr",
          ScType::EdgeDCommonVar >> "_edgeCommon",
          ScType::Link >> "_link",
          ScType::EdgeAccessVarPosPerm >> "_edgeAttr",
          nrelMainIdtf >> "_nrelMainIdtf");

    templ.Triple(
          lang >> "_lang",
          ScType::EdgeAccessVarPosPerm >> "_edgeLang",
          "_link");

    // search
    {
      ScTemplateSearchResult res;
      REQUIRE(ctx.HelperSearchTemplate(templ, res));

      REQUIRE(res.Size() == 1);
      REQUIRE(res[0]["_addr"] == addr);
      REQUIRE(res[0]["_edgeCommon"] == edgeCommon);
      REQUIRE(res[0]["_link"] == link);
      REQUIRE(res[0]["_edgeAttr"] == edgeAttr);
      REQUIRE(res[0]["_nrelMainIdtf"] == nrelMainIdtf);
      REQUIRE(res[0]["_lang"] == lang);
      REQUIRE(res[0]["_edgeLang"] == edgeLang);
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"templates_3\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_search_in_struct", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_search_in_struct");

  /*
  *   _y
  *    ^
  *    | <--- _z
  *    x
  * scs: x _-> _z:: _y;;
  */

  try
  {
    ScAddr templateAddr = ctx.CreateNode(ScType::NodeConstStruct);
    REQUIRE(templateAddr.IsValid());

    ScAddr xAddr, _yAddr, _zAddr, _xyEdgeAddr, _zxyEdgeAddr;
    ScStruct templStruct(&ctx, templateAddr);
    {
      xAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(xAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("x", xAddr));

      _yAddr = ctx.CreateNode(ScType::NodeVar);
      REQUIRE(_yAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_y", _yAddr));

      _zAddr = ctx.CreateNode(ScType::NodeVar);
      REQUIRE(_zAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_z", _zAddr));

      _xyEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, xAddr, _yAddr);
      REQUIRE(_xyEdgeAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_xyEdge", _xyEdgeAddr));

      _zxyEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, _zAddr, _xyEdgeAddr);
      REQUIRE(_zxyEdgeAddr.IsValid());
      REQUIRE(ctx.HelperSetSystemIdtf("_zxyEdge", _zxyEdgeAddr));

      templStruct << xAddr << _yAddr << _zAddr << _xyEdgeAddr << _zxyEdgeAddr;
    }

    ScTemplate templ;
    REQUIRE(ctx.HelperBuildTemplate(templ, templateAddr));

    // create text struct
    ScAddr testStructAddr = ctx.CreateNode(ScType::NodeConstStruct);
    REQUIRE(testStructAddr.IsValid());

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

      tyAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(tyAddr.IsValid());

      tgAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(tgAddr.IsValid());

      tuAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(tuAddr.IsValid());

      tzAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(tzAddr.IsValid());

      tsAddr = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(tsAddr.IsValid());

      tyuEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, tyAddr, tuAddr);
      REQUIRE(tyuEdgeAddr.IsValid());

      txyEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
      REQUIRE(txyEdgeAddr.IsValid());

      txgEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tgAddr);
      REQUIRE(txgEdgeAddr.IsValid());

      tzxyEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, tzAddr, txyEdgeAddr);
      REQUIRE(tzxyEdgeAddr.IsValid());

      tsxgEdgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, tsAddr, txgEdgeAddr);
      REQUIRE(tsxgEdgeAddr.IsValid());

      testStruct << tyAddr << txAddr << tgAddr
                 << tuAddr << tzAddr << tsAddr
                 << tyuEdgeAddr << txyEdgeAddr
                 << txgEdgeAddr << tzxyEdgeAddr
                 << tsxgEdgeAddr;
    }

    // add extra edges that not included into struct
    // scs: x -> t: y;;
    ScAddr edge1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, txAddr, tyAddr);
    REQUIRE(edge1.IsValid());
    ScAddr edge2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, tzAddr, edge1);
    REQUIRE(edge2.IsValid());

    {
      ScTemplateSearchResult result;
      REQUIRE(ctx.HelperSearchTemplateInStruct(templ, *testStruct, result));

      REQUIRE(result.Size() == 2);
      for (uint32_t i = 0; i < result.Size(); ++i)
      {
        ScTemplateSearchResultItem res1 = result[i];
        bool res = (res1["_y"] == tyAddr || res1["_y"] == tgAddr) &&
                   (res1["_z"] == tzAddr || res1["_z"] == tsAddr) &&
                   (res1["_xyEdge"] == txyEdgeAddr || res1["_xyEdge"] == txgEdgeAddr) &&
                   (res1["_zxyEdge"] == tsxgEdgeAddr || res1["_zxyEdge"] == tzxyEdgeAddr);
        REQUIRE(res);
      }

    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_search_in_struct\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_performance", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScAddr node1, node2, node3, node4, edge1, edge2, edge3;
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_performance");

  try
  {
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
      REQUIRE(ctx.HelperGenTemplate(templ, result));
      node1 = result["Node1"];
      node2 = result["Node2"];
      node3 = result["Node3"];
      node4 = result["Node4"];
      edge1 = result["Edge1"];
      edge2 = result["Edge2"];
      edge3 = result["Edge3"];
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
      REQUIRE(res);
    }

    SC_LOG_INFO("Time: " << sum_time);
    SC_LOG_INFO("Time per search: " << (sum_time / iterCount));
    SC_LOG_INFO("Search per second: " << (iterCount / sum_time) << " search/sec \n");
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_performance\" failed")
  }
  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_double_attrs", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_double_attrs");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(addr2.IsValid());
    ScAddr const addr3 = ctx.CreateNode(ScType::NodeConstRole);
    REQUIRE(addr3.IsValid());
    ScAddr const addr4 = ctx.CreateNode(ScType::NodeConstRole);
    REQUIRE(addr4.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    REQUIRE(edge1.IsValid());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr3, edge1);
    REQUIRE(edge2.IsValid());
    ScAddr const edge3 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr4, edge1);
    REQUIRE(edge3.IsValid());

    auto const testOrder = [&ctx](std::vector<ScAddr> const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));
    };

    SECTION("forward_order")
    {
      SUBTEST_START("forward_order")
      {
        try
        {
          testOrder({ addr1, addr2, addr3, addr4, edge1, edge2, edge3 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"forward_order\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("backward_order")
    {
      SUBTEST_START("backward_order")
      {
        try
        {
          testOrder({ edge3, edge2, edge1, addr4, addr3, addr2, addr1 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"backward_order\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("test_order")
    {
      SUBTEST_START("test_order")
      {
        try
        {
          testOrder({ addr1, addr2, addr3, addr4, edge2, edge1, edge3 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"test_order\" failed")
        }
      }
      SUBTEST_END()
    }

  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_double_attrs\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_edge_from_edge", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_edge_from_edge");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(addr2.IsValid());
    ScAddr const addr3 = ctx.CreateNode(ScType::NodeConstRole);
    REQUIRE(addr3.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    REQUIRE(edge1.IsValid());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, edge1, addr3);
    REQUIRE(edge2.IsValid());

    auto const testOrder = [&ctx](std::vector<ScAddr> const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));
    };

    SECTION("forward_order")
    {
      SUBTEST_START("forward_order")
      {
        try
        {
          testOrder({ addr1, addr2, addr3, edge1, edge2 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"forward_order\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("backward_order")
    {
      SUBTEST_START("backward_order")
      {
        try
        {
          testOrder({ edge2, edge1, addr3, addr2, addr1 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"backward_order\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("test_order")
    {
      SUBTEST_START("test_order")
      {
        try
        {
          testOrder({ addr1, addr2, addr3, edge2, edge1 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"test_order\" failed")
        }
      }
      SUBTEST_END()
    }

  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_edge_from_edge\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}


//                _tuple --------> addr1
//                   |      ||
//                   |======|| <---- norole
//                   |
//                  \/
//                _addr2
//
TEST_CASE("template_edge_from_edge_to_edge", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_edge_from_edge_to_edge");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(addr2.IsValid());
    ScAddr const addrTuple = ctx.CreateNode(ScType::NodeVarTuple);
    REQUIRE(addrTuple.IsValid());
    ScAddr const addrNoRole = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(addrNoRole.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addrTuple, addr1);
    REQUIRE(edge1.IsValid());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addrTuple, addr2);
    REQUIRE(edge2.IsValid());
    ScAddr const commonEdge = ctx.CreateEdge(ScType::EdgeDCommonVar, edge1, edge2);
    REQUIRE(commonEdge.IsValid());
    ScAddr const edgeToCommon = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addrNoRole, commonEdge);
    REQUIRE(edgeToCommon.IsValid());

    auto const testOrder = [&ctx, addr1, addrNoRole](ScAddrVector const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));

      ScTemplateGenResult result;
      REQUIRE(ctx.HelperGenTemplate(templ, result));

      ScIterator3Ptr it3 = ctx.Iterator3(
            ScType::NodeConstTuple,
            ScType::EdgeAccessConstPosPerm,
            addr1);
      REQUIRE(it3->Next());

      ScIterator5Ptr it5 = ctx.Iterator5(
            it3->Get(1),
            ScType::EdgeDCommonConst,
            ScType::EdgeAccessConstPosPerm,
            ScType::EdgeAccessConstPosPerm,
            addrNoRole);
      REQUIRE(it5->Next());
      REQUIRE(it5->Get(1).IsValid());
    };

    SECTION("forward_order")
    {
      SUBTEST_START("forward_order")
      {
        try
        {
          testOrder({ addr1, addr2, addrTuple, addrNoRole, edge1, edge2, commonEdge, edgeToCommon });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"forward_order\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("test_order")
    {
      SUBTEST_START("test_order")
      {
        try
        {
          testOrder({ edge1, edge2, edgeToCommon, commonEdge, addrTuple, addr1, addr2, addrNoRole });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"test_order\" failed")
        }
      }
      SUBTEST_END()
    }

  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_edge_from_edge_to_edge\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

//
//                _addr2-------     _addr3   _addr4
//                   /\       |       ||        |
//                   |        |       ||        |
//                   |        |       ||        |====
//                   |        |       ||<--------   ||
//                   |        |<=======             ||
//                   |<--------                     ||
//                   |                              ||
//                   |<==============================
//                   |                /\
//                   |                 |
//                 addr1---------------
//
TEST_CASE("template_high_edge_dependence_power", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_high_edge_dependence_power");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(addr2.IsValid());
    ScAddr const addr3 = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(addr3.IsValid());
    ScAddr const addr4 = ctx.CreateNode(ScType::NodeVar);
    REQUIRE(addr4.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    REQUIRE(edge1.IsValid());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr2, edge1);
    REQUIRE(edge2.IsValid());
    ScAddr const edge3 = ctx.CreateEdge(ScType::EdgeDCommonVar, addr3, edge2);
    REQUIRE(edge3.IsValid());
    ScAddr const edge4 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr4, edge3);
    REQUIRE(edge4.IsValid());
    ScAddr const edge5 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, edge4);
    REQUIRE(edge5.IsValid());
    ScAddr const edge6 = ctx.CreateEdge(ScType::EdgeDCommonVar, edge5, edge2);
    REQUIRE(edge6.IsValid());
    ScAddr const edge7 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, edge6);
    REQUIRE(edge7.IsValid());

    auto const testOrder = [&ctx, addr1](ScAddrVector const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));

      ScTemplateGenResult result;
      REQUIRE(ctx.HelperGenTemplate(templ, result));

      ScIterator3Ptr it3 = ctx.Iterator3(
            addr1,
            ScType::EdgeAccessConstPosPerm,
            ScType::NodeConst);
      REQUIRE(it3->Next());

      it3 = ctx.Iterator3(
            ScType::NodeConst, // addr2 value
            ScType::EdgeAccessConstPosPerm,
            it3->Get(1));
      REQUIRE(it3->Next());

      ScAddr const edge2 = it3->Get(1);

      it3 = ctx.Iterator3(
            ScType::NodeConst, // addr3 value
            ScType::EdgeDCommonConst,
            edge2);
      REQUIRE(it3->Next());

      it3 = ctx.Iterator3(
            ScType::NodeConst, // addr4 value
            ScType::EdgeAccessConstPosPerm,
            it3->Get(1));
      REQUIRE(it3->Next());

      it3 = ctx.Iterator3(
            addr1,
            ScType::EdgeAccessConstPosPerm,
            it3->Get(1));
      REQUIRE(it3->Next());

      ScIterator5Ptr it5 = ctx.Iterator5(
            it3->Get(1),
            ScType::EdgeDCommonConst,
            ScType::EdgeAccessConstPosPerm,
            ScType::EdgeAccessConstPosPerm,
            addr1);
      REQUIRE(it5->Next());
      REQUIRE(it5->Get(1).IsValid());
    };

    SECTION("forward_order")
    {
      SUBTEST_START("forward_order")
      {
        try
        {
          testOrder({ addr1, addr2, addr3, addr4,
                      edge1, edge2, edge3, edge4, edge5, edge6, edge7 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_high_edge_dependence_power\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("test_order")
    {
      SUBTEST_START("test_order")
      {
        try
        {
          testOrder({ edge1, edge2, edge3, edge4, edge5, edge6, edge7,
                      addr1, addr2, addr3, addr4});
        } catch (...)
        {
          SC_LOG_ERROR("Test \"test_order\" failed")
        }
      }
      SUBTEST_END()
    }

  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_edge_from_edge\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

//
//                _addr2-------     _addr3   _addr4
//                   /\       |       ||        |
//                   |        |       ||        |
//                   |        |       ||        |====
//                   |        |       ||<--------   ||
//                   |        |<=======             ||
//                   |<--------                     ||
//                   |                              ||
//                   |<==============================
//                   |                /\
//                   |                 |
//                 addr1---------------
//
TEST_CASE("template_high_edge_dependence_power_search", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_high_edge_dependence_power_search");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    ctx.HelperSetSystemIdtf("_addr1", addr1);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeVar);
    ctx.HelperSetSystemIdtf("_addr2", addr2);
    REQUIRE(addr2.IsValid());
    ScAddr const addr3 = ctx.CreateNode(ScType::NodeVar);
    ctx.HelperSetSystemIdtf("_addr3", addr3);
    REQUIRE(addr3.IsValid());
    ScAddr const addr4 = ctx.CreateNode(ScType::NodeVar);
    ctx.HelperSetSystemIdtf("_addr4", addr4);
    REQUIRE(addr4.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    ctx.HelperSetSystemIdtf("_edge1", edge1);
    REQUIRE(edge1.IsValid());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr2, edge1);
    ctx.HelperSetSystemIdtf("_edge2", edge2);
    REQUIRE(edge2.IsValid());
    ScAddr const edge3 = ctx.CreateEdge(ScType::EdgeDCommonVar, addr3, edge2);
    ctx.HelperSetSystemIdtf("_edge3", edge3);
    REQUIRE(edge3.IsValid());
    ScAddr const edge4 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr4, edge3);
    ctx.HelperSetSystemIdtf("_edge4", edge4);
    REQUIRE(edge4.IsValid());
    ScAddr const edge5 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, edge4);
    ctx.HelperSetSystemIdtf("_edge5", edge5);
    REQUIRE(edge5.IsValid());
    ScAddr const edge6 = ctx.CreateEdge(ScType::EdgeDCommonVar, edge5, edge2);
    ctx.HelperSetSystemIdtf("_edge6", edge6);
    REQUIRE(edge6.IsValid());
    ScAddr const edge7 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, edge6);
    ctx.HelperSetSystemIdtf("_edge7", edge7);
    REQUIRE(edge7.IsValid());

    auto const testOrder = [&ctx](ScAddrVector const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));

      ScTemplateGenResult genResult;
      REQUIRE(ctx.HelperGenTemplate(templ, genResult));

      ScTemplateSearchResult searchResult;
      REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

      REQUIRE(searchResult[0]["_addr1"].IsValid());
      REQUIRE(searchResult[0]["_addr2"].IsValid());
      REQUIRE(searchResult[0]["_addr3"].IsValid());
      REQUIRE(searchResult[0]["_addr4"].IsValid());

      REQUIRE(searchResult[0]["_edge1"].IsValid());
      REQUIRE(searchResult[0]["_edge2"].IsValid());
      REQUIRE(searchResult[0]["_edge3"].IsValid());
      REQUIRE(searchResult[0]["_edge4"].IsValid());
      REQUIRE(searchResult[0]["_edge5"].IsValid());
      REQUIRE(searchResult[0]["_edge6"].IsValid());
      REQUIRE(searchResult[0]["_edge7"].IsValid());
    };

    SECTION("forward_order")
    {
      SUBTEST_START("forward_order")
      {
        try
        {
          testOrder({ addr1, addr2, addr3, addr4,
                      edge1, edge2, edge3, edge4, edge5, edge6, edge7 });
        } catch (...)
        {
          SC_LOG_ERROR("Test \"template_high_edge_dependence_power_search\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("test_order")
    {
      SUBTEST_START("test_order")
      {
        try
        {
          testOrder({ edge1, edge2, edge3, edge4, edge5, edge6, edge7,
                      addr1, addr2, addr3, addr4});
        } catch (...)
        {
          SC_LOG_ERROR("Test \"test_order\" failed")
        }
      }
      SUBTEST_END()
    }

  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_high_edge_dependence_power_search\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_replacement_naming", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_replacement_naming");

  try
  {
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    ctx.HelperSetSystemIdtf("addr1", addr1);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateLink(ScType::LinkVar);
    REQUIRE(addr2.IsValid());
    ScAddr const addr3 = ctx.CreateLink(ScType::LinkVar);
    REQUIRE(addr2.IsValid());

    ScAddr const edge1 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr2);
    REQUIRE(edge1.IsValid());
    ScAddr const edge2 = ctx.CreateEdge(ScType::EdgeAccessVarPosPerm, addr1, addr3);
    REQUIRE(edge1.IsValid());


    auto const testReplacement = [&ctx](ScAddrVector const & addrs)
    {
      ScAddr const structAddr = ctx.CreateNode(ScType::NodeConstStruct);
      ScStruct st(&ctx, structAddr);

      for (auto const & a : addrs)
        st << a;

      ScTemplate templ;
      REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));

      ScTemplateGenResult genResult;
      REQUIRE(ctx.HelperGenTemplate(templ, genResult));

      ScTemplateSearchResult searchResult;
      REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

      REQUIRE(searchResult[0]["addr1"].IsValid());

      ScIterator3Ptr it3 = ctx.Iterator3(
            searchResult[0]["addr1"],
            ScType::EdgeAccessConstPosPerm,
            ScType::LinkConst);
      REQUIRE(it3->Next());
      REQUIRE(ctx.GetElementType(it3->Get(2)) == ScType::LinkConst);

      REQUIRE(it3->Next());
      REQUIRE(ctx.GetElementType(it3->Get(2)) == ScType::LinkConst);
    };

    testReplacement({ addr1, addr2, addr3, edge1, edge2 });

  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_replacement_naming\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

// https://github.com/ostis-dev/sc-machine/issues/224
TEST_CASE("template_issue_224", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "template_issue_224");

  /// TODO: replace with scs string in future
  // create template in memory
  try
  {
    std::vector<ScAddr> contourItems;
    {
      auto testCreateNode = [&ctx, &contourItems](ScType const & type)
      {
        ScAddr const addr = ctx.CreateNode(type);
        REQUIRE(addr.IsValid());
        contourItems.push_back(addr);
        return addr;
      };

      auto testCreateNodeIdtf = [&ctx, &contourItems](ScType const & type, std::string const & idtf)
      {
        ScAddr const addr = ResolveKeynode(ctx, type, idtf);
        REQUIRE(addr.IsValid());
        contourItems.push_back(addr);
        return addr;
      };

      auto testCreateEdge = [&ctx, &contourItems](ScType const & type, ScAddr const & src, ScAddr const & trg)
      {
        ScAddr const edge = ctx.CreateEdge(type, src, trg);
        REQUIRE(edge.IsValid());
        contourItems.push_back(edge);
        return edge;
      };

      auto testCreateEdgeAttrs = [&ctx, &testCreateEdge](ScType const & type,
                                                         ScAddr const & src, ScAddr const & trg,
                                                         ScType const & attrsEdgeType,
                                                         std::vector<ScAddr> const & attrNodes)
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
      ScAddr const nrel_decompoisition_of_actionAddr = testCreateNodeIdtf(ScType::NodeVarNoRole,
                                                                          "nrel_decomposition_of_action");
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
      for (size_t i = 0; i < testNum; ++i)
      {
        shuffle(1);
        ScAddr const structAddr = ResolveKeynode(ctx, ScType::NodeConstStruct, "test_program" + std::to_string(i));
        ScStruct contour(&ctx, structAddr);

        for (auto const & a : contourItems)
          contour << a;

        ScTemplate templ;
        REQUIRE(ctx.HelperBuildTemplate(templ, structAddr));
      }
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_issue_224\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

// https://github.com/ostis-dev/sc-machine/issues/251
TEST_CASE("template_issue_251", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_issue_251");

  try
  {
    /* generate equal to scs:
     * k => rel: [] (* <- t;; *);;
     */
    ScAddr const kAddr = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(kAddr.IsValid());
    ScAddr const relAddr = ctx.CreateNode(ScType::NodeConstRole);
    REQUIRE(relAddr.IsValid());
    ScAddr const tAddr = ctx.CreateNode(ScType::NodeConstClass);
    REQUIRE(tAddr.IsValid());
    ScAddr const linkAddr = ctx.CreateLink();
    REQUIRE(linkAddr.IsValid());

    ScAddr const edgeK_link = ctx.CreateEdge(ScType::EdgeDCommonConst, kAddr, linkAddr);
    REQUIRE(edgeK_link.IsValid());
    ScAddr const edgeT_link = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, tAddr, linkAddr);
    REQUIRE(edgeT_link.IsValid());
    ScAddr const edgeRel_edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, relAddr, edgeK_link);
    REQUIRE(edgeRel_edge.IsValid());

    // create template for a search
    ScTemplate templ;

    templ.TripleWithRelation(
          kAddr,
          ScType::EdgeDCommonVar,
          ScType::Link >> "_link",
          ScType::EdgeAccessVarPosPerm,
          relAddr
    );
    templ.Triple(
          tAddr,
          ScType::EdgeAccessVarPosPerm,
          "_link"
    );

    ScTemplateSearchResult res;
    REQUIRE(ctx.HelperSearchTemplate(templ, res));

    // checks
    REQUIRE(res.Size() == 1);

    REQUIRE(res[0]["_link"] == linkAddr);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_issue_251\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_search_unknown", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_search_unknown");

  try
  {
    // addr1 -> addr2;;
    ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr1.IsValid());
    ScAddr const addr2 = ctx.CreateNode(ScType::NodeConstAbstract);
    REQUIRE(addr2.IsValid());

    ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
    REQUIRE(edge.IsValid());


    ScTemplate templ;
    templ.Triple(
          addr1,
          ScType::EdgeAccessVarPosPerm >> "edge",
          ScType::Unknown >> "addr2"
    );

    ScTemplateSearchResult res;
    REQUIRE(ctx.HelperSearchTemplate(templ, res));
    REQUIRE(res.Size() == 1);

    REQUIRE(res[0]["edge"] == edge);
    REQUIRE(res[0]["addr2"] == addr2);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_search_unknown\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_search_some_relations", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_search_some_relations");

  try
  {
    /* Check template:
      deviceAddr _<= nrel_installed_apps: _tuple;;
      _tuple _-> _app;
      _app => nrel_idtf: _idtf;;
      _app => nrel_image: _image;;
     */

    ScAddr const deviceAddr = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(deviceAddr.IsValid());

    ScAddr const nrelInstalledApp = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(nrelInstalledApp.IsValid());

    ScAddr const _tuple = ctx.CreateNode(ScType::NodeConstTuple);
    REQUIRE(_tuple.IsValid());

    ScAddr const nrelIdtf = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(nrelIdtf.IsValid());

    ScAddr const nrelImage = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(nrelImage.IsValid());

    ScAddr edge = ctx.CreateEdge(ScType::EdgeDCommonConst, _tuple, deviceAddr);
    REQUIRE(edge.IsValid());

    edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelInstalledApp, edge);
    REQUIRE(edge.IsValid());

    struct TestData
    {
      ScAddr m_app;
      ScAddr m_idtf;
      ScAddr m_image;
    };

    size_t i = 0;
    std::vector<TestData> data(100);
    for (auto & d : data)
    {
      d.m_app = ctx.CreateNode(ScType::NodeConstAbstract);
      REQUIRE(d.m_app.IsValid());

      edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, _tuple, d.m_app);
      REQUIRE(edge.IsValid());

      d.m_idtf = ctx.CreateLink();
      REQUIRE(d.m_idtf.IsValid());

      ScLink idtfLink(ctx, d.m_idtf);
      REQUIRE(idtfLink.Set("idtf_" + std::to_string(i)));

      edge = ctx.CreateEdge(ScType::EdgeDCommonConst, d.m_app, d.m_idtf);
      REQUIRE(edge.IsValid());

      edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelIdtf, edge);
      REQUIRE(edge.IsValid());

      d.m_image = ctx.CreateLink();
      REQUIRE(d.m_image.IsValid());

      ScLink imageLink(ctx, d.m_image);
      REQUIRE(imageLink.Set("data_" + std::to_string(i)));

      edge = ctx.CreateEdge(ScType::EdgeDCommonConst, d.m_app, d.m_image);
      REQUIRE(edge.IsValid());

      edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelImage, edge);
      REQUIRE(edge.IsValid());

      ++i;
    }

    SECTION("one_thread")
    {
      SUBTEST_START("one_thread")
      {
        try
        {
          ScTemplate templ;

          templ.TripleWithRelation(
                ScType::NodeVarTuple >> "_tuple",
                ScType::EdgeDCommonVar,
                deviceAddr,
                ScType::EdgeAccessVarPosPerm,
                nrelInstalledApp);

          templ.Triple(
                "_tuple",
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVar >> "_app");

          templ.TripleWithRelation(
                "_app",
                ScType::EdgeDCommonVar,
                ScType::Link >> "_idtf",
                ScType::EdgeAccessVarPosPerm,
                nrelIdtf);

          templ.TripleWithRelation(
                "_app",
                ScType::EdgeDCommonVar,
                ScType::Link >> "_image",
                ScType::EdgeAccessVarPosPerm,
                nrelImage);

          ScTemplateSearchResult searchRes;
          REQUIRE(ctx.HelperSearchTemplate(templ, searchRes));


          REQUIRE(searchRes.Size() == data.size());
          std::vector<TestData> foundData(data.size());
          for (size_t i = 0; i < searchRes.Size(); ++i)
          {
            auto & d = foundData[i];

            d.m_app = searchRes[i]["_app"];
            d.m_idtf = searchRes[i]["_idtf"];
            d.m_image = searchRes[i]["_image"];
          }

          auto compare = [](TestData const & a, TestData const & b)
          {
            return (a.m_app.Hash() < b.m_app.Hash());
          };
          std::sort(data.begin(), data.end(), compare);
          std::sort(foundData.begin(), foundData.end(), compare);

          for (size_t i = 0; i < searchRes.Size(); ++i)
          {
            auto & d1 = foundData[i];
            auto & d2 = data[i];

            REQUIRE(d1.m_app == d2.m_app);
            REQUIRE(d1.m_idtf == d2.m_idtf);
            REQUIRE(d1.m_image == d2.m_image);
          }
        } catch (...)
        {
          SC_LOG_ERROR("Test \"one_thread\" failed")
        }
      }
      SUBTEST_END()
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_search_some_relations\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_one_edge_inclusion", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_one_edge_inclusion");

  try
  {
    /* In case:
        a -> b (* <- sc_node_material;; *);;
        a -> c;;

        We should get just one seach result, edge `a -> c` shouldn't appears twicely
     */

    ScAddr const a = ctx.CreateNode(ScType::Node);
    REQUIRE(a.IsValid());

    ScTemplate templ;

    templ.Triple(
          a >> "a",
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVarMaterial >> "b");

    templ.Triple(
          "a",
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVar >> "c");

    ScTemplateGenResult genResult;
    REQUIRE(ctx.HelperGenTemplate(templ, genResult));
    REQUIRE(a == genResult["a"]);

    ScTemplateSearchResult searchResult;
    REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));
    REQUIRE(searchResult.Size() == 1);

    REQUIRE(searchResult[0]["a"] == genResult["a"]);
    REQUIRE(searchResult[0]["b"] == genResult["b"]);
    REQUIRE(searchResult[0]["c"] == genResult["c"]);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_one_edge_inclusion\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_templates", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_templates");

  SECTION("build_ok")
  {
    SUBTEST_START("build_ok")
    {
      try
      {
        ScAddr const addr = ctx.CreateNode(ScType::NodeConst);
        REQUIRE(addr.IsValid());
        REQUIRE(ctx.HelperSetSystemIdtf("d", addr));

        ScTemplate templ;
        char const * data = "_a _-> d;;";
        REQUIRE(ctx.HelperBuildTemplate(templ, data));
      } catch (...)
      {
        SC_LOG_ERROR("Test \"build_ok\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("build_fail")
  {
    SUBTEST_START("build_fail")
    {
      try
      {
        ScTemplate templ;
        char const * data = "_a _-> b";
        REQUIRE_FALSE(ctx.HelperBuildTemplate(templ, data));
      } catch (...)
      {
        SC_LOG_ERROR("Test \"build_fail\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("search_simple")
  {
    SUBTEST_START("search_simple")
    {
      try
      {
        ScTemplate genTempl;
        genTempl.Triple(
              ScType::NodeVar >> "_a",
              ScType::EdgeAccessVarPosPerm >> "_edge",
              ScType::NodeVarTuple >> "b");

        ScTemplateGenResult genResult;
        REQUIRE(ctx.HelperGenTemplate(genTempl, genResult));

        ScAddr const bAddr = genResult["b"];
        REQUIRE(bAddr.IsValid());

        REQUIRE(ctx.HelperSetSystemIdtf("b", bAddr));
        {
          ScTemplate templ;
          char const * data = "_a _-> b (* <- sc_node_tuple;; *);;";

          REQUIRE(ctx.HelperBuildTemplate(templ, data));
          REQUIRE_FALSE(templ.IsEmpty());

          ScTemplateSearchResult searchResult;
          REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

          REQUIRE(searchResult.Size() == 1);
          REQUIRE(searchResult[0]["_a"] == genResult["_a"]);
          REQUIRE(searchResult[0]["b"] == genResult["b"]);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"search_simple\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("gen_simple")
  {
    SUBTEST_START("gen_simple")
    {
      try
      {
        ScAddr const cAddr = ctx.CreateNode(ScType::Node);
        REQUIRE(cAddr.IsValid());
        REQUIRE(ctx.HelperSetSystemIdtf("c1", cAddr));

        ScTemplate templ;
        char const * data = "c1 _=> _b1;; _b1 <- sc_node_abstract;;";

        REQUIRE(ctx.HelperBuildTemplate(templ, data));
        ScTemplateGenResult genResult;
        REQUIRE(ctx.HelperGenTemplate(templ, genResult));

        // check
        {
          ScTemplate searchTempl;

          searchTempl.Triple(
                cAddr >> "c1",
                ScType::EdgeDCommonVar >> "_edge",
                ScType::NodeVarAbstract >> "_b1");

          ScTemplateSearchResult searchResult;
          REQUIRE(ctx.HelperSearchTemplate(searchTempl, searchResult));

          REQUIRE(searchResult.Size() == 1);
          REQUIRE(genResult["c1"] == searchResult[0]["c1"]);
          REQUIRE(genResult["_b1"] == searchResult[0]["_b1"]);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"search_simple\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("gen_search")
  {
    SUBTEST_START("gen_search")
    {
      try
      {
        ScAddr const cAddr = ctx.CreateNode(ScType::Node);
        REQUIRE(cAddr.IsValid());
        REQUIRE(ctx.HelperSetSystemIdtf("g1", cAddr));

        ScTemplate templ;
        char const * data = "g1 _-> _l1 (* <- sc_node_material;; *);; g1 _-> _f1;;";

        REQUIRE(ctx.HelperBuildTemplate(templ, data));
        ScTemplateGenResult genResult;
        REQUIRE(ctx.HelperGenTemplate(templ, genResult));
        REQUIRE(ctx.GetElementType(genResult["_l1"]) == ScType::NodeConstMaterial);
        REQUIRE(ctx.GetElementType(genResult["_f1"]) == ScType::NodeConst);

        // check
        {
          ScTemplateSearchResult searchResult;
          REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

          REQUIRE(searchResult.Size() == 1);
          REQUIRE(genResult["g1"] == searchResult[0]["g1"]);
          REQUIRE(genResult["_l1"] == searchResult[0]["_l1"]);
          REQUIRE(genResult["_f1"] == searchResult[0]["_f1"]);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"gen_search\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
//TODO
/*
TEST_CASE("big_template_2_15", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "pend_events");

  try
  {
    ScAddr const set1 = ctx.CreateNode(ScType::NodeConstClass);
    ScAddr const rel = ctx.CreateNode(ScType::NodeConstNoRole);

    static const size_t el_num = 1 << 15;
    std::set<ScAddr, ScAddLessFunc> elements;
    for (size_t i = 0; i < el_num; ++i)
    {
      ScAddr const a = ctx.CreateNode(ScType::NodeConst);
      REQUIRE(a.IsValid());
      elements.insert(a);
    }

    // create template for pending events check
    ScTemplate templ;
    for (auto const & a : elements)
    {
      templ.TripleWithRelation(
            set1,
            ScType::EdgeDCommonVar,
            a >> "_el",
            ScType::EdgeAccessVarPosPerm,
            rel);
    }

    ScTemplateGenResult genResult;
    REQUIRE(ctx.HelperGenTemplate(templ, genResult));

    // ensure whole data created correctly
    ScTemplateSearchResult searchResult;
    REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

    for (size_t i = 0; i < searchResult.Size(); ++i)
    {
      REQUIRE(elements.find(searchResult[i]["_el"]) != elements.end());
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"big_template_2_15\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}*/

TEST_CASE("template_optional_complex", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_optional_complex");

  try
  {
    SCsHelper helper(ctx, std::make_shared<DummyFileInterface>());
    std::string const scsData =
          "langs <- sc_node_class;;"
          "langEN <- langs;;"
          "nrelMainIdtf <- sc_node_norole_relation;;"
          "nrelImage <- sc_node_norole_relation;;"
          "nrelSysIdtf <- sc_node_norole_relation;;"
          "item"
          "  => nrelSysIdtf:"
          "   [item];;";

    REQUIRE(helper.GenerateBySCsText(scsData));

    ScAddr const item = ctx.HelperFindBySystemIdtf("item");
    REQUIRE(item.IsValid());

    ScAddr const languages = ctx.HelperFindBySystemIdtf("langs");
    REQUIRE(languages.IsValid());
    REQUIRE(ctx.GetElementType(languages) == ScType::NodeConstClass);

    ScAddr const nrelSysIdtf = ctx.HelperFindBySystemIdtf("nrelSysIdtf");
    REQUIRE(nrelSysIdtf.IsValid());
    REQUIRE(ctx.GetElementType(nrelSysIdtf) == ScType::NodeConstNoRole);

    ScAddr const nrelMainIdtf = ctx.HelperFindBySystemIdtf("nrelMainIdtf");
    REQUIRE(nrelMainIdtf.IsValid());
    REQUIRE(ctx.GetElementType(nrelMainIdtf) == ScType::NodeConstNoRole);

    ScAddr const nrelImage = ctx.HelperFindBySystemIdtf("nrelImage");
    REQUIRE(nrelImage.IsValid());
    REQUIRE(ctx.GetElementType(nrelImage) == ScType::NodeConstNoRole);

    ScTemplate templ;
    templ.TripleWithRelation(
          item,
          ScType::EdgeDCommonVar,
          ScType::Link >> "_sys_idtf",
          ScType::EdgeAccessVarPosPerm,
          nrelSysIdtf,
          ScTemplate::TripleFlag::NotRequired);

    templ.TripleWithRelation(
          item,
          ScType::EdgeDCommonVar,
          ScType::Link >> "_main_idtf",
          ScType::EdgeAccessVarPosPerm,
          nrelMainIdtf,
          ScTemplate::TripleFlag::NotRequired);

    templ.Triple(
          ScType::NodeVar >> "_lang",
          ScType::EdgeAccessVarPosPerm,
          "_main_idtf",
          ScTemplate::TripleFlag::NotRequired);

    templ.Triple(
          languages,
          ScType::EdgeAccessVarPosPerm,
          "_lang",
          ScTemplate::TripleFlag::NotRequired);

    templ.TripleWithRelation(
          item,
          ScType::EdgeDCommonVar,
          ScType::Link >> "_image",
          ScType::EdgeAccessVarPosPerm,
          nrelImage,
          ScTemplate::TripleFlag::NotRequired);

    ScTemplateSearchResult result;
    REQUIRE(ctx.HelperSearchTemplate(templ, result));
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_optional_complex\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_optional", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_optional");

  ScAddr const addr1 = ctx.CreateNode(ScType::NodeConst);
  ScAddr const addr2 = ctx.CreateNode(ScType::NodeConst);
  ScAddr const relAddr = ctx.CreateNode(ScType::NodeConstRole);

  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);

  SECTION("not found")
  {
    SUBTEST_START("not found")
    {
      try
      {
        ScTemplate templ;

        templ.Triple(
              addr1 >> "_addr1",
              ScType::EdgeAccessVarPosPerm >> "_edge",
              addr2 >> "_addr2");

        templ.Triple(
              relAddr >> "_relAddr",
              ScType::EdgeAccessVarPosPerm,
              "_edge");

        ScTemplateSearchResult result;
        REQUIRE_FALSE(ctx.HelperSearchTemplate(templ, result));
        REQUIRE(result.IsEmpty());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"not found\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("found")
  {
    SUBTEST_START("found")
    {
      try
      {
        ScTemplate templ;

        templ.Triple(
              addr1 >> "_addr1",
              ScType::EdgeAccessVarPosPerm >> "_edge",
              addr2 >> "_addr2");

        templ.Triple(
              relAddr >> "_relAddr",
              ScType::EdgeAccessVarPosPerm >> "_relEdge",
              "_edge",
              ScTemplate::TripleFlag::NotRequired);

        ScTemplateSearchResult result;
        REQUIRE(ctx.HelperSearchTemplate(templ, result));
        REQUIRE_FALSE(result.IsEmpty());
        REQUIRE(result.Size() == 1);

        REQUIRE(result[0]["_addr1"] == addr1);
        REQUIRE(result[0]["_addr2"] == addr2);
        REQUIRE(result[0]["_edge"] == edge);

        REQUIRE(!result[0]["_relAddr"].IsValid());
        REQUIRE(!result[0]["_relEdge"].IsValid());

        REQUIRE(!result[0][3].IsValid());
        REQUIRE(!result[0][4].IsValid());
        REQUIRE(!result[0][5].IsValid());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"found\" failed")
      }
    }
    SUBTEST_END()
  }

    /*
  SUBTEST_START("fully_optional")
  {
    ScTemplate templ;

    templ.Triple(
      addr1 >> "_addr1",
      ScType::EdgeAccessVarFuzTemp >> "_edge",
      addr2 >> "_addr2",
      ScTemplate::TripleFlag::NotRequired);

    ScTemplateSearchResult result;
    bool passed = false;
    try
    {
      ctx.HelperSearchTemplate(templ, result);
    }
    catch (utils::ExceptionInvalidParams const & ex)
    {
      passed = true;
      SC_UNUSED(ex);
    }
    SC_CHECK(passed, ());
  }
  SUBTEST_END()
    */

  SECTION("generate_optional")
  {
    SUBTEST_START("generate_optional")
    {
      try
      {
        ScTemplate templ;

        templ.Triple(
              addr2,
              ScType::EdgeAccessVarPosPerm,
              addr1);

        templ.Triple(
              addr1,
              ScType::EdgeAccessVarPosPerm,
              ScType::NodeVarMaterial,
              ScTemplate::TripleFlag::NotRequired);

        ScTemplateGenResult result;
        bool passed = false;
        try
        {
          ctx.HelperGenTemplate(templ, result);
        }
        catch (utils::ExceptionInvalidParams const & ex)
        {
          passed = true;
          SC_UNUSED(ex);
        }
        REQUIRE(passed);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"generate_optional\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("optional complex")
  {
    SUBTEST_START("optional complex")
    {
      try
      {
        ScAddr const classAddr = ctx.CreateNode(ScType::NodeConstClass);
        REQUIRE(classAddr.IsValid());

        ScAddr const instAddr = ctx.CreateNode(ScType::NodeConst);
        REQUIRE(instAddr.IsValid());

        ScAddr const edgeAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, classAddr, instAddr);
        REQUIRE(edgeAddr.IsValid());

        // template without optional
        ScTemplate templNoOpt;

        templNoOpt.Triple(
              classAddr >> "_class",
              ScType::EdgeAccessVarPosPerm >> "_edge",
              ScType::NodeVar >> "_inst");

        templNoOpt.Triple(
              ScType::NodeVar,
              ScType::EdgeAccessVarPosPerm,
              "_inst");

        ScTemplateSearchResult searchResult;
        REQUIRE_FALSE(ctx.HelperSearchTemplate(templNoOpt, searchResult));
        REQUIRE(searchResult.Size() == 0);

        // with optional
        ScTemplate templOptional;

        templOptional.Triple(
              classAddr >> "_class",
              ScType::EdgeAccessVarPosPerm >> "_edge",
              ScType::NodeVar >> "_inst");

        templOptional.Triple(
              ScType::NodeVar,
              ScType::EdgeAccessVarPosPerm,
              "_inst",
              ScTemplate::TripleFlag::NotRequired);

        REQUIRE(ctx.HelperSearchTemplate(templOptional, searchResult));

        REQUIRE(searchResult.Size() == 1);
        REQUIRE(searchResult[0]["_class"] == classAddr);
        REQUIRE(searchResult[0]["_inst"] == instAddr);
        REQUIRE(searchResult[0]["_edge"] == edgeAddr);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"generate_optional\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("search_links", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_optional");

  try
  {
    ScAddr const node = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(node.IsValid());

    ScAddr const link = ctx.CreateLink();
    REQUIRE(link.IsValid());

    ScAddr const edge = ctx.CreateEdge(ScType::EdgeDCommonConst, node, link);
    REQUIRE(edge.IsValid());

    ScAddr const rel = ctx.CreateNode(ScType::NodeConstNoRole);
    REQUIRE(rel.IsValid());

    ScAddr const relEdge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, rel, edge);
    REQUIRE(relEdge.IsValid());

    ScTemplate templ;
    templ.TripleWithRelation(
          node >> "_node",
          ScType::EdgeDCommonVar >> "_edge",
          link >> "_link",
          ScType::EdgeAccessVarPosPerm >> "_edgeRel",
          rel >> "_rel");

    ScTemplateSearchResult res;
    REQUIRE(ctx.HelperSearchTemplate(templ, res));

    REQUIRE(res.Size() == 1);
    REQUIRE(node == res[0]["_node"]);
    REQUIRE(edge == res[0]["_edge"]);
    REQUIRE(link == res[0]["_link"]);
    REQUIRE(relEdge == res[0]["_edgeRel"]);
    REQUIRE(rel == res[0]["_rel"]);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"search_links\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("templates_from_scs", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "templates_common");

  SECTION("system_idtf_does_not_exist")
  {
    SUBTEST_START("system_idtf_does_not_exist")
    {
      try
      {
        ScTemplate templ;
        ScTemplate::Result result = ctx.HelperBuildTemplate(templ, "non_existing_item _-> _z;;");
        REQUIRE_FALSE(result);
        REQUIRE(result.Msg().size() > 0);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"system_idtf_does_not_exist\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_issue_295", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_issue_295");

  try
  {
    // Check search by this template
    // device_switch_multilevel _-> _x;;
    // _x _=> nrel_value::
    //      _[] (* _<= _range;; *);;

    // input data
    std::string const inData =
          "device_switch_multilevel <- sc_node_class;;"
          "nrel_value <- sc_node_norole_relation;;"
          "device_switch_multilevel -> ..x;;"
          "..x => nrel_value:"
          "  [67] (* <= ..range;; *);;";

    SCsHelper scs(ctx, std::make_shared<DummyFileInterface>());

    REQUIRE(scs.GenerateBySCsText(inData));

    // find by template
    std::string const searchSCs =
          "device_switch_multilevel _-> _x;;"
          "_x _=> nrel_value::"
          "  _[] (* _<= _range;; *);;";

    ScTemplate templ;
    REQUIRE(ctx.HelperBuildTemplate(templ, searchSCs));

    ScTemplateSearchResult searchResult;
    REQUIRE(ctx.HelperSearchTemplate(templ, searchResult));

    REQUIRE(searchResult.Size() == 1);
    auto const item = searchResult[0];

    REQUIRE(item["device_switch_multilevel"].IsValid());
    REQUIRE(item["_x"].IsValid());
    REQUIRE(item["nrel_value"].IsValid());
    REQUIRE(item["_range"].IsValid());

    REQUIRE(ctx.GetElementType(item["device_switch_multilevel"]) == ScType::NodeConstClass);
    REQUIRE(ctx.GetElementType(item["nrel_value"]) == ScType::NodeConstNoRole);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_issue_295\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("template_a_a_a_a_f", "[test templates]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "template_a_a_a_a_f");

  try
  {
    SCsHelper scs(ctx, std::make_shared<DummyFileInterface>());
    REQUIRE(scs.GenerateBySCsText("x => nrel: [];;"));

    ScAddr const nrelAddr = ctx.HelperResolveSystemIdtf("nrel");
    REQUIRE(nrelAddr.IsValid());

    ScAddr const xAddr = ctx.HelperResolveSystemIdtf("x");
    REQUIRE(xAddr.IsValid());

    ScTemplate templ;
    templ.TripleWithRelation(
          ScType::Unknown >> "_x",
          ScType::EdgeDCommonVar,
          ScType::Link,
          ScType::EdgeAccessVarPosPerm,
          nrelAddr);

    ScTemplateSearchResult res;
    REQUIRE(ctx.HelperSearchTemplate(templ, res));

    REQUIRE(res.Size() == 1);
    REQUIRE(res[0]["_x"] == xAddr);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"template_a_a_a_a_f\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
