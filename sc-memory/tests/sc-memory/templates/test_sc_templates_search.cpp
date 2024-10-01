#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_structure.hpp"

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
  ScAddr const templateAddr = m_ctx->GenerateNode(ScType::ConstNodeStructure);
  EXPECT_TRUE(templateAddr.IsValid());

  ScStructure templStruct = m_ctx->ConvertToStructure(templateAddr);
  ScAddr xAddr;
  {
    ScAddr _yAddr, _zAddr, _sAddr;

    xAddr = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
    EXPECT_TRUE(xAddr.IsValid());
    EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("x", xAddr));

    _yAddr = m_ctx->GenerateNode(ScType::Var);
    EXPECT_TRUE(_yAddr.IsValid());
    EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_y", _yAddr));

    _zAddr = m_ctx->GenerateNode(ScType::VarNodeRole);
    EXPECT_TRUE(_zAddr.IsValid());
    EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_z", _zAddr));

    _sAddr = m_ctx->GenerateNode(ScType::VarNodeClass);
    EXPECT_TRUE(_sAddr.IsValid());
    EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("_s", _sAddr));

    ScAddr const xyAddr = m_ctx->GenerateConnector(ScType::VarPermPosArc, xAddr, _yAddr);
    EXPECT_TRUE(xyAddr.IsValid());
    ScAddr const zxyAddr = m_ctx->GenerateConnector(ScType::VarPermPosArc, _zAddr, xyAddr);
    EXPECT_TRUE(zxyAddr.IsValid());
    ScAddr const sxAddr = m_ctx->GenerateConnector(ScType::VarPermPosArc, _sAddr, xAddr);
    EXPECT_TRUE(sxAddr.IsValid());

    // append created elements into struct
    templStruct << xAddr << _yAddr << _zAddr << xyAddr << zxyAddr << _sAddr << sxAddr;
  }

  ScTemplate templ;
  m_ctx->BuildTemplate(templ, templateAddr);

  // create test structure that correspond to template
  {
    ScAddr const yAddr = m_ctx->GenerateNode(ScType::Const);
    EXPECT_TRUE(yAddr.IsValid());

    ScAddr const zAddr = m_ctx->GenerateNode(ScType::ConstNodeRole);
    EXPECT_TRUE(zAddr.IsValid());

    ScAddr const sAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
    EXPECT_TRUE(sAddr.IsValid());

    ScAddr const xyAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, xAddr, yAddr);
    EXPECT_TRUE(xyAddr.IsValid());
    ScAddr const zxyAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, zAddr, xyAddr);
    EXPECT_TRUE(zxyAddr.IsValid());
    ScAddr const sxAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, sAddr, xAddr);
    EXPECT_TRUE(sxAddr.IsValid());

    // test search by template
    {
      ScTemplateSearchResult result;
      EXPECT_TRUE(m_ctx->SearchByTemplate(templ, result));

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
  ScAddr const addr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr.IsValid());
  ScAddr const nrelMainIdtf = m_ctx->GenerateNode(ScType::ConstNodeNoRole);
  EXPECT_TRUE(nrelMainIdtf.IsValid());
  ScAddr const lang = m_ctx->GenerateNode(ScType::ConstNodeClass);
  EXPECT_TRUE(lang.IsValid());
  ScAddr const link = m_ctx->GenerateLink();
  EXPECT_TRUE(link.IsValid());

  ScAddr const arcCommon = m_ctx->GenerateConnector(ScType::ConstCommonArc, addr, link);
  EXPECT_TRUE(arcCommon.IsValid());
  ScAddr const arcAttr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nrelMainIdtf, arcCommon);
  EXPECT_TRUE(arcAttr.IsValid());
  ScAddr const arcLang = m_ctx->GenerateConnector(ScType::ConstPermPosArc, lang, link);
  EXPECT_TRUE(arcLang.IsValid());

  // now check search
  ScTemplate templ;
  templ.Quintuple(
      addr >> "_addr",
      ScType::VarCommonArc >> "_arcCommon",
      ScType::NodeLink >> "_link",
      ScType::VarPermPosArc >> "_arcAttr",
      nrelMainIdtf >> "_nrelMainIdtf");

  templ.Triple(lang >> "_lang", ScType::VarPermPosArc >> "_arcLang", "_link");

  // search
  {
    ScTemplateSearchResult res;
    EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));

    EXPECT_EQ(res.Size(), 1u);
    EXPECT_EQ(res[0]["_addr"], addr);
    EXPECT_EQ(res[0]["_arcCommon"], arcCommon);
    EXPECT_EQ(res[0]["_link"], link);
    EXPECT_EQ(res[0]["_arcAttr"], arcAttr);
    EXPECT_EQ(res[0]["_nrelMainIdtf"], nrelMainIdtf);
    EXPECT_EQ(res[0]["_lang"], lang);
    EXPECT_EQ(res[0]["_arcLang"], arcLang);
  }
}

