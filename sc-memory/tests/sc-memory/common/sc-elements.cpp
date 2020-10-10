#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"


TEST_F(ScMemoryTest, elements)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "elements");

  ScAddr const addr = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(addr.IsValid());

  ScAddr const link = ctx.CreateLink();
  EXPECT_TRUE(link.IsValid());

  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, addr, link);
  EXPECT_TRUE(edge.IsValid());

  EXPECT_TRUE(ctx.IsElement(addr));
  EXPECT_TRUE(ctx.IsElement(link));
  EXPECT_TRUE(ctx.IsElement(edge));

  EXPECT_EQ(ctx.GetEdgeSource(edge), addr);
  EXPECT_EQ(ctx.GetEdgeTarget(edge), link);

  EXPECT_EQ(ctx.GetElementType(addr), ScType::NodeConst);
  EXPECT_EQ(ctx.GetElementType(link), ScType::LinkConst);
  EXPECT_EQ(ctx.GetElementType(edge), ScType::EdgeAccessConstPosPerm);

  EXPECT_TRUE(ctx.SetElementSubtype(addr, ScType::Var));
  EXPECT_EQ(ctx.GetElementType(addr), ScType::NodeVar);

  EXPECT_TRUE(ctx.EraseElement(addr));
  EXPECT_TRUE(ctx.IsElement(link));
  EXPECT_FALSE(ctx.IsElement(addr));
  EXPECT_FALSE(ctx.IsElement(edge));
}
