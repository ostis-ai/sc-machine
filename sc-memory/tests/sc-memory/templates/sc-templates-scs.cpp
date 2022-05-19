#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"
#include "template_test_utils.hpp"

using ScTemplateSCsTest = ScTemplateTest;

TEST_F(ScTemplateSCsTest, build_ok)
{
  ScAddr const addr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(addr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("d", addr));

  ScTemplate templ;
  char const * data = "_a _-> d;; _a <- sc_node_class;;";
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, data));
}

TEST_F(ScTemplateSCsTest, build_fail)
{
  ScTemplate templ;
  char const * data = "_a _-> b";
  EXPECT_FALSE(m_ctx->HelperBuildTemplate(templ, data));
}

TEST_F(ScTemplateSCsTest, search)
{
  ScTemplate genTempl;
  genTempl.Triple(
    ScType::NodeVar >> "_a",
    ScType::EdgeAccessVarPosPerm >> "_edge",
    ScType::NodeVarTuple >> "b");

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(genTempl, genResult));

  ScAddr const bAddr = genResult["b"];
  EXPECT_TRUE(bAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("b", bAddr));

  // do search
  ScTemplate templ;
  char const * data = "_a _-> b (* <- sc_node_tuple;; *);;";

  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, data));
  EXPECT_FALSE(templ.IsEmpty());

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(searchResult[0]["_a"], genResult["_a"]);
  EXPECT_EQ(searchResult[0]["b"], genResult["b"]);
}

TEST_F(ScTemplateSCsTest, generate)
{
  ScAddr const cAddr = m_ctx->CreateNode(ScType::Node);
  EXPECT_TRUE(cAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("c1", cAddr));

  ScTemplate templ;
  char const * data = "c1 _=> _b1;; _b1 <- sc_node_abstract;;";

  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, data));
  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));

  // check
  ScTemplate searchTempl;
  searchTempl.Triple(
    cAddr >> "c1",
    ScType::EdgeDCommonVar >> "_edge",
    ScType::NodeVarAbstract >> "_b1");

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(searchTempl, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(genResult["c1"], searchResult[0]["c1"]);
  EXPECT_EQ(genResult["_b1"], searchResult[0]["_b1"]);
}

TEST_F(ScTemplateSCsTest, generate_and_search)
{
  ScAddr const cAddr = m_ctx->CreateNode(ScType::Node);
  EXPECT_TRUE(cAddr.IsValid());
  EXPECT_TRUE(m_ctx->HelperSetSystemIdtf("g1", cAddr));

  ScTemplate templ;
  char const * data = "g1 _-> _l1 (* <- sc_node_material;; *);; g1 _-> _f1;;";

  EXPECT_TRUE(m_ctx->HelperBuildTemplate(templ, data));
  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));
  EXPECT_EQ(m_ctx->GetElementType(genResult["_l1"]), ScType::NodeConstMaterial);
  EXPECT_EQ(m_ctx->GetElementType(genResult["_f1"]), ScType::NodeConst);

  // check
  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(templ, searchResult));

  EXPECT_EQ(searchResult.Size(), 1u);
  EXPECT_EQ(genResult["g1"], searchResult[0]["g1"]);
  EXPECT_EQ(genResult["_l1"], searchResult[0]["_l1"]);
  EXPECT_EQ(genResult["_f1"], searchResult[0]["_f1"]);
}

TEST_F(ScTemplateSCsTest, idtf_does_not_exist)
{
  ScTemplate templ;
  ScTemplate::Result result = m_ctx->HelperBuildTemplate(templ, "non_existing_item _-> _z;;");
  EXPECT_FALSE(result);
  EXPECT_GT(result.Msg().size(), 0u);
}
