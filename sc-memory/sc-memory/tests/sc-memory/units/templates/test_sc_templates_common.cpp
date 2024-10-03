/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_structure.hpp>

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
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const addr3 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const arc1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr1, addr2);
  EXPECT_TRUE(arc1.IsValid());

  ScAddr const arc2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr3, arc1);
  EXPECT_TRUE(arc2.IsValid());

  ScTemplate templ;

  templ.Triple(addr1 >> "addr1", ScType::VarPermPosArc >> "arc1", ScType::VarNode >> "addr2");
  templ.Triple(ScType::VarNode >> "_addr1T2", ScType::VarPermPosArc >> "_addr2T2", "arc1");
  templ.Triple("addr2", ScType::VarCommonArc >> "_addr2T3", "arc1");

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);

  ScIterator5Ptr const it5 =
      m_ctx->CreateIterator5(addr1, ScType::ConstPermPosArc, ScType::Node, ScType::ConstPermPosArc, ScType::Node);

  EXPECT_TRUE(it5->Next());
  EXPECT_EQ(it5->Get(0), result["addr1"]);
  EXPECT_EQ(it5->Get(1), result["arc1"]);
  EXPECT_EQ(it5->Get(2), result["addr2"]);
  EXPECT_EQ(it5->Get(3), result["_addr2T2"]);
  EXPECT_EQ(it5->Get(4), result["_addr1T2"]);

  ScIterator3Ptr const it3 = m_ctx->CreateIterator3(result["addr2"], ScType::CommonArc, ScType::ConstPermPosArc);

  EXPECT_TRUE(it3->Next());
  EXPECT_EQ(it3->Get(0), result["addr2"]);
  EXPECT_EQ(it3->Get(1), result["_addr2T3"]);
  EXPECT_EQ(it3->Get(2), result["arc1"]);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);

  ScTemplateSearchResultItem res = searchResult[0];

  EXPECT_EQ(it5->Get(0), res["addr1"]);
  EXPECT_EQ(it5->Get(1), res["arc1"]);
  EXPECT_EQ(it5->Get(2), res["addr2"]);
  EXPECT_EQ(it5->Get(3), result["_addr2T2"]);
  EXPECT_EQ(it5->Get(4), result["_addr1T2"]);

  EXPECT_EQ(it3->Get(0), res["addr2"]);
  EXPECT_EQ(it3->Get(1), result["_addr2T3"]);
  EXPECT_EQ(it3->Get(2), res["arc1"]);
}

TEST_F(ScTemplateCommonTest, search)
{
  size_t const testCount = 10;
  ScAddrVector nodes, connectors;

  ScAddr const addrSrc = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addrSrc.IsValid());
  for (size_t i = 0; i < testCount; ++i)
  {
    ScAddr const addrTrg = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(addrTrg.IsValid());

    ScAddr const addrEdge = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addrSrc, addrTrg);
    EXPECT_TRUE(addrEdge.IsValid());

    nodes.push_back(addrTrg);
    connectors.push_back(addrEdge);
  }

  ScTemplate templ;
  templ.Triple(addrSrc >> "addrSrc", ScType::VarPermPosArc >> "arcAddr", ScType::VarNode >> "addrTrg");

  ScTemplateSearchResult result;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));

  size_t const count = result.Size();
  for (size_t i = 0; i < count; ++i)
  {
    ScTemplateSearchResultItem const & r = result[i];

    EXPECT_EQ(r["addrSrc"], addrSrc);

    EXPECT_TRUE(HasAddr(connectors, r["arcAddr"]));
    EXPECT_TRUE(HasAddr(nodes, r["addrTrg"]));
  }
}

TEST_F(ScTemplateCommonTest, searchQuintuple)
{
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr1.IsValid());

  ScAddr const addr2 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr2.IsValid());

  ScAddr const addr3 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr3.IsValid());

  ScAddr const arc1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr1, addr2);
  EXPECT_TRUE(arc1.IsValid());

  ScAddr const arc2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr3, arc1);
  EXPECT_TRUE(arc2.IsValid());

  {
    ScTemplate templ;
    templ.Quintuple(addr1, ScType::VarPermPosArc, ScType::VarNode, ScType::VarPermPosArc, addr3);

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
    EXPECT_EQ(result.Size(), 1u);
  }

  {
    ScTemplate templ;

    templ.Quintuple(
        addr1 >> "1", ScType::VarPermPosArc >> "2", ScType::VarNode >> "3", ScType::VarPermPosArc >> "4", addr3 >> "5");

    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
    EXPECT_EQ(result.Size(), 1u);
  }
}

