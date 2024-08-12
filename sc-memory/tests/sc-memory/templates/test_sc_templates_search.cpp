#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "template_test_utils.hpp"

using ScTemplateSearchTest = ScTemplateTest;

TEST_F(ScTemplateSearchTest, SimpleSearch1)
{
  /**			_y
   *			^
   *			|
   *			| <----- _z
   *			|
   *			x <----- _s
   *
   * scs: x _-> _z:: _y;; _s _-> x;;
   */
  ScAddr const templateAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(templateAddr.IsValid());

  ScStruct templStruct = m_ctx->UseStructure(templateAddr);
  ScAddr xAddr;
  {
    ScAddr _yAddr, _zAddr, _sAddr;

    xAddr = m_ctx->CreateNode(ScType::NodeConstMaterial);
    EXPECT_TRUE(xAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("x", xAddr));

    _yAddr = m_ctx->CreateNode(ScType::Var);
    EXPECT_TRUE(_yAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_y", _yAddr));

    _zAddr = m_ctx->CreateNode(ScType::NodeVarRole);
    EXPECT_TRUE(_zAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_z", _zAddr));

    _sAddr = m_ctx->CreateNode(ScType::NodeVarClass);
    EXPECT_TRUE(_sAddr.IsValid());
    EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("_s", _sAddr));

    ScAddr const xyAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, xAddr, _yAddr);
    EXPECT_TRUE(xyAddr.IsValid());
    ScAddr const zxyAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _zAddr, xyAddr);
    EXPECT_TRUE(zxyAddr.IsValid());
    ScAddr const sxAddr = m_ctx->CreateEdge(ScType::EdgeAccessVarPosPerm, _sAddr, xAddr);
    EXPECT_TRUE(sxAddr.IsValid());

    // append created elements into struct
    templStruct << xAddr << _yAddr << _zAddr << xyAddr << zxyAddr << _sAddr << sxAddr;
  }

  ScTemplate templ;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, templateAddr));

  // create test structure that correspond to template
  {
    ScAddr const yAddr = m_ctx->CreateNode(ScType::Const);
    EXPECT_TRUE(yAddr.IsValid());

    ScAddr const zAddr = m_ctx->CreateNode(ScType::NodeConstRole);
    EXPECT_TRUE(zAddr.IsValid());

    ScAddr const sAddr = m_ctx->CreateNode(ScType::NodeConstClass);
    EXPECT_TRUE(sAddr.IsValid());

    ScAddr const xyAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, xAddr, yAddr);
    EXPECT_TRUE(xyAddr.IsValid());
    ScAddr const zxyAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, zAddr, xyAddr);
    EXPECT_TRUE(zxyAddr.IsValid());
    ScAddr const sxAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, sAddr, xAddr);
    EXPECT_TRUE(sxAddr.IsValid());

    // test search by template
    {
      ScTemplateSearchResult result;
      EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, result));

      EXPECT_EQ(result.Size(), 1u);
      ScTemplateSearchResultItem const item = result[0];

      EXPECT_EQ(item["x"], xAddr);
      EXPECT_EQ(item["_y"], yAddr);
      EXPECT_EQ(item["_z"], zAddr);
      EXPECT_EQ(item["_s"], sAddr);
    }
  }
}

TEST_F(ScTemplateSearchTest, SimpleSearch2)
{
  /** SCs:
   * addr => nrel_main_idtf: [] (* <- lang;; *);;
   */
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr.IsValid());
  ScAddr const nrelMainIdtf = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelMainIdtf.IsValid());
  ScAddr const lang = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(lang.IsValid());
  ScAddr const link = m_ctx->CreateLink();
  EXPECT_TRUE(link.IsValid());

  ScAddr const edgeCommon = m_ctx->CreateEdge(ScType::EdgeDCommonConst, addr, link);
  EXPECT_TRUE(edgeCommon.IsValid());
  ScAddr const edgeAttr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelMainIdtf, edgeCommon);
  EXPECT_TRUE(edgeAttr.IsValid());
  ScAddr const edgeLang = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, lang, link);
  EXPECT_TRUE(edgeLang.IsValid());

  // now check search
  ScTemplate templ;
  templ.Quintuple(
      addr >> "_addr",
      ScType::EdgeDCommonVar >> "_edgeCommon",
      ScType::Link >> "_link",
      ScType::EdgeAccessVarPosPerm >> "_edgeAttr",
      nrelMainIdtf >> "_nrelMainIdtf");

  templ.Triple(lang >> "_lang", ScType::EdgeAccessVarPosPerm >> "_edgeLang", "_link");

  // search
  {
    ScTemplateSearchResult res;
    EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, res));

    EXPECT_EQ(res.Size(), 1u);
    EXPECT_EQ(res[0]["_addr"], addr);
    EXPECT_EQ(res[0]["_edgeCommon"], edgeCommon);
    EXPECT_EQ(res[0]["_link"], link);
    EXPECT_EQ(res[0]["_edgeAttr"], edgeAttr);
    EXPECT_EQ(res[0]["_nrelMainIdtf"], nrelMainIdtf);
    EXPECT_EQ(res[0]["_lang"], lang);
    EXPECT_EQ(res[0]["_edgeLang"], edgeLang);
  }
}

