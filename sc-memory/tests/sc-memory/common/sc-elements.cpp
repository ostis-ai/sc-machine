#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"


TEST_F(ScMemoryTest, elements)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "elements");

  ScAddr const node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.CreateLink();
  EXPECT_TRUE(link.IsValid());

  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(edge.IsValid());

  EXPECT_TRUE(ctx.IsElement(node));
  EXPECT_TRUE(ctx.IsElement(link));
  EXPECT_TRUE(ctx.IsElement(edge));

  EXPECT_EQ(ctx.GetEdgeSource(edge), node);
  EXPECT_EQ(ctx.GetEdgeTarget(edge), link);

  EXPECT_EQ(ctx.GetElementType(node), ScType::NodeConst);
  EXPECT_EQ(ctx.GetElementType(link), ScType::LinkConst);
  EXPECT_EQ(ctx.GetElementType(edge), ScType::EdgeAccessConstPosPerm);

  EXPECT_TRUE(ctx.SetElementSubtype(node, ScType::NodeVar));
  EXPECT_EQ(ctx.GetElementType(node), ScType::NodeVar);

  EXPECT_TRUE(ctx.SetElementSubtype(node, ScType::NodeConstStruct));
  EXPECT_EQ(ctx.GetElementType(node), ScType::NodeConstStruct);

  EXPECT_TRUE(ctx.SetElementSubtype(link, ScType::LinkVar));
  EXPECT_EQ(ctx.GetElementType(link), ScType::LinkVar);

  EXPECT_TRUE(ctx.SetElementSubtype(edge, ScType::EdgeAccessConstNegPerm));
  EXPECT_EQ(ctx.GetElementType(edge), ScType::EdgeAccessConstNegPerm);

  EXPECT_FALSE(ctx.SetElementSubtype(node, ScType::EdgeAccessConstFuzPerm));

  EXPECT_TRUE(ctx.EraseElement(node));
  EXPECT_TRUE(ctx.IsElement(link));
  EXPECT_FALSE(ctx.IsElement(node));
  EXPECT_FALSE(ctx.IsElement(edge));
}
