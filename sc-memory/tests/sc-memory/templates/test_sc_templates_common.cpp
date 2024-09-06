#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_structure.hpp"

#include "template_test_utils.hpp"

TEST(ScTemplateResultTest, ScTemplateResult)
{
  {
    ScTemplate::Result result(true);

    EXPECT_TRUE(result);
    EXPECT_EQ(result.Msg(), "");
  }

  {
    ScTemplate::Result result(false, "msg");

    EXPECT_FALSE(result);
    EXPECT_EQ(result.Msg(), "msg");
  }
}

using ScTemplateCommonTest = ScTemplateTest;

TEST_F(ScTemplateCommonTest, smoke)
{
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const addr3 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());

  ScAddr const edge2 = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());

  ScTemplate templ;

  templ.Triple(addr1 >> "addr1", ScType::EdgeAccessVarPosPerm >> "edge1", ScType::NodeVar >> "addr2");
  templ.Triple(ScType::NodeVar >> "_addr1T2", ScType::EdgeAccessVarPosPerm >> "_addr2T2", "edge1");
  templ.Triple("addr2", ScType::EdgeDCommonVar >> "_addr2T3", "edge1");

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));

  ScIterator5Ptr const it5 = m_ctx->Iterator5(
      addr1, ScType::EdgeAccessConstPosPerm, ScType::Node, ScType::EdgeAccessConstPosPerm, ScType::Node);

  EXPECT_TRUE(it5->Next());
  EXPECT_EQ(it5->Get(0), result["addr1"]);
  EXPECT_EQ(it5->Get(1), result["edge1"]);
  EXPECT_EQ(it5->Get(2), result["addr2"]);
  EXPECT_EQ(it5->Get(3), result["_addr2T2"]);
  EXPECT_EQ(it5->Get(4), result["_addr1T2"]);

  ScIterator3Ptr const it3 = m_ctx->Iterator3(result["addr2"], ScType::EdgeDCommon, ScType::EdgeAccessConstPosPerm);

  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), result["addr2"]);
  EXPECT_EQ(it3->Get(1), result["_addr2T3"]);
  EXPECT_EQ(it3->Get(2), result["edge1"]);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);

  ScTemplateSearchResultItem res = searchResult[0];

  EXPECT_EQ(it5->Get(0), res["addr1"]);
  EXPECT_EQ(it5->Get(1), res["edge1"]);
  EXPECT_EQ(it5->Get(2), res["addr2"]);
  EXPECT_EQ(it5->Get(3), result["_addr2T2"]);
  EXPECT_EQ(it5->Get(4), result["_addr1T2"]);

  EXPECT_EQ(it3->Get(0), res["addr2"]);
  EXPECT_EQ(it3->Get(1), result["_addr2T3"]);
  EXPECT_EQ(it3->Get(2), res["edge1"]);
}

TEST_F(ScTemplateCommonTest, search)
{
  size_t const testCount = 10;
  ScAddrVector nodes, edges;

  ScAddr const addrSrc = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addrSrc.IsValid());
  for (size_t i = 0; i < testCount; ++i)
  {
    ScAddr const addrTrg = m_ctx->GenerateNode(ScType::NodeConst);
    EXPECT_TRUE(addrTrg.IsValid());

    ScAddr const addrEdge = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, addrSrc, addrTrg);
    EXPECT_TRUE(addrEdge.IsValid());

    nodes.push_back(addrTrg);
    edges.push_back(addrEdge);
  }

  ScTemplate templ;
  templ.Triple(addrSrc >> "addrSrc", ScType::EdgeAccessVarPosPerm >> "arcAddr", ScType::NodeVar >> "addrTrg");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));

  size_t const count = result.Size();
  for (size_t i = 0; i < count; ++i)
  {
    ScTemplateSearchResultItem const & r = result[i];

    EXPECT_EQ(r["addrSrc"], addrSrc);

    EXPECT_TRUE(HasAddr(edges, r["arcAddr"]));
    EXPECT_TRUE(HasAddr(nodes, r["addrTrg"]));
  }
}