TEST_F(ScTemplateSearchTest, UnknownType)
{
  // addr1 -> addr2;;
  ScAddr const addr1 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->CreateNode(ScType::NodeConstAbstract);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);
  EXPECT_TRUE(edge.IsValid());

  ScTemplate templ;
  templ.Triple(addr1, ScType::EdgeAccessVarPosPerm >> "edge", ScType::Unknown >> "addr2");

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, res));
  EXPECT_EQ(res.Size(), 1u);

  EXPECT_EQ(res[0]["edge"], edge);
  EXPECT_EQ(res[0]["addr2"], addr2);
}

TEST_F(ScTemplateSearchTest, LinkWithRelation)
{
  /**
   * deviceAddr _<= nrel_installed_apps: _tuple;;
   * _tuple _-> _app;
   *    _app => nrel_idtf: _idtf;;
   *    _app => nrel_image: _image;;
   */

  ScAddr const deviceAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(deviceAddr.IsValid());

  ScAddr const nrelInstalledApp = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelInstalledApp.IsValid());

  ScAddr const _tuple = m_ctx->CreateNode(ScType::NodeConstTuple);
  EXPECT_TRUE(_tuple.IsValid());

  ScAddr const nrelIdtf = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelIdtf.IsValid());

  ScAddr const nrelImage = m_ctx->CreateNode(ScType::NodeConstNoRole);
  EXPECT_TRUE(nrelImage.IsValid());

  ScAddr edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, _tuple, deviceAddr);
  EXPECT_TRUE(edge.IsValid());

  edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelInstalledApp, edge);
  EXPECT_TRUE(edge.IsValid());

  struct TestData
  {
    ScAddr m_app;
    ScAddr m_idtf;
    ScAddr m_image;
  };

  size_t i = 0;
  std::vector<TestData> data(50);
  for (auto & d : data)
  {
    d.m_app = m_ctx->CreateNode(ScType::NodeConstAbstract);
    EXPECT_TRUE(d.m_app.IsValid());

    edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, _tuple, d.m_app);
    EXPECT_TRUE(edge.IsValid());

    d.m_idtf = m_ctx->CreateLink();
    EXPECT_TRUE(d.m_idtf.IsValid());

    ScLink idtfLink(*m_ctx, d.m_idtf);
    EXPECT_TRUE(idtfLink.Set("idtf_" + std::to_string(i)));

    edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, d.m_app, d.m_idtf);
    EXPECT_TRUE(edge.IsValid());

    edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelIdtf, edge);
    EXPECT_TRUE(edge.IsValid());

    d.m_image = m_ctx->CreateLink();
    EXPECT_TRUE(d.m_image.IsValid());

    ScLink imageLink(*m_ctx, d.m_image);
    EXPECT_TRUE(imageLink.Set("data_" + std::to_string(i)));

    edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, d.m_app, d.m_image);
    EXPECT_TRUE(edge.IsValid());

    edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nrelImage, edge);
    EXPECT_TRUE(edge.IsValid());

    ++i;
  }

  ScTemplate templ;
  templ.Quintuple(
      ScType::NodeVarTuple >> "_tuple",
      ScType::EdgeDCommonVar,
      deviceAddr,
      ScType::EdgeAccessVarPosPerm,
      nrelInstalledApp);

  templ.Triple("_tuple", ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_app");

  templ.Quintuple("_app", ScType::EdgeDCommonVar, ScType::Link >> "_idtf", ScType::EdgeAccessVarPosPerm, nrelIdtf);

  templ.Quintuple("_app", ScType::EdgeDCommonVar, ScType::Link >> "_image", ScType::EdgeAccessVarPosPerm, nrelImage);

  ScTemplateSearchResult searchRes;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchRes));

  EXPECT_EQ(searchRes.Size(), data.size());
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
    auto const & d1 = foundData[i];
    auto const & d2 = data[i];

    EXPECT_EQ(d1.m_app, d2.m_app);
    EXPECT_EQ(d1.m_idtf, d2.m_idtf);
    EXPECT_EQ(d1.m_image, d2.m_image);
  }
}

