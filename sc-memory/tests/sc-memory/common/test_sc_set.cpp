#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_set.hpp"

#include "sc_test.hpp"

using ScSetTest = ScMemoryTest;

TEST_F(ScSetTest, AppendElementWithDefaultArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr));
  EXPECT_TRUE(set.Has(elementAddr));
}

TEST_F(ScSetTest, AppendElementWithTempArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr, ScType::EdgeAccessConstPosTemp));
  EXPECT_TRUE(set.Has(elementAddr));
  EXPECT_TRUE(set.Has(elementAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScSetTest, AppendElementWithNegArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr, ScType::EdgeAccessConstNegPerm));
}

TEST_F(ScSetTest, AppendMultipleElements)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr1));
  EXPECT_TRUE(set.Append(elementAddr2));
  EXPECT_TRUE(set.Has(elementAddr1));
  EXPECT_TRUE(set.Has(elementAddr2));
}

TEST_F(ScSetTest, AppendExistingElementWithDefaultArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr));
  EXPECT_FALSE(set.Append(elementAddr));
}

TEST_F(ScSetTest, AppendExistingElementWithTempArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr, ScType::EdgeAccessConstPosTemp));
  EXPECT_FALSE(set.Append(elementAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScSetTest, AppendExistingElementWithDefaultAndTempArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr));
  EXPECT_FALSE(set.Append(elementAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScSetTest, AppendElementWithDefaultArcTypeAndRemoveElementWithDefaultArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr));
  EXPECT_TRUE(set.Remove(elementAddr));
  EXPECT_FALSE(set.Has(elementAddr));
}

TEST_F(ScSetTest, AppendElementWithDefaultArcTypeAndRemoveElementWithTempArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr));
  EXPECT_FALSE(set.Remove(elementAddr, ScType::EdgeAccessConstPosTemp));
  EXPECT_TRUE(set.Has(elementAddr));
}

TEST_F(ScSetTest, AppendElementWithTempArcTypeAndRemoveElementWithDefaultArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr, ScType::EdgeAccessConstPosTemp));
  EXPECT_TRUE(set.Remove(elementAddr));
  EXPECT_FALSE(set.Has(elementAddr));
  EXPECT_FALSE(set.Has(elementAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScSetTest, AppendElementWithTempArcTypeAndRemoveElementWithTempArcType)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(set.Append(elementAddr, ScType::EdgeAccessConstPosTemp));
  EXPECT_TRUE(set.Remove(elementAddr, ScType::EdgeAccessConstPosTemp));
  EXPECT_FALSE(set.Has(elementAddr));
  EXPECT_FALSE(set.Has(elementAddr, ScType::EdgeAccessConstPosTemp));
}

TEST_F(ScSetTest, RemoveNonExistingElement)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_FALSE(set.Remove(elementAddr));
}

TEST_F(ScSetTest, IsEmpty)
{
  ScSet set = m_ctx->CreateSet();
  EXPECT_TRUE(set.IsEmpty());
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr);
  EXPECT_FALSE(set.IsEmpty());
}

TEST_F(ScSetTest, GetPower)
{
  ScSet set = m_ctx->CreateSet();
  EXPECT_EQ(set.GetPower(), 0u);

  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr);
  EXPECT_EQ(set.GetPower(), 1u);

  set.Remove(elementAddr);
  EXPECT_EQ(set.GetPower(), 0u);
}

TEST_F(ScSetTest, UniteSetsWithoutCommonElements)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr1);

  ScSet otherSet = m_ctx->CreateSet();
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  otherSet.Append(elementAddr2);

  ScSet resultSet = set + otherSet;
  EXPECT_TRUE(resultSet.Has(elementAddr1));
  EXPECT_TRUE(resultSet.Has(elementAddr2));
}

TEST_F(ScSetTest, UniteSetsWithCommonElements)
{
  ScSet set1 = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set1.Append(elementAddr);

  ScSet set2 = m_ctx->CreateSet();
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  set2.Append(elementAddr2);
  set2.Append(elementAddr);

  ScSet resultSet = set1 + set2;
  EXPECT_TRUE(resultSet.Has(elementAddr));
  EXPECT_TRUE(resultSet.Has(elementAddr2));
}