TEST_F(ScTemplateCommonTest, searchQuintuple)
{
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const addr3 = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const edge1 = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge1.IsValid());

  ScAddr const edge2 = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, addr3, edge1);
  EXPECT_TRUE(edge2.IsValid());

  {
    ScTemplate templ;
    templ.Quintuple(addr1, ScType::EdgeAccessVarPosPerm, ScType::NodeVar, ScType::EdgeAccessVarPosPerm, addr3);

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
    EXPECT_EQ(result.Size(), 1u);
  }

  {
    ScTemplate templ;

    templ.Quintuple(
        addr1 >> "1",
        ScType::EdgeAccessVarPosPerm >> "2",
        ScType::NodeVar >> "3",
        ScType::EdgeAccessVarPosPerm >> "4",
        addr3 >> "5");

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
    EXPECT_EQ(result.Size(), 1u);
  }
}

TEST_F(ScTemplateCommonTest, params_correct)
{
  ScAddr const addrConst = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const addrTest3 = m_ctx->GenerateNode(ScType::NodeConstTuple);
  ScAddr const addrTest6 = m_ctx->GenerateNode(ScType::NodeConstClass);

  ScTemplate templ;
  templ.Triple(addrConst >> "1", ScType::EdgeAccessVarPosPerm >> "_2", ScType::NodeVarTuple >> "_3");
  templ.Triple("_3", ScType::EdgeAccessVarPosPerm >> "_5", ScType::NodeVarClass >> "_6");

  ScTemplateParams params;
  params.Add("_3", addrTest3).Add("_6", addrTest6);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, params));

  ScTemplate searchTempl;
  searchTempl.Triple(addrConst >> "1", ScType::EdgeAccessVarPosPerm >> "_2", ScType::NodeVarTuple >> "_3");
  searchTempl.Triple("_3", ScType::EdgeAccessVarPosPerm >> "_5", ScType::NodeVarClass >> "_6");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_3"], addrTest3);
  EXPECT_EQ(searchResult[0]["_6"], addrTest6);
}

TEST_F(ScTemplateCommonTest, params_invalid)
{
  ScAddr const addrConst = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const addrTest3 = m_ctx->GenerateNode(ScType::NodeConstTuple);
  ScAddr const addrEdge2 = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, addrConst, addrTest3);

  ScTemplate templ;
  templ.Triple(
      addrConst >> "1",
      ScType::EdgeAccessVarPosPerm >> "_2",
      ScType::NodeVar >> "_3");  // can't be replaced by param in template generation

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));

  // test arcAddr
  {
    ScTemplateParams params;
    params.Add("_2", addrEdge2);

    EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, params));
  }

  // test invalid params
  {
    // target is const
    {
      EXPECT_EQ(
          false,
          TestTemplParams(*m_ctx)(addrConst >> "1", ScType::EdgeAccessVarPosPerm >> "_2", ScType::NodeConst >> "_3"));
    }

    // source is const
    {
      EXPECT_EQ(
          false,
          TestTemplParams(*m_ctx)(ScType::NodeConst >> "_1", ScType::EdgeAccessVarPosPerm >> "_2", addrConst >> "3"));
    }

    // arcAddr is const
    {
      EXPECT_EQ(
          false,
          TestTemplParams(*m_ctx)(ScType::NodeVar >> "_1", ScType::EdgeAccessConstPosPerm >> "_2", addrConst >> "3"));
    }
  }
}