TEST_F(ScTemplateSearchTest, UnknownType)
{
  // addr1 -> addr2;;
  ScAddr const addr1 = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr1.IsValid());
  ScAddr const addr2 = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
  EXPECT_TRUE(addr2.IsValid());
  ScAddr const arcAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr1, addr2);
  EXPECT_TRUE(arcAddr.IsValid());

  ScTemplate templ;
  templ.Triple(addr1, ScType::VarPermPosArc >> "arcAddr", ScType::Unknown >> "addr2");

  ScTemplateSearchResult res;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, res));
  EXPECT_EQ(res.Size(), 1u);

  EXPECT_EQ(res[0]["arcAddr"], arcAddr);
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

  ScAddr const deviceAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(deviceAddr.IsValid());

  ScAddr const nrelInstalledApp = m_ctx->GenerateNode(ScType::ConstNodeNoRole);
  EXPECT_TRUE(nrelInstalledApp.IsValid());

  ScAddr const _tuple = m_ctx->GenerateNode(ScType::ConstNodeTuple);
  EXPECT_TRUE(_tuple.IsValid());

  ScAddr const nrelIdtf = m_ctx->GenerateNode(ScType::ConstNodeNoRole);
  EXPECT_TRUE(nrelIdtf.IsValid());

  ScAddr const nrelImage = m_ctx->GenerateNode(ScType::ConstNodeNoRole);
  EXPECT_TRUE(nrelImage.IsValid());

  ScAddr arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, _tuple, deviceAddr);
  EXPECT_TRUE(arcAddr.IsValid());

  arcAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nrelInstalledApp, arcAddr);
  EXPECT_TRUE(arcAddr.IsValid());

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
    d.m_app = m_ctx->GenerateNode(ScType::ConstNodeMaterial);
    EXPECT_TRUE(d.m_app.IsValid());

    arcAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, _tuple, d.m_app);
    EXPECT_TRUE(arcAddr.IsValid());

    d.m_idtf = m_ctx->GenerateLink();
    EXPECT_TRUE(d.m_idtf.IsValid());

    ScLink idtfLink(*m_ctx, d.m_idtf);
    EXPECT_TRUE(idtfLink.Set("idtf_" + std::to_string(i)));

    arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, d.m_app, d.m_idtf);
    EXPECT_TRUE(arcAddr.IsValid());

    arcAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nrelIdtf, arcAddr);
    EXPECT_TRUE(arcAddr.IsValid());

    d.m_image = m_ctx->GenerateLink();
    EXPECT_TRUE(d.m_image.IsValid());

    ScLink imageLink(*m_ctx, d.m_image);
    EXPECT_TRUE(imageLink.Set("data_" + std::to_string(i)));

    arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, d.m_app, d.m_image);
    EXPECT_TRUE(arcAddr.IsValid());

    arcAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nrelImage, arcAddr);
    EXPECT_TRUE(arcAddr.IsValid());

    ++i;
  }

  ScTemplate templ;
  templ.Quintuple(
      ScType::VarNodeTuple >> "_tuple", ScType::VarCommonArc, deviceAddr, ScType::VarPermPosArc, nrelInstalledApp);

  templ.Triple("_tuple", ScType::VarPermPosArc, ScType::VarNode >> "_app");

  templ.Quintuple("_app", ScType::VarCommonArc, ScType::NodeLink >> "_idtf", ScType::VarPermPosArc, nrelIdtf);

  templ.Quintuple("_app", ScType::VarCommonArc, ScType::NodeLink >> "_image", ScType::VarPermPosArc, nrelImage);

  ScTemplateSearchResult searchRes;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchRes));

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

  ScAddr const classAddr1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  EXPECT_TRUE(classAddr1.IsValid());

  ScAddr const classAddr2 = m_ctx->GenerateNode(ScType::ConstNodeClass);
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
    d.m_addr = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(d.m_addr.IsValid());

    d.m_classEdge1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, classAddr1, d.m_addr);
    EXPECT_TRUE(d.m_classEdge1.IsValid());

    d.m_classEdge2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, classAddr2, d.m_addr);
    EXPECT_TRUE(d.m_classEdge2.IsValid());
  }

  ScTemplate templ;
  templ.Triple(classAddr1, ScType::VarPermPosArc >> "_class_arc1", ScType::VarNode >> "_node");
  templ.Triple(classAddr2, ScType::VarPermPosArc >> "_class_arc2", "_node");

  ScTemplateSearchResult searchRes;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchRes));

  EXPECT_EQ(searchRes.Size(), data.size());
  std::vector<TestData> foundData(data.size());
  for (size_t i = 0; i < searchRes.Size(); ++i)
  {
    auto & d = foundData[i];

    d.m_addr = searchRes[i]["_node"];
    d.m_classEdge1 = searchRes[i]["_class_arc1"];
    d.m_classEdge2 = searchRes[i]["_class_arc2"];
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
   *  We should get just one search result, arcAddr `a -> c` shouldn't appears twicely
   */
  ScAddr const a = m_ctx->GenerateNode(ScType::Node);
  EXPECT_TRUE(a.IsValid());

  ScTemplate templ;
  templ.Triple(a >> "a", ScType::VarPermPosArc, ScType::VarNodeMaterial >> "b");

  templ.Triple("a", ScType::VarPermPosArc, ScType::VarNode >> "c");

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);
  EXPECT_EQ(a, genResult["a"]);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  EXPECT_EQ(searchResult[0]["a"], genResult["a"]);
  EXPECT_EQ(searchResult[0]["b"], genResult["b"]);
  EXPECT_EQ(searchResult[0]["c"], genResult["c"]);
}

