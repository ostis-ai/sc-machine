#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"
#include "template_test_utils.hpp"

using ScTemplateSCsTest = ScTemplateTest;

TEST_F(ScTemplateSCsTest, BuildSuccessful)
{
  ScAddr const addr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_TRUE(addr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("d", addr));

  ScTemplate templ;
  sc_char const * data = "_a _-> d;; _a <- sc_node_class;;";
  m_ctx->BuildTemplate(templ, data);
}

TEST_F(ScTemplateSCsTest, BuildFail)
{
  ScTemplate templ;
  sc_char const * data = "_a _-> b";
  EXPECT_THROW(m_ctx->BuildTemplate(templ, data), utils::ExceptionParseError);
}

TEST_F(ScTemplateSCsTest, GenBuildSearch)
{
  ScTemplate genTempl;
  genTempl.Triple(ScType::VarNode >> "_a", ScType::VarPermPosArc >> "_edge", ScType::VarNodeTuple >> "b");

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(genTempl, genResult);

  ScAddr const bAddr = genResult["b"];
  EXPECT_TRUE(bAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("b", bAddr));

  // do search
  ScTemplate templ;
  sc_char const * data = "_a _-> b (* <- sc_node_tuple;; *);;";

  m_ctx->BuildTemplate(templ, data);
  EXPECT_FALSE(templ.IsEmpty());

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_a"], genResult["_a"]);
  EXPECT_EQ(searchResult[0]["b"], genResult["b"]);
}

TEST_F(ScTemplateSCsTest, BuildGenerate)
{
  ScAddr const cAddr = m_ctx->GenerateNode(ScType::Node);
  EXPECT_TRUE(cAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("c1", cAddr));

  ScTemplate templ;
  sc_char const * data = "c1 _=> _b1;; _b1 <- sc_node_material;;";

  m_ctx->BuildTemplate(templ, data);
  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);

  // check
  ScTemplate searchTempl;
  searchTempl.Triple(cAddr >> "c1", ScType::VarCommonArc >> "_edge", ScType::VarNodeMaterial >> "_b1");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(searchTempl, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(genResult["c1"], searchResult[0]["c1"]);
  EXPECT_EQ(genResult["_b1"], searchResult[0]["_b1"]);
}

TEST_F(ScTemplateSCsTest, GenerateSearch)
{
  ScAddr const cAddr = m_ctx->GenerateNode(ScType::Node);
  EXPECT_TRUE(cAddr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("g1", cAddr));

  ScTemplate templ;
  sc_char const * data = "g1 _-> _l1 (* <- sc_node_material;; *);; g1 _-> _f1;;";

  m_ctx->BuildTemplate(templ, data);
  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);
  EXPECT_EQ(m_ctx->GetElementType(genResult["_l1"]), ScType::ConstNodeMaterial);
  EXPECT_EQ(m_ctx->GetElementType(genResult["_f1"]), ScType::ConstNode);

  // check
  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(genResult["g1"], searchResult[0]["g1"]);
  EXPECT_EQ(genResult["_l1"], searchResult[0]["_l1"]);
  EXPECT_EQ(genResult["_f1"], searchResult[0]["_f1"]);
}

TEST_F(ScTemplateSCsTest, IdtfNotExist)
{
  ScTemplate templ;
  EXPECT_THROW(m_ctx->BuildTemplate(templ, "non_existing_item _-> _z;;"), utils::ExceptionInvalidState);
}