TEST_F(ScTemplateCommonTest, a_a_a)
{
  /**
   * _struct
   * _-> rrel_location:: _apiai_location;
   * _=> nrel_translation:: _apiai_speech
   *       (* _<- _lang;; *);;
   */

  ScAddr const _structAddr = m_ctx->GenerateNode(ScType::NodeVarStruct);
  EXPECT_TRUE(_structAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_struct", _structAddr));

  ScAddr const _apiai_locationAddr = m_ctx->GenerateNode(ScType::NodeVar);
  EXPECT_TRUE(_apiai_locationAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_apiai_location", _apiai_locationAddr));

  ScAddr const _apiai_speechAddr = m_ctx->GenerateNode(ScType::NodeVar);
  EXPECT_TRUE(_apiai_speechAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_apiai_speech", _apiai_speechAddr));

  ScAddr const _langAddr = m_ctx->GenerateNode(ScType::NodeVar);
  EXPECT_TRUE(_langAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_lang", _langAddr));

  ScAddr const rrel_locationAddr = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(rrel_locationAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("rrel_location", rrel_locationAddr));

  ScAddr const nrel_translationAddr = m_ctx->GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(nrel_translationAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("nrel_translation", nrel_translationAddr));

  ScAddr const _struct_locationEdgeAddr =
      m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, _structAddr, _apiai_locationAddr);
  EXPECT_TRUE(_struct_locationEdgeAddr.IsValid());

  ScAddr const _rrel_locationEdgeAddr =
      m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, rrel_locationAddr, _struct_locationEdgeAddr);
  EXPECT_TRUE(_rrel_locationEdgeAddr.IsValid());

  ScAddr const _struct_speechEdgeAddr =
      m_ctx->GenerateConnector(ScType::EdgeDCommonVar, _structAddr, _apiai_speechAddr);
  EXPECT_TRUE(_struct_speechEdgeAddr.IsValid());

  ScAddr const _nrel_translationEdgeAddr =
      m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, nrel_translationAddr, _struct_speechEdgeAddr);
  EXPECT_TRUE(_nrel_translationEdgeAddr.IsValid());

  ScAddr const _langEdgeAddr = m_ctx->GenerateConnector(ScType::EdgeAccessVarPosPerm, _langAddr, _apiai_speechAddr);
  EXPECT_TRUE(_langEdgeAddr.IsValid());

  // create template
  ScAddr const templStructAddr = m_ctx->GenerateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(templStructAddr.IsValid());
  ScStructure templStruct = m_ctx->ConvertToStructure(templStructAddr);

  templStruct << _structAddr << _apiai_locationAddr << _apiai_speechAddr << _langAddr << rrel_locationAddr
              << nrel_translationAddr << _struct_locationEdgeAddr << _rrel_locationEdgeAddr << _struct_speechEdgeAddr
              << _nrel_translationEdgeAddr << _langEdgeAddr;

  ScTemplate templ;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, templStructAddr));

  // check creation by this template
  {
    ScTemplateGenResult result;
    EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));
  }

  // check search by this template
  {
    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));
  }
}

TEST_F(ScTemplateCommonTest, a_a_a_a_f)
{
  SCsHelper scs(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(scs.GenerateBySCsText("x => nrel: [];;"));

  ScAddr const nrelAddr = m_ctx->ResolveElementSystemIdentifier("nrel");
  EXPECT_TRUE(nrelAddr.IsValid());

  ScAddr const xAddr = m_ctx->ResolveElementSystemIdentifier("x");
  EXPECT_TRUE(xAddr.IsValid());

  ScTemplate templ;
  templ.Quintuple(
      ScType::Unknown >> "_x", ScType::EdgeDCommonVar, ScType::Link, ScType::EdgeAccessVarPosPerm, nrelAddr);

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, res));

  EXPECT_EQ(res.Size(), 1u);
  EXPECT_EQ(res[0]["_x"], xAddr);
}

