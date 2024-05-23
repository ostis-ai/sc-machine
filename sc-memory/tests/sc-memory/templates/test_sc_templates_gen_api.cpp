#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_struct.hpp"

#include "template_test_utils.hpp"

using ScTemplateGenApiTest = ScTemplateTest;

TEST_F(ScTemplateGenApiTest, GenWithResultNotSafeGet)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_edge", ScType::NodeVar >> "_addr2");

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
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_edge", ScType::LinkVar >> "_addr2");

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

TEST_F(ScTemplateGenApiTest, GenTripleWithTargetEdge)
{
  ScTemplate templ;
  templ.Triple(
      ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_edge", ScType::EdgeAccessVarPosTemp >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnkownTargetElement)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_edge", ScType::Unknown >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnkownSourceElement)
{
  ScTemplate templ;
  templ.Triple(ScType::Unknown >> "_addr1", ScType::EdgeAccessVarPosPerm >> "_edge", ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnkownSecondElement)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::Unknown >> "_edge", ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenEmptyTemplate)
{
  ScTemplate templ;
  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithOutErrorCode)
{
  ScTemplate templ;
  ScTemplateGenResult result;
  ScTemplateResultCode errorCode;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, ScTemplateParams::Empty, &errorCode));
  EXPECT_EQ(errorCode, ScTemplateResultCode::Success);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithFixedSecondEdge)
{
  ScAddr const & addr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & addr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, addr1, addr2);

  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", edgeAddr, ScType::Unknown >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithTypedFirstEdge)
{
  ScTemplate templ;
  templ.Triple(ScType::EdgeAccessVarPosPerm >> "_addr1", ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithTypedThirdEdge)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosPerm, ScType::EdgeAccessVarPosPerm >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithInvalidRefSecondElement)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", "_edge", ScType::NodeVar >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithFullyReplacedVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, "_edge");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr2);
  params.Add("_edge", edgeAddr);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, params));

  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result["_edge"], edgeAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidSourceElementInFullyReplacedVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, "_edge");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr2);
  params.Add("_addr2", nodeAddr2);
  params.Add("_edge", edgeAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidTargetElementInFullyReplacedVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, "_edge");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr1);
  params.Add("_edge", edgeAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedVariableThirdEdgeInVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr2", ScType::EdgeAccessVarPosTemp, ScType::EdgeDCommonVar >> "_edge");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_edge", edgeAddr);
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, params));

  EXPECT_EQ(result["_edge"], edgeAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedSourceAndTargetInVariableTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", edgeAddr, ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, edgeAddr);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, params));

  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result[edgeAddr], edgeAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacedSourceInVariableTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", edgeAddr, ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, edgeAddr);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr2);
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacedTargetInVariableTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", edgeAddr, ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, edgeAddr);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr1);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithConstantTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(nodeAddr1, edgeAddr, nodeAddr2);
  templ.Triple(nodeAddr2, ScType::EdgeAccessVarPosTemp, edgeAddr);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result));

  EXPECT_EQ(result[nodeAddr1], nodeAddr1);
  EXPECT_EQ(result[edgeAddr], edgeAddr);
  EXPECT_EQ(result[nodeAddr2], nodeAddr2);

  for (ScAddr const & addr : result)
    EXPECT_TRUE(addr.IsValid());
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidSourceInConstantTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(nodeAddr2, edgeAddr, nodeAddr2);
  templ.Triple(nodeAddr2, ScType::EdgeAccessVarPosTemp, edgeAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidTargetInConstantTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(nodeAddr1, edgeAddr, nodeAddr1);
  templ.Triple(nodeAddr2, ScType::EdgeAccessVarPosTemp, edgeAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedSecondElementInVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeAccessVarPosTemp, ScType::EdgeDCommonVar >> "_edge");
  templ.Triple("_addr1", "_edge", ScType::NodeVar >> "_addr2");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_edge", edgeAddr);
  params.Add("_addr1", nodeAddr1);

  ScTemplateGenResult result;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, result, params));

  EXPECT_EQ(result["_edge"], edgeAddr);
  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacementNameInParams)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, "_edge");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_invalid_edge_name", edgeAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacementVarInParams)
{
  m_ctx->HelperResolveSystemIdtf("_var", ScType::NodeVar);

  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVar >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, "_edge");

  ScAddr const & nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeDCommonConst, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_var", nodeAddr1);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);

  ScAddr resultAddr;
  EXPECT_FALSE(result.Get(0, resultAddr));
  EXPECT_EQ(resultAddr, ScAddr::Empty);

  EXPECT_THROW(result[0], utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedTargetHavingUnextendableType)
{
  ScTemplate templ;
  templ.Triple(ScType::NodeVar >> "_addr1", ScType::EdgeDCommonVar >> "_edge", ScType::NodeVarTuple >> "_addr2");
  templ.Triple("_addr2", ScType::EdgeAccessVarPosTemp, "_edge");

  ScAddr const & nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);

  ScTemplateParams params;
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->HelperGenTemplate(templ, result, params), utils::ExceptionInvalidParams);
}
