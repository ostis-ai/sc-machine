/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_agent_context.hpp>
#include <sc-memory/sc_structure.hpp>
#include <sc-memory/sc_oriented_set.hpp>

using ScAgentContextTest = ScMemoryTest;

TEST_F(ScAgentContextTest, MoveAgentContext)
{
  ScAgentContext context1;
  ScAgentContext context2 = ScAgentContext();
  EXPECT_TRUE(context2.IsValid());

  context1 = std::move(context2);

  EXPECT_TRUE(context1.IsValid());
  EXPECT_FALSE(context2.IsValid());

  ScAgentContext context3 = std::move(context2);
  EXPECT_FALSE(context3.IsValid());
}

TEST_F(ScAgentContextTest, GenerateOrientedSet)
{
  ScOrientedSet set = m_ctx->GenerateOrientedSet();
  EXPECT_EQ(m_ctx->GetElementType(set), ScType::ConstNode);

  ScOrientedSet set2 = m_ctx->ConvertToOrientedSet(set);
  EXPECT_EQ(set, set2);
}

TEST_F(ScAgentContextTest, ConvertInvalidOrientedSet)
{
  EXPECT_THROW(m_ctx->ConvertToOrientedSet(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentContextTest, GenerateSet)
{
  ScSet set = m_ctx->GenerateSet();
  EXPECT_EQ(m_ctx->GetElementType(set), ScType::ConstNode);

  ScSet set2 = m_ctx->ConvertToSet(set);
  EXPECT_EQ(set, set2);
}

TEST_F(ScAgentContextTest, ConvertInvalidSet)
{
  EXPECT_THROW(m_ctx->ConvertToSet(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentContextTest, CreateStruct)
{
  ScStructure structure = m_ctx->GenerateStructure();
  EXPECT_EQ(m_ctx->GetElementType(structure), ScType::ConstNodeStructure);

  ScSet structure2 = m_ctx->ConvertToStructure(structure);
  EXPECT_EQ(structure, structure2);
}

TEST_F(ScAgentContextTest, ConvertInvalidStructure)
{
  EXPECT_THROW(m_ctx->ConvertToStructure(ScAddr::Empty), utils::ExceptionInvalidParams);
}