// TODO: Optimize sc-template search with very-very big templates
TEST_F(ScTemplateCommonTest, DISABLED_BigTemplateSmoke)
{
  ScAddr const set1 = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const rel = m_ctx->GenerateNode(ScType::NodeConstNoRole);

  static size_t const el_num = 1 << 12;
  ScAddrSet elements;
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = m_ctx->GenerateNode(ScType::NodeConst);
    EXPECT_TRUE(a.IsValid());
    elements.insert(a);
  }

  // create template for pending events check
  ScTemplate templ;
  for (auto const & a : elements)
  {
    templ.Quintuple(set1, ScType::EdgeDCommonVar, a >> "_el", ScType::EdgeAccessVarPosPerm, rel);
  }

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));

  // ensure whole data created correctly
  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));

  for (size_t i = 0; i < searchResult.Size(); ++i)
    EXPECT_TRUE(elements.find(searchResult[i]["_el"]) != elements.end());
}

TEST_F(ScTemplateCommonTest, CycledTemplateSmoke)
{
  ScAddr const & classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);
  m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, sourceNodeAddr);

  size_t const constrCount = 5;
  size_t const tripleCount = 20;
  for (size_t i = 0; i < constrCount; ++i)
  {
    ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

    ScTemplate genTempl;
    genTempl.Triple(sourceNodeAddr, ScType::EdgeAccessVarPosPerm, targetNodeAddr);

    for (size_t j = 0; j < tripleCount; ++j)
    {
      ScTemplateGenResult result;
      EXPECT_TRUE(m_ctx->HelperGenTemplate(genTempl, result));
    }
  }

  ScTemplate searchTempl;
  searchTempl.Triple(classAddr, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_source");
  searchTempl.Triple("_source", ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_target");
  for (size_t i = 0; i < tripleCount - 1; ++i)
  {
    searchTempl.Triple("_source", ScType::EdgeAccessVarPosPerm, "_target");
  }

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), constrCount);
}

TEST_F(ScTemplateCommonTest, MultipleConnectivitiesTemplateSmoke)
{
  size_t const constrCount = 20;
  std::map<ScAddr, ScAddr, ScAddrLessFunc> cache;
  for (size_t i = 0; i < constrCount; ++i)
  {
    ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);
    ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

    ScTemplate genTempl;
    genTempl.Triple(sourceNodeAddr, ScType::EdgeAccessVarPosPerm, targetNodeAddr);
    ScTemplateGenResult result;
    m_ctx->HelperGenTemplate(genTempl, result);

    cache.insert({sourceNodeAddr, targetNodeAddr});
  }

  ScTemplate searchTempl;
  for (auto it : cache)
  {
    searchTempl.Triple(it.first, ScType::EdgeAccessVarPosPerm, it.second);
  }

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}

TEST_F(ScTemplateCommonTest, EdgesTemplateSmoke)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScTemplate genTempl;
  genTempl.Triple(sourceNodeAddr, ScType::EdgeUCommonVar, targetNodeAddr);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(genTempl, result));

  ScTemplate searchTempl;
  searchTempl.Triple(sourceNodeAddr, ScType::EdgeUCommonVar, ScType::NodeVar >> "_target");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_target"], targetNodeAddr);

  searchTempl.Clear();
  searchTempl.Triple(targetNodeAddr, ScType::EdgeUCommonVar, ScType::NodeVar >> "_target");

  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_target"], sourceNodeAddr);
}

TEST_F(ScTemplateCommonTest, CycledEdgesTemplateSmoke)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

  ScTemplate genTempl;
  genTempl.Triple(sourceNodeAddr, ScType::EdgeUCommonVar, sourceNodeAddr);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(genTempl, result));

  ScTemplate searchTempl;
  searchTempl.Triple(sourceNodeAddr, ScType::EdgeUCommonVar, ScType::NodeVar >> "_target");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_target"], sourceNodeAddr);

  searchTempl.Clear();
  searchTempl.Triple(ScType::NodeVar >> "_source", ScType::EdgeUCommonVar, sourceNodeAddr);

  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_source"], sourceNodeAddr);
}
