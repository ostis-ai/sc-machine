/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_structure.hpp>

#include "template_test_utils.hpp"

using ScTemplateLoadTest = ScTemplateTest;

class ScTemplateLoadContext : public ScMemoryContext
{
public:
  void LoadTemplate(
      ScTemplate & translatableTemplate,
      ScAddr & resultTemplateAddr,
      ScTemplateParams const & params = ScTemplateParams())
  {
    ScMemoryContext::LoadTemplate(translatableTemplate, resultTemplateAddr, params);
  }
};

TEST_F(ScTemplateLoadTest, LoadCheckTemplate)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & testRelationAddr = m_ctx->GenerateNode(ScType::ConstNodeNoRole);

  ScTemplate templ;
  templ.Triple(testClassAddr, ScType::VarPermPosArc >> "_arc_to_test_object", ScType::VarNodeLink >> "_test_object");
  templ.Quintuple(
      "_test_object", ScType::VarCommonArc, ScType::VarNode >> "_test_set", ScType::VarPermPosArc, testRelationAddr);
  templ.Triple("_test_set", ScType::VarPermPosArc, "_arc_to_test_object");

  ScAddr templAddr;
  ScTemplateLoadContext ctx;
  ctx.LoadTemplate(templ, templAddr);

  ScStructure templateStruct = m_ctx->ConvertToStructure(templAddr);
  {
    EXPECT_TRUE(templateStruct.HasElement(testClassAddr));
    EXPECT_TRUE(templateStruct.HasElement(testRelationAddr));

    ScIterator3Ptr it3 = m_ctx->CreateIterator3(testClassAddr, ScType::VarPermPosArc, ScType::VarNodeLink);
    EXPECT_TRUE(it3->Next());
    ScAddr const & arcToTestObject = it3->Get(1);
    ScAddr const & testObject = it3->Get(2);
    EXPECT_TRUE(templateStruct.HasElement(arcToTestObject));
    EXPECT_TRUE(templateStruct.HasElement(testObject));
    EXPECT_FALSE(it3->Next());

    ScIterator5Ptr it5 = m_ctx->CreateIterator5(
        testObject, ScType::VarCommonArc, ScType::VarNode, ScType::VarPermPosArc, testRelationAddr);
    EXPECT_TRUE(it5->Next());
    ScAddr const & arcToTestSet = it5->Get(1);
    ScAddr const & testSet = it5->Get(2);
    ScAddr const & arcToArcToTestSet = it5->Get(3);
    EXPECT_TRUE(templateStruct.HasElement(arcToTestSet));
    EXPECT_TRUE(templateStruct.HasElement(testSet));
    EXPECT_TRUE(templateStruct.HasElement(arcToArcToTestSet));
    EXPECT_FALSE(it5->Next());

    it3 = m_ctx->CreateIterator3(testSet, ScType::VarPermPosArc, arcToTestObject);
    EXPECT_TRUE(it3->Next());
    ScAddr const & arcToArcToTestObject = it3->Get(1);
    EXPECT_TRUE(templateStruct.HasElement(arcToArcToTestObject));
    EXPECT_FALSE(it3->Next());
  }
}

TEST_F(ScTemplateLoadTest, GenerateSearchLoadCheckBuildSearchTemplate)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & testRelationAddr = m_ctx->GenerateNode(ScType::ConstNodeNoRole);

  ScTemplate templ;
  templ.Triple(testClassAddr, ScType::VarPermPosArc >> "_arc_to_test_object", ScType::VarNodeLink >> "_test_object");
  templ.Quintuple(
      "_test_object", ScType::VarCommonArc, ScType::VarNode >> "_test_set", ScType::VarPermPosArc, testRelationAddr);
  templ.Triple("_test_set", ScType::VarPermPosArc, "_arc_to_test_object");
  EXPECT_EQ(templ.Size(), 4u);

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  ScAddr templAddr;
  ScTemplateLoadContext ctx;
  ctx.LoadTemplate(templ, templAddr);

  ScTemplate builtTemplate;
  m_ctx->BuildTemplate(builtTemplate, templAddr);

  EXPECT_EQ(builtTemplate.Size(), 4u);

  EXPECT_TRUE(m_ctx->SearchByTemplate(builtTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}

TEST_F(ScTemplateLoadTest, GenerateSearchLoadWithGeneratedLinkCheckBuildSearchTemplate)
{
  ScAddr const & testClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const & testRelationAddr = m_ctx->GenerateNode(ScType::ConstNodeNoRole);

  ScTemplate templ;
  templ.Triple(testClassAddr, ScType::VarPermPosArc >> "_arc_to_test_object", ScType::VarNodeLink >> "_test_object");
  templ.Quintuple(
      "_test_object", ScType::VarCommonArc, ScType::VarNode >> "_test_set", ScType::VarPermPosArc, testRelationAddr);
  templ.Triple("_test_set", ScType::VarPermPosArc, "_arc_to_test_object");
  EXPECT_EQ(templ.Size(), 4u);

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);

  ScAddr const & testObject = genResult["_test_object"];
  ScTemplateParams params;
  params.Add("_test_object", testObject);

  ScAddr templAddr;
  ScTemplateLoadContext ctx;
  ctx.LoadTemplate(templ, templAddr, params);
  ScStructure templateStruct = m_ctx->ConvertToStructure(templAddr);
  EXPECT_TRUE(templateStruct.HasElement(testObject));

  ScTemplate builtTemplate;
  m_ctx->BuildTemplate(builtTemplate, templAddr);

  EXPECT_EQ(builtTemplate.Size(), 4u);

  EXPECT_TRUE(m_ctx->SearchByTemplate(builtTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), 1u);
}
