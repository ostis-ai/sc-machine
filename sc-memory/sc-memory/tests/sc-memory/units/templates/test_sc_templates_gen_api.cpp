/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_structure.hpp>

#include "template_test_utils.hpp"

using ScTemplateGenApiTest = ScTemplateTest;

TEST_F(ScTemplateGenApiTest, GenWithResultNotSafeGet)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);
  EXPECT_EQ(result.Size(), 3u);

  EXPECT_TRUE(result[0].IsValid());
  EXPECT_TRUE(result["_addr1"].IsValid());
  EXPECT_TRUE(result[1].IsValid());
  EXPECT_TRUE(result["_arc"].IsValid());
  EXPECT_TRUE(result[2].IsValid());
  EXPECT_TRUE(result["_addr2"].IsValid());

  EXPECT_THROW(result[3], utils::ExceptionInvalidParams);
  EXPECT_THROW(result["_other_arc"], utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenWithResultSafeGet)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::VarNodeLink >> "_addr2");

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);
  EXPECT_EQ(result.Size(), 3u);

  ScAddr foundAddr;
  EXPECT_TRUE(result.Get(0, foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get("_addr1", foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get(1, foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get("_arc", foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get(2, foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());
  EXPECT_TRUE(result.Get("_addr2", foundAddr));
  EXPECT_TRUE(foundAddr.IsValid());

  EXPECT_FALSE(result.Get(3, foundAddr));
  EXPECT_FALSE(foundAddr.IsValid());
  EXPECT_FALSE(result.Get("_other_arc", foundAddr));
  EXPECT_FALSE(foundAddr.IsValid());
}

TEST_F(ScTemplateGenApiTest, GenTripleWithTargetEdge)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::VarTempPosArc >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnknownTargetElement)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::Unknown >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnknownSourceElement)
{
  ScTemplate templ;
  templ.Triple(ScType::Unknown >> "_addr1", ScType::VarPermPosArc >> "_arc", ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithUnknownSecondElement)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::Unknown >> "_arc", ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenEmptyTemplate)
{
  ScTemplate templ;
  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);
}

TEST_F(ScTemplateGenApiTest, GenTemplateSuccessfully)
{
  ScTemplate templ;
  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, ScTemplateParams::Empty);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithFixedSecondEdge)
{
  ScAddr const & addr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & addr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstPermPosArc, addr1, addr2);

  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", arcAddr, ScType::Unknown >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithTypedFirstEdge)
{
  ScTemplate templ;
  templ.Triple(ScType::VarPermPosArc >> "_addr1", ScType::VarPermPosArc, ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithTypedThirdEdge)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarPermPosArc, ScType::VarPermPosArc >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTripleWithInvalidRefSecondElement)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", "_arc", ScType::VarNode >> "_addr2");

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithFullyReplacedVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarCommonArc >> "_arc", ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, "_arc");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr2);
  params.Add("_arc", arcAddr);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, params);

  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result["_arc"], arcAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidSourceElementInFullyReplacedVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarCommonArc >> "_arc", ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, "_arc");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr2);
  params.Add("_addr2", nodeAddr2);
  params.Add("_arc", arcAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidTargetElementInFullyReplacedVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarCommonArc >> "_arc", ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, "_arc");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr1);
  params.Add("_arc", arcAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedVariableThirdEdgeInVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr2", ScType::VarTempPosArc, ScType::VarCommonArc >> "_arc");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_arc", arcAddr);
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, params);

  EXPECT_EQ(result["_arc"], arcAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedSourceAndTargetInVariableTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", arcAddr, ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, arcAddr);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, params);

  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result[arcAddr], arcAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithConstantArc)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", arcAddr, ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, arcAddr);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);

  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result[arcAddr], arcAddr);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacedSourceInVariableTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", arcAddr, ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, arcAddr);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr2);
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacedTargetInVariableTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", arcAddr, ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, arcAddr);

  ScTemplateParams params;
  params.Add("_addr1", nodeAddr1);
  params.Add("_addr2", nodeAddr1);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithConstantTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(nodeAddr1, arcAddr, nodeAddr2);
  templ.Triple(nodeAddr2, ScType::VarTempPosArc, arcAddr);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result);

  EXPECT_EQ(result[nodeAddr1], nodeAddr1);
  EXPECT_EQ(result[arcAddr], arcAddr);
  EXPECT_EQ(result[nodeAddr2], nodeAddr2);

  for (ScAddr const & addr : result)
    EXPECT_TRUE(addr.IsValid());
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidSourceInConstantTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(nodeAddr2, arcAddr, nodeAddr2);
  templ.Triple(nodeAddr2, ScType::VarTempPosArc, arcAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidTargetInConstantTriple)
{
  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplate templ;
  templ.Triple(nodeAddr1, arcAddr, nodeAddr1);
  templ.Triple(nodeAddr2, ScType::VarTempPosArc, arcAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedSecondElementInVariableTriple)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarTempPosArc, ScType::VarCommonArc >> "_arc");
  templ.Triple("_addr1", "_arc", ScType::VarNode >> "_addr2");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_arc", arcAddr);
  params.Add("_addr1", nodeAddr1);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, params);

  EXPECT_EQ(result["_arc"], arcAddr);
  EXPECT_EQ(result["_addr1"], nodeAddr1);
  EXPECT_EQ(result["_addr2"], nodeAddr2);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacementNameInParams)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarCommonArc >> "_arc", ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, "_arc");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_invalid_arc_name", arcAddr);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithInvalidReplacementVarInParams)
{
  m_ctx->ResolveElementSystemIdentifier("_var", ScType::VarNode);

  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarCommonArc >> "_arc", ScType::VarNode >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, "_arc");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_var", nodeAddr1);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);

  ScAddr resultAddr;
  EXPECT_FALSE(result.Get(0, resultAddr));
  EXPECT_EQ(resultAddr, ScAddr::Empty);

  EXPECT_THROW(result[0], utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithReplacedTargetHavingUnextendableType)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarCommonArc >> "_arc", ScType::VarNodeTuple >> "_addr2");
  templ.Triple("_addr2", ScType::VarTempPosArc, "_arc");

  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);

  ScTemplateParams params;
  params.Add("_addr2", nodeAddr2);

  ScTemplateGenResult result;
  EXPECT_THROW(m_ctx->GenerateByTemplate(templ, result, params), utils::ExceptionInvalidParams);
}

TEST_F(ScTemplateGenApiTest, GenTemplateWithNodeReplacedByArc)
{
  ScTemplate templ;
  templ.Triple(ScType::VarNode, ScType::VarCommonArc >> "_arc", ScType::VarNode);
  templ.Triple(ScType::VarNode >> "_addr1", ScType::VarTempPosArc, "_arc");
  templ.Triple("_arc", ScType::VarPermPosArc, ScType::VarNode >> "_addr2");

  ScAddr const & nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & edgeAddr = m_ctx->GenerateConnector(ScType::ConstCommonArc, nodeAddr1, nodeAddr2);

  ScTemplateParams params;
  params.Add("_addr2", edgeAddr);

  ScTemplateGenResult result;
  m_ctx->GenerateByTemplate(templ, result, params);

  EXPECT_EQ(result["_addr2"], edgeAddr);
}
