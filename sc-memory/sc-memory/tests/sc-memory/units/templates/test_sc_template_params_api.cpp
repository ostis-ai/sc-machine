/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_structure.hpp>

#include "template_test_utils.hpp"

using ScTemplateParamsTest = ScTemplateTest;

TEST_F(ScTemplateParamsTest, ParamsWithVarNameSubstitute)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);
  ScTemplateParams params;

  params.Add("_node", addr);
  ScAddr outAddr;
  params.Get("_node", outAddr);
  EXPECT_EQ(outAddr, addr);
}

TEST_F(ScTemplateParamsTest, ParamsWithVarNameSubstituteDuplicateError)
{
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);
  ScTemplateParams params;

  params.Add("_node", addr);
  EXPECT_THROW(params.Add("_node", addr), utils::ExceptionInvalidParams);
  ScAddr outAddr;
  params.Get("_node", outAddr);
  EXPECT_EQ(outAddr, addr);
}

TEST_F(ScTemplateParamsTest, ParamsWithVarNameSubstituteNotFoundError)
{
  ScTemplateParams params;

  ScAddr outAddr;
  EXPECT_FALSE(params.Get("_node", outAddr));
  EXPECT_FALSE(outAddr.IsValid());
}

TEST_F(ScTemplateParamsTest, ParamsWithVarAddrSubstitute)
{
  ScAddr const & varAddr = m_ctx->GenerateNode(ScType::VarNode);
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);
  ScTemplateParams params;

  params.Add(varAddr, addr);
  ScAddr outAddr;
  EXPECT_TRUE(params.Get(varAddr, outAddr));
  EXPECT_EQ(outAddr, addr);
}

TEST_F(ScTemplateParamsTest, ParamsWithVarAddrSubstituteDuplicateError)
{
  ScAddr const & varAddr = m_ctx->GenerateNode(ScType::VarNode);
  ScAddr const & addr = m_ctx->GenerateNode(ScType::ConstNode);
  ScTemplateParams params;

  params.Add(varAddr, addr);
  EXPECT_THROW(params.Add(varAddr, addr), utils::ExceptionInvalidParams);
  ScAddr outAddr;
  params.Get(varAddr, outAddr);
  EXPECT_EQ(outAddr, addr);
}

TEST_F(ScTemplateParamsTest, ParamsWithVarAddrSubstituteError)
{
  ScAddr const & varAddr = m_ctx->GenerateNode(ScType::VarNode);
  ScTemplateParams params;

  ScAddr outAddr;
  EXPECT_FALSE(params.Get(varAddr, outAddr));
  EXPECT_FALSE(outAddr.IsValid());
}
