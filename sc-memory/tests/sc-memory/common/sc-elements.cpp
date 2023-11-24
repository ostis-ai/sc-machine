#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

extern "C"
{
#include "sc-core/sc-store/sc_storage.h"
}

#include "sc_test.hpp"


TEST_F(ScMemoryTest, Elements)
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

TEST_F(ScMemoryTest, InvalidElements)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "elements");

  ScAddr const node{454545454455444};
  EXPECT_FALSE(ctx.IsElement(node));

  ScAddr const edge{454};
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

TEST(SmallScMemoryTest, FullMemory)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  params.max_loaded_segments = 1;

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();

  ScMemoryContext ctx(sc_access_lvl_make_min);

  ScAddrList addrs;

  size_t count = SC_SEGMENT_ELEMENTS_COUNT / 3;
  for (size_t i = 0; i < count; ++i)
  {
    ScAddr const node = ctx.CreateNode(ScType::Const);
    if (!node.IsValid() && !ctx.IsElement(node))
      break;

    ScAddr const link = ctx.CreateLink();
    if (!link.IsValid() && !ctx.IsElement(link))
      break;

    ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);
    if (!edge.IsValid() && !ctx.IsElement(edge))
      break;
    addrs.push_back(edge);
  }

  count = addrs.size();
  for (size_t i = 0; i < count; ++i)
  {
    ScAddr addr = addrs.back();
    ctx.EraseElement(addr);
    addrs.pop_back();
  }

  for (size_t i = 0; i < count; ++i)
  {
    ScAddr const node = ctx.CreateNode(ScType::Const);
    EXPECT_TRUE(node.IsValid());
    EXPECT_TRUE(ctx.IsElement(node));
  }

  ScAddr const node = ctx.CreateNode(ScType::Const);
  EXPECT_FALSE(node.IsValid());
  EXPECT_FALSE(ctx.IsElement(node));

  ctx.Destroy();
  ScMemory::LogMute();
  ScMemory::Shutdown();
  ScMemory::LogUnmute();
}

TEST(SmallScMemoryTest, EmptyMemory)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  params.max_loaded_segments = 0;

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();

  ScMemoryContext ctx(sc_access_lvl_make_min);

  ScAddrList addrs;

  size_t count = SC_SEGMENT_ELEMENTS_COUNT / 3;
  for (size_t i = 0; i < count; ++i)
  {
    ScAddr const node = ctx.CreateNode(ScType::Const);
    if (!node.IsValid() && !ctx.IsElement(node))
      break;
  }

  EXPECT_EQ(addrs.size(), 0u);

  ScAddr const node = ctx.CreateNode(ScType::Const);
  EXPECT_FALSE(node.IsValid());
  EXPECT_FALSE(ctx.IsElement(node));

  ctx.Destroy();
  ScMemory::LogMute();
  ScMemory::Shutdown();
  ScMemory::LogUnmute();
}

TEST(SmallScMemoryTest, DistributedMemory)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  params.max_loaded_segments = 1;

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();

  ScMemoryContext ctx(sc_access_lvl_make_min);

  sc_storage_start_new_process();
  ScAddr node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  EXPECT_TRUE(ctx.EraseElement(node));
  EXPECT_FALSE(ctx.IsElement(node));
  node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  sc_storage_end_new_process();

  sc_storage_start_new_process();
  node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  EXPECT_TRUE(ctx.EraseElement(node));
  EXPECT_FALSE(ctx.IsElement(node));
  sc_storage_end_new_process();

  ctx.Destroy();
  ScMemory::LogMute();
  ScMemory::Shutdown();
  ScMemory::LogUnmute();
}
