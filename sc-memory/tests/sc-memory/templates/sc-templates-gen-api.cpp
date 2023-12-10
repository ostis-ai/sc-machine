#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "template_test_utils.hpp"

using ScTemplateGenApiTest = ScTemplateTest;

TEST_F(ScTemplateGenApiTest, GenWithResultNotSafeGet)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));
  EXPECT_EQ(result.Size(), 3u);

  EXPECT_TRUE(result[0].IsValid());
  EXPECT_TRUE(result["_addr1"].IsValid());
  EXPECT_TRUE(result[1].IsValid());
  EXPECT_TRUE(result["_edge"].IsValid());
  EXPECT_TRUE(result[2].IsValid());
  EXPECT_TRUE(result["_addr2"].IsValid());

  EXPECT_THROW(result[3], utils::ExceptionInvalidParams);
  EXPECT_THROW(result["_other_edge"], utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenWithResultSafeGet)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));
  EXPECT_EQ(result.Size(), 3u);

  ScAddr foundAddr;
  EXPECT_TRUE(result.Get(0, foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get("_addr1", foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get(1, foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get("_edge", foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get(2, foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get("_addr2", foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());

  EXPECT_FALSE(result.Get(3, foundAddr));
  EXPECT_FALSE(foundAddr.IsValid());
  EXPECT_FALSE(result.Get("_other_edge", foundAddr));
  EXPECT_FALSE(foundAddr.IsValid());
}

TEST_F(ScTemplateGenApiTest, GenTripleWithEdgeTarget)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::EdgeAccessVarPosTemp >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnkownTarget)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      ScType::EdgeAccessVarPosPerm >> "_edge",
      ScType::Unknown >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));
}

TEST_F(ScTemplateGenApiTest, GenEmptyTemplate)
{
  ScTemplate templ;
  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));
}

TEST_F(ScTemplateGenApiTest, GenTripleWithFixedSecondEdge)
{
  ScAddr const & addr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & addr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);

  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      edgeAddr,
      ScType::Unknown >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithTypedThirdEdge)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      ScType::EdgeAccessVarPosPerm,
      ScType::EdgeAccessVarPosPerm >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithRefSecondEdge)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1",
      "_edge",
      ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}
