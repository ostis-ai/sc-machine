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

TEST_F(ScMemoryTest, CreateDeleteCountEdges)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "CreateDeleteCountEdges");

  ScAddr const node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.CreateLink();
  EXPECT_TRUE(link.IsValid());

  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 0u);

  ScAddr edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(edge.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 1u);

  edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(edge.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 2u);

  EXPECT_TRUE(ctx.EraseElement(edge));
  EXPECT_TRUE(edge.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 1u);

  edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(edge.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 2u);
}

TEST_F(ScMemoryTest, CreateDeleteCountEdges2)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "CreateDeleteCountEdges");

  ScAddr const node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.CreateLink();
  EXPECT_TRUE(link.IsValid());

  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 0u);

  ScAddr edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);

  ScAddr const relation = ctx.CreateNode(ScType::NodeConstRole);
  ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, relation, edge);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(relation), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(relation), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(edge), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(edge), 1u);

  EXPECT_TRUE(ctx.EraseElement(edge));
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(relation), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(relation), 0u);
}