TEST_F(ScTemplateSearchTest, NodesWithTwoClasses)
{
  /**
   * class1 _-> _element;;
   * class2 _-> _element;;
   */

  ScAddr const classAddr1 = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(classAddr1.IsValid());

  ScAddr const classAddr2 = m_ctx->CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(classAddr2.IsValid());

  struct TestData
  {
    ScAddr m_addr;
    ScAddr m_classEdge1;
    ScAddr m_classEdge2;
  };

  std::vector<TestData> data(50);
  for (auto & d : data)
  {
    d.m_addr = m_ctx->CreateNode(ScType::NodeConstAbstract);
    EXPECT_TRUE(d.m_addr.IsValid());

    d.m_classEdge1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, classAddr1, d.m_addr);
    EXPECT_TRUE(d.m_classEdge1.IsValid());

    d.m_classEdge2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, classAddr2, d.m_addr);
    EXPECT_TRUE(d.m_classEdge2.IsValid());
  }

  ScTemplate templ;
  templ.Triple(classAddr1, ScType::EdgeAccessVarPosPerm >> "_class_edge1", ScType::NodeVar >> "_node");
  templ.Triple(classAddr2, ScType::EdgeAccessVarPosPerm >> "_class_edge2", "_node");

  ScTemplateSearchResult searchRes;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchRes));

  EXPECT_EQ(searchRes.Size(), data.size());
  std::vector<TestData> foundData(data.size());
  for (size_t i = 0; i < searchRes.Size(); ++i)
  {
    auto & d = foundData[i];

    d.m_addr = searchRes[i]["_node"];
    d.m_classEdge1 = searchRes[i]["_class_edge1"];
    d.m_classEdge2 = searchRes[i]["_class_edge2"];
  }

  auto compare = [](TestData const & a, TestData const & b)
  {
    return (a.m_addr.Hash() < b.m_addr.Hash());
  };
  std::sort(data.begin(), data.end(), compare);
  std::sort(foundData.begin(), foundData.end(), compare);

  for (size_t i = 0; i < searchRes.Size(); ++i)
  {
    auto const & d1 = foundData[i];
    auto const & d2 = data[i];

    EXPECT_EQ(d1.m_addr, d2.m_addr);
    EXPECT_EQ(d1.m_classEdge1, d2.m_classEdge1);
    EXPECT_EQ(d1.m_classEdge2, d2.m_classEdge2);
  }
}

TEST_F(ScTemplateSearchTest, ResultDeduplication)
{
  /**
   *   a -> b (* <- sc_node_material;; *);;
   *   a -> c;;
   *
   *  We should get just one search result, edge `a -> c` shouldn't appears twicely
   */
  ScAddr const a = m_ctx->CreateNode(ScType::Node);
  EXPECT_TRUE(a.IsValid());

  ScTemplate templ;
  templ.Triple(a >> "a", ScType::EdgeAccessVarPosPerm, ScType::NodeVarMaterial >> "b");

  templ.Triple("a", ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "c");

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));
  EXPECT_EQ(a, genResult["a"]);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  EXPECT_EQ(searchResult[0]["a"], genResult["a"]);
  EXPECT_EQ(searchResult[0]["b"], genResult["b"]);
  EXPECT_EQ(searchResult[0]["c"], genResult["c"]);
}