TEST_F(ScTemplateSearchTest, EqualConstructions)
{
  ScAddr const & begin = m_ctx->ResolveElementSystemIdentifier("begin", ScType::ConstNodeClass);
  ScAddr const & history = m_ctx->ResolveElementSystemIdentifier("history", ScType::ConstNodeClass);
  ScAddr const & nrel_changes_history =
      m_ctx->ResolveElementSystemIdentifier("nrel_changes_history", ScType::ConstNodeNoRole);
  ScAddr const & nrel_model_version =
      m_ctx->ResolveElementSystemIdentifier("nrel_model_version", ScType::ConstNodeNoRole);

  ScTemplate initVersionSearchTemplate;
  initVersionSearchTemplate.Triple(
      begin,
      ScType::VarPermPosArc >> "_begin_date_parameter_access_arc",
      ScType::VarNodeClass >> "_begin_date_parameter");
  initVersionSearchTemplate.Quintuple(
      ScType::VarNode >> "_unchanged_sd_version",
      ScType::VarCommonArc >> "_version_pair_arc",
      ScType::VarNode >> "_changed_sd_version",
      ScType::VarPermPosArc >> "_begin_date_parameter_relation_access_arc",
      "_begin_date_parameter");
  initVersionSearchTemplate.Triple(
      ScType::VarNodeStructure >> "_changes_history",
      ScType::VarPermPosArc >> "_changes_history_access_arc",
      "_version_pair_arc");
  initVersionSearchTemplate.Triple(history, ScType::VarPermPosArc >> "_history_access_arc", "_changes_history");
  initVersionSearchTemplate.Quintuple(
      ScType::VarNodeStructure >> "_model_example",
      ScType::VarCommonArc >> "_changes_history_pair_arc",
      "_changes_history",
      ScType::VarPermPosArc >> "_nrel_changes_history_access_arc",
      nrel_changes_history);
  initVersionSearchTemplate.Quintuple(
      "_model_example",
      ScType::VarCommonArc >> "_nrel_model_version_pair_arc",
      "_version_pair_arc",
      ScType::VarPermPosArc >> "_nrel_model_version_access_arc",
      nrel_model_version);
  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(initVersionSearchTemplate, genResult);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(initVersionSearchTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}

TEST_F(ScTemplateSearchTest, StructureElements)
{
  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());

  ScAddr const & structureAddr = m_ctx->GenerateNode(ScType::ConstNodeStructure);
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
  size_t const size = m_ctx->GetElementEdgesAndOutgoingArcsCount(structureAddr);
  for (size_t i = 0; i < size; ++i)
  {
    EXPECT_TRUE(
        helper.GenerateBySCsText("test_node => test_relation: [];;"
                                 "test_node => test_relation: [];;"));
  }

  ScAddr const & sourceAddr = m_ctx->ResolveElementSystemIdentifier("test_node");
  ScAddr const & targetAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
  ScAddr const & relationAddr = m_ctx->ResolveElementSystemIdentifier("test_relation");

  ScTemplate templ;
  templ.Quintuple(
      sourceAddr >> "_source",
      ScType::VarCommonArc >> "_arc",
      targetAddr >> "_target",
      ScType::VarPermPosArc >> "_rel_arc",
      relationAddr >> "_relation");
  templ.Triple(structureAddr, ScType::VarPermPosArc, "_rel_arc");
  templ.Triple(structureAddr, ScType::VarPermPosArc, "_arc");
  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);

  templ.Clear();
  templ.Triple(relationAddr >> "_relation", ScType::VarPermPosArc >> "_rel_arc", ScType::VarCommonArc >> "_arc");
  templ.Triple(structureAddr, ScType::VarPermPosArc, "_rel_arc");
  templ.Triple(sourceAddr >> "_source", "_arc", targetAddr >> "_target");
  templ.Triple(structureAddr, ScType::VarPermPosArc, "_arc");
  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  EXPECT_EQ(searchResult[0]["_source"], sourceAddr);
  EXPECT_EQ(searchResult[0]["_target"], targetAddr);
  EXPECT_EQ(searchResult[0]["_relation"], relationAddr);
}