TEST_F(ScTemplateCommonTest, params_correct)
{
  ScAddr const addrConst = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const addrTest3 = m_ctx->GenerateNode(ScType::ConstNodeTuple);
  ScAddr const addrTest6 = m_ctx->GenerateNode(ScType::ConstNodeClass);

  ScTemplate templ;
  templ.Triple(addrConst >> "1", ScType::VarPermPosArc >> "_2", ScType::VarNodeTuple >> "_3");
  templ.Triple("_3", ScType::VarPermPosArc >> "_5", ScType::VarNodeClass >> "_6");

  ScTemplateParams params;
  params.Add("_3", addrTest3).Add("_6", addrTest6);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, params);

  ScTemplate searchTempl;
  searchTempl.Triple(addrConst >> "1", ScType::VarPermPosArc >> "_2", ScType::VarNodeTuple >> "_3");
  searchTempl.Triple("_3", ScType::VarPermPosArc >> "_5", ScType::VarNodeClass >> "_6");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_3"], addrTest3);
  EXPECT_EQ(searchResult[0]["_6"], addrTest6);
}

TEST_F(ScTemplateCommonTest, params_invalid)
{
  ScAddr const addrConst = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const addrTest3 = m_ctx->GenerateNode(ScType::ConstNodeTuple);
  ScAddr const addrEdge2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addrConst, addrTest3);

  ScTemplate templ;
  templ.Triple(
      addrConst >> "1",
      ScType::VarPermPosArc >> "_2",
      ScType::VarNode >> "_3");  // can't be replaced by param in template generation

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);

  // test arcAddr
  {
    ScTemplateParams params;
    params.Add("_2", addrEdge2);

    m_ctx->GenerateByTemplate(templ, result, params);
  }

  // test invalid params
  {
    // target is const
    {
      EXPECT_EQ(
          false, TestTemplParams(*m_ctx)(addrConst >> "1", ScType::VarPermPosArc >> "_2", ScType::ConstNode >> "_3"));
    }

    // source is const
    {
      EXPECT_EQ(
          false, TestTemplParams(*m_ctx)(ScType::ConstNode >> "_1", ScType::VarPermPosArc >> "_2", addrConst >> "3"));
    }

    // arcAddr is const
    {
      EXPECT_EQ(
          false, TestTemplParams(*m_ctx)(ScType::VarNode >> "_1", ScType::ConstPermPosArc >> "_2", addrConst >> "3"));
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

  ScAddr const _structAddr = m_ctx->GenerateNode(ScType::VarNodeStructure);
  EXPECT_TRUE(_structAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_struct", _structAddr));

  ScAddr const _apiai_locationAddr = m_ctx->GenerateNode(ScType::VarNode);
  EXPECT_TRUE(_apiai_locationAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_apiai_location", _apiai_locationAddr));

  ScAddr const _apiai_speechAddr = m_ctx->GenerateNode(ScType::VarNode);
  EXPECT_TRUE(_apiai_speechAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_apiai_speech", _apiai_speechAddr));

  ScAddr const _langAddr = m_ctx->GenerateNode(ScType::VarNode);
  EXPECT_TRUE(_langAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_lang", _langAddr));

  ScAddr const rrel_locationAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(rrel_locationAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("rrel_location", rrel_locationAddr));

  ScAddr const nrel_translationAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(nrel_translationAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("nrel_translation", nrel_translationAddr));

  ScAddr const _struct_locationEdgeAddr =
      m_ctx->GenerateConnector(ScType::VarPermPosArc, _structAddr, _apiai_locationAddr);
  EXPECT_TRUE(_struct_locationEdgeAddr.IsValid());

  ScAddr const _rrel_locationEdgeAddr =
      m_ctx->GenerateConnector(ScType::VarPermPosArc, rrel_locationAddr, _struct_locationEdgeAddr);
  EXPECT_TRUE(_rrel_locationEdgeAddr.IsValid());

  ScAddr const _struct_speechEdgeAddr = m_ctx->GenerateConnector(ScType::VarCommonArc, _structAddr, _apiai_speechAddr);
  EXPECT_TRUE(_struct_speechEdgeAddr.IsValid());

  ScAddr const _nrel_translationEdgeAddr =
      m_ctx->GenerateConnector(ScType::VarPermPosArc, nrel_translationAddr, _struct_speechEdgeAddr);
  EXPECT_TRUE(_nrel_translationEdgeAddr.IsValid());

  ScAddr const _langEdgeAddr = m_ctx->GenerateConnector(ScType::VarPermPosArc, _langAddr, _apiai_speechAddr);
  EXPECT_TRUE(_langEdgeAddr.IsValid());

  // create template
  ScAddr const templStructAddr = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(templStructAddr.IsValid());
  ScStructure templStruct = m_ctx->ConvertToStructure(templStructAddr);

  templStruct << _structAddr << _apiai_locationAddr << _apiai_speechAddr << _langAddr << rrel_locationAddr
              << nrel_translationAddr << _struct_locationEdgeAddr << _rrel_locationEdgeAddr << _struct_speechEdgeAddr
              << _nrel_translationEdgeAddr << _langEdgeAddr;

  ScTemplate templ;
  m_ctx->BuildTemplate(templ, templStructAddr);

  // check creation by this template
  {
    ScTemplateGenResult result;
    m_ctx->GenerateByTemplate(templ, result);
  }

  // check search by this template
  {
    ScTemplateSearchResult result;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));
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
  templ.Quintuple(ScType::Unknown >> "_x", ScType::VarCommonArc, ScType::NodeLink, ScType::VarPermPosArc, nrelAddr);

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));

  EXPECT_EQ(res.Size(), 1u);
  EXPECT_EQ(res[0]["_x"], xAddr);
}

// TODO: Optimize sc-template search with very-very big templates
TEST_F(ScTemplateCommonTest, DISABLED_BigTemplateSmoke)
{
  ScAddr const set1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const rel = m_ctx->GenerateNode(ScType::ConstNodeNoRole);

  static size_t const el_num = 1 << 12;
  ScAddrSet elements;
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(a.IsValid());
    elements.insert(a);
  }

  // create template for pending events check
  ScTemplate templ;
  for (auto const & a : elements)
  {
    templ.Quintuple(set1, ScType::VarCommonArc, a >> "_el", ScType::VarPermPosArc, rel);
  }

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);

  // ensure whole data created correctly
  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));

  for (size_t i = 0; i < searchResult.Size(); ++i)
    EXPECT_TRUE(elements.find(searchResult[i]["_el"]) != elements.end());
}