TEST_F(ScTemplateSearchTest, EqualConstructions)
{
  ScAddr const & begin = m_ctx->HelperResolveSystemIdtf("begin", ScType::NodeConstClass);
  ScAddr const & history = m_ctx->HelperResolveSystemIdtf("history", ScType::NodeConstClass);
  ScAddr const & nrel_changes_history = m_ctx->HelperResolveSystemIdtf("nrel_changes_history", ScType::NodeConstNoRole);
  ScAddr const & nrel_model_version = m_ctx->HelperResolveSystemIdtf("nrel_model_version", ScType::NodeConstNoRole);

  ScTemplate initVersionSearchTemplate;
  initVersionSearchTemplate.Triple(
      begin,
      ScType::EdgeAccessVarPosPerm >> "_begin_date_parameter_access_arc",
      ScType::NodeVarClass >> "_begin_date_parameter");
  initVersionSearchTemplate.Quintuple(
      ScType::NodeVar >> "_unchanged_sd_version",
      ScType::EdgeDCommonVar >> "_version_pair_arc",
      ScType::NodeVar >> "_changed_sd_version",
      ScType::EdgeAccessVarPosPerm >> "_begin_date_parameter_relation_access_arc",
      "_begin_date_parameter");
  initVersionSearchTemplate.Triple(
      ScType::NodeVarStruct >> "_changes_history",
      ScType::EdgeAccessVarPosPerm >> "_changes_history_access_arc",
      "_version_pair_arc");
  initVersionSearchTemplate.Triple(history, ScType::EdgeAccessVarPosPerm >> "_history_access_arc", "_changes_history");
  initVersionSearchTemplate.Quintuple(
      ScType::NodeVarStruct >> "_model_example",
      ScType::EdgeDCommonVar >> "_changes_history_pair_arc",
      "_changes_history",
      ScType::EdgeAccessVarPosPerm >> "_nrel_changes_history_access_arc",
      nrel_changes_history);
  initVersionSearchTemplate.Quintuple(
      "_model_example",
      ScType::EdgeDCommonVar >> "_nrel_model_version_pair_arc",
      "_version_pair_arc",
      ScType::EdgeAccessVarPosPerm >> "_nrel_model_version_access_arc",
      nrel_model_version);
  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(initVersionSearchTemplate, genResult));

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(initVersionSearchTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}

TEST_F(ScTemplateSearchTest, StructureElements)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  ScAddr const & structureAddr = m_ctx->CreateNode(ScType::NodeConstStruct);
  EXPECT_TRUE(structureAddr.IsValid());
  EXPECT_TRUE(helper.GenerateBySCsText(
      "test_node => test_relation: [];;"
      "test_node => test_relation: [];;"
      "test_node => test_relation: [];;"
      "test_node => test_relation: [];;"
      "test_node => test_relation: [];;"
      "test_node => test_relation: [];;"
      "test_node => test_relation: [];;",
      structureAddr));
  size_t const size = m_ctx->GetElementOutputArcsCount(structureAddr);
  for (size_t i = 0; i < size; ++i)
  {
    EXPECT_TRUE(
        helper.GenerateBySCsText("test_node => test_relation: [];;"
                                 "test_node => test_relation: [];;"));
  }

  ScAddr const & sourceAddr = m_ctx->HelperResolveSystemIdtf("test_node");
  ScAddr const & targetAddr = m_ctx->CreateLink(ScType::LinkConst);
  ScAddr const & relationAddr = m_ctx->HelperResolveSystemIdtf("test_relation");

  ScTemplate templ;
  templ.Quintuple(
      sourceAddr >> "_source",
      ScType::EdgeDCommonVar >> "_edge",
      targetAddr >> "_target",
      ScType::EdgeAccessVarPosPerm >> "_rel_edge",
      relationAddr >> "_relation");
  templ.Triple(structureAddr, ScType::EdgeAccessVarPosPerm, "_rel_edge");
  templ.Triple(structureAddr, ScType::EdgeAccessVarPosPerm, "_edge");
  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));

  templ.Clear();
  templ.Triple(
      relationAddr >> "_relation", ScType::EdgeAccessVarPosPerm >> "_rel_edge", ScType::EdgeDCommonVar >> "_edge");
  templ.Triple(structureAddr, ScType::EdgeAccessVarPosPerm, "_rel_edge");
  templ.Triple(sourceAddr >> "_source", "_edge", targetAddr >> "_target");
  templ.Triple(structureAddr, ScType::EdgeAccessVarPosPerm, "_edge");
  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  EXPECT_EQ(searchResult[0]["_source"], sourceAddr);
  EXPECT_EQ(searchResult[0]["_target"], targetAddr);
  EXPECT_EQ(searchResult[0]["_relation"], relationAddr);
}