TEST_F(ScSetTest, IntersectSetsWithoutCommonElements)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr1);

  ScSet otherSet = m_ctx->CreateSet();
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  otherSet.Append(elementAddr2);

  ScSet resultSet = set * otherSet;
  EXPECT_TRUE(resultSet.IsEmpty());
  EXPECT_TRUE(resultSet.Has(elementAddr1, ScType::EdgeAccessConstNegPerm));
  EXPECT_TRUE(resultSet.Has(elementAddr2, ScType::EdgeAccessConstNegPerm));
}

TEST_F(ScSetTest, IntersectSetsWithCommonElements)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr1);

  ScSet otherSet = m_ctx->CreateSet();
  otherSet.Append(elementAddr1);
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  otherSet.Append(elementAddr2);

  ScSet resultSet = set * otherSet;
  EXPECT_FALSE(resultSet.IsEmpty());
  EXPECT_EQ(resultSet.GetPower(), 1u);
  EXPECT_TRUE(resultSet.Has(elementAddr1));
  EXPECT_FALSE(resultSet.Has(elementAddr2));
}

TEST_F(ScSetTest, SubtractSetsWithCommonElements)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr);

  ScSet otherSet = m_ctx->CreateSet();
  otherSet.Append(elementAddr);

  ScSet resultSet = set - otherSet;
  EXPECT_TRUE(resultSet.IsEmpty());
  EXPECT_TRUE(resultSet.Has(elementAddr, ScType::EdgeAccessConstNegPerm));
}

TEST_F(ScSetTest, SubtractLargerSetWithCommonElement)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr1);

  ScSet otherSet = m_ctx->CreateSet();
  otherSet.Append(elementAddr1);
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  otherSet.Append(elementAddr2);

  ScSet resultSet = set - otherSet;
  EXPECT_TRUE(resultSet.IsEmpty());
  EXPECT_TRUE(resultSet.Has(elementAddr1, ScType::EdgeAccessConstNegPerm));
  EXPECT_TRUE(resultSet.Has(elementAddr2, ScType::EdgeAccessConstNegPerm));
}

TEST_F(ScSetTest, SubtractSmallerSetWithCommonElement)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr1 = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr1);
  ScAddr const & elementAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr2);

  ScSet otherSet = m_ctx->CreateSet();
  otherSet.Append(elementAddr1);

  ScSet resultSet = set - otherSet;
  EXPECT_FALSE(resultSet.IsEmpty());
  EXPECT_TRUE(resultSet.Has(elementAddr1, ScType::EdgeAccessConstNegPerm));
  EXPECT_TRUE(resultSet.Has(elementAddr2));
}

TEST_F(ScSetTest, SubtractSetFromItself)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr);

  ScSet resultSet = set - set;
  EXPECT_TRUE(resultSet.IsEmpty());
  EXPECT_TRUE(resultSet.Has(elementAddr, ScType::EdgeAccessConstNegPerm));
}

TEST_F(ScSetTest, CompareEqualSets)
{
  ScSet set = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set.Append(elementAddr);

  ScSet otherSet = m_ctx->CreateSet();
  otherSet.Append(elementAddr);

  EXPECT_EQ(set, otherSet);
}

TEST_F(ScSetTest, CompareNotEqualSets)
{
  ScSet set = m_ctx->CreateSet();
  set.Append(m_ctx->CreateNode(ScType::NodeConst));

  ScSet otherSet = m_ctx->CreateSet();
  otherSet.Append(m_ctx->CreateNode(ScType::NodeConst));

  EXPECT_NE(set, otherSet);
}

TEST_F(ScSetTest, CompareSetsWithSameElementsDifferentArcTypes)
{
  ScSet set1 = m_ctx->CreateSet();
  ScAddr const & elementAddr = m_ctx->CreateNode(ScType::NodeConst);
  set1.Append(elementAddr, ScType::EdgeAccessConstPosTemp);

  ScSet set2 = m_ctx->CreateSet();
  set2.Append(elementAddr, ScType::EdgeAccessConstNegPerm);

  EXPECT_NE(set1, set2);
}