TEST_F(ScTemplateCommonTest, CycledTemplateSmoke)
{
  ScAddr const & classAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, classAddr, sourceNodeAddr);

  size_t const constrCount = 5;
  size_t const tripleCount = 20;
  for (size_t i = 0; i < constrCount; ++i)
  {
    ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

    ScTemplate genTempl;
    genTempl.Triple(sourceNodeAddr, ScType::VarPermPosArc, targetNodeAddr);

    for (size_t j = 0; j < tripleCount; ++j)
    {
      ScTemplateGenResult result;
      m_ctx->GenerateByTemplate(genTempl, result);
    }
  }

  ScTemplate searchTempl;
  searchTempl.Triple(classAddr, ScType::VarPermPosArc, ScType::VarNode >> "_source");
  searchTempl.Triple("_source", ScType::VarPermPosArc, ScType::VarNode >> "_target");
  for (size_t i = 0; i < tripleCount - 1; ++i)
  {
    searchTempl.Triple("_source", ScType::VarPermPosArc, "_target");
  }

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), constrCount);
}

TEST_F(ScTemplateCommonTest, MultipleConnectivitiesTemplateSmoke)
{
  size_t const constrCount = 20;
  std::map<ScAddr, ScAddr, ScAddrLessFunc> cache;
  for (size_t i = 0; i < constrCount; ++i)
  {
    ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

    ScTemplate genTempl;
    genTempl.Triple(sourceNodeAddr, ScType::VarPermPosArc, targetNodeAddr);
    ScTemplateGenResult result;
    m_ctx->GenerateByTemplate(genTempl, result);

    cache.insert({sourceNodeAddr, targetNodeAddr});
  }

  ScTemplate searchTempl;
  for (auto it : cache)
  {
    searchTempl.Triple(it.first, ScType::VarPermPosArc, it.second);
  }

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}

TEST_F(ScTemplateCommonTest, ConnectorsTemplateSmoke)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & targetNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScTemplate genTempl;
  genTempl.Triple(sourceNodeAddr, ScType::VarCommonEdge, targetNodeAddr);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(genTempl, result);

  ScTemplate searchTempl;
  searchTempl.Triple(sourceNodeAddr, ScType::VarCommonEdge, ScType::VarNode >> "_target");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_target"], targetNodeAddr);

  searchTempl.Clear();
  searchTempl.Triple(targetNodeAddr, ScType::VarCommonEdge, ScType::VarNode >> "_target");

  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_target"], sourceNodeAddr);
}

TEST_F(ScTemplateCommonTest, CycledConnectorsTemplateSmoke)
{
  ScAddr const & sourceNodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  ScTemplate genTempl;
  genTempl.Triple(sourceNodeAddr, ScType::VarCommonEdge, sourceNodeAddr);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(genTempl, result);

  ScTemplate searchTempl;
  searchTempl.Triple(sourceNodeAddr, ScType::VarCommonEdge, ScType::VarNode >> "_target");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_target"], sourceNodeAddr);

  searchTempl.Clear();
  searchTempl.Triple(ScType::VarNode >> "_source", ScType::VarCommonEdge, sourceNodeAddr);

  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_source"], sourceNodeAddr);
}
