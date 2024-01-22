#include <gtest/gtest.h>

#include <filesystem>

#include "sc-memory/sc_memory.hpp"

extern "C"
{
#include "sc-core/sc-store/sc_storage.h"
#include "sc-core/sc-store/sc_storage_private.h"
}

#include "sc_test.hpp"

TEST_F(ScMemoryTest, Elements)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
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
  ScMemoryContext ctx;

  ScAddr const node{454545454455444};
  EXPECT_FALSE(ctx.IsElement(node));

  ScAddr const edge{454};
  EXPECT_FALSE(ctx.IsElement(edge));

  EXPECT_FALSE(ctx.EraseElement(node));
  EXPECT_THROW(ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeSource(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeTarget(node), utils::ExceptionInvalidParams);
  ScAddr sourceAddr, targetAddr;
  EXPECT_THROW(ctx.GetEdgeInfo(node, sourceAddr, targetAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementType(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSubtype(node, ScType::NodeConst), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(node, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(node, ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.HelperSetSystemIdtf("identifier", node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementOutputArcsCount(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementInputArcsCount(node), utils::ExceptionInvalidParams);

  EXPECT_FALSE(ctx.EraseElement(edge));
  EXPECT_THROW(ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, edge, edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeSource(edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeTarget(edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeInfo(edge, sourceAddr, targetAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementType(edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSubtype(edge, ScType::NodeConst), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(edge, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(edge, ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.HelperSetSystemIdtf("identifier", edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementOutputArcsCount(edge), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementInputArcsCount(edge), utils::ExceptionInvalidParams);

  EXPECT_FALSE(ctx.EraseElement(ScAddr::Empty));
  EXPECT_THROW(
      ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, ScAddr::Empty, ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeSource(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeTarget(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeInfo(ScAddr::Empty, sourceAddr, targetAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementType(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSubtype(ScAddr::Empty, ScType::NodeConst), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(ScAddr::Empty, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(ScAddr::Empty, ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.HelperSetSystemIdtf("identifier", ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementOutputArcsCount(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementInputArcsCount(ScAddr::Empty), utils::ExceptionInvalidParams);

  EXPECT_THROW(ctx.HelperResolveSystemIdtf("****"), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.HelperFindBySystemIdtf("****"), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.FindLinksByContent(ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.FindLinksByContentSubstring(ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.FindLinksContentsByContentSubstring(ScStreamPtr()), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryTest, NotEdge)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  EXPECT_THROW(ctx.GetEdgeSource(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetEdgeTarget(node), utils::ExceptionInvalidParams);
  ScAddr srcAddr, trgAddr;
  EXPECT_THROW(ctx.GetEdgeInfo(node, srcAddr, trgAddr), utils::ExceptionInvalidParams);
  EXPECT_FALSE(srcAddr.IsValid());
  EXPECT_FALSE(trgAddr.IsValid());
}

TEST_F(ScMemoryTest, NotLink)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  EXPECT_THROW(ctx.GetLinkContent(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(node, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(node), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryTest, CreateDeleteCountArcs)
{
  ScMemoryContext ctx;

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

TEST_F(ScMemoryTest, CreateDeleteCountArcs2)
{
  ScMemoryContext ctx;

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

TEST_F(ScMemoryTest, CreateDeleteCountEdges)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.CreateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.CreateLink();
  EXPECT_TRUE(link.IsValid());

  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 0u);

  ScAddr edge1 = ctx.CreateEdge(ScType::EdgeUCommonConst, node, link);
  EXPECT_TRUE(edge1.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 1u);

  ScAddr edge2 = ctx.CreateEdge(ScType::EdgeUCommonConst, node, link);
  EXPECT_TRUE(edge2.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 2u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 2u);

  EXPECT_TRUE(ctx.EraseElement(edge1));
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 1u);

  EXPECT_TRUE(ctx.EraseElement(edge2));
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 0u);

  ScIterator3Ptr it3 = ctx.Iterator3(node, ScType::EdgeUCommonConst, link);
  EXPECT_FALSE(it3->Next());

  it3 = ctx.Iterator3(link, ScType::EdgeUCommonConst, node);
  EXPECT_FALSE(it3->Next());

  ScAddr edge3 = ctx.CreateEdge(ScType::EdgeUCommonConst, node, link);
  EXPECT_TRUE(edge3.IsValid());
  EXPECT_EQ(ctx.GetElementOutputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementOutputArcsCount(link), 1u);
  EXPECT_EQ(ctx.GetElementInputArcsCount(link), 1u);
}

TEST_F(ScMemoryTest, CreateEdges)
{
  ScMemoryContext ctx;

  ScAddr const nodeAddr = ctx.CreateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());
  ScAddr const classAddr = ctx.CreateNode(ScType::NodeConstClass);
  EXPECT_TRUE(classAddr.IsValid());
  ScAddr const arcAddr1 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr);
  EXPECT_TRUE(arcAddr1.IsValid());

  ScAddr const linkAddr = ctx.CreateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr.IsValid());
  ScAddr const classLinkAddr = ctx.CreateLink(ScType::LinkClass);
  EXPECT_TRUE(classLinkAddr.IsValid());
  ScAddr const arcAddr2 = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, classLinkAddr, linkAddr);
  EXPECT_TRUE(arcAddr2.IsValid());

  ScAddr const edgeAddr = ctx.CreateEdge(ScType::EdgeUCommonConst, nodeAddr, linkAddr);
  EXPECT_TRUE(edgeAddr.IsValid());

  EXPECT_TRUE(ctx.HelperCheckEdge(classAddr, nodeAddr, ScType::EdgeAccessConstPosPerm));
  EXPECT_TRUE(ctx.HelperCheckEdge(classLinkAddr, linkAddr, ScType::EdgeAccessConstPosPerm));
  EXPECT_TRUE(ctx.HelperCheckEdge(nodeAddr, linkAddr, ScType::EdgeUCommonConst));
  EXPECT_TRUE(ctx.HelperCheckEdge(linkAddr, nodeAddr, ScType::EdgeUCommonConst));
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

  ScMemoryContext ctx;

  ScAddrList addrs;

  size_t count = SC_SEGMENT_ELEMENTS_COUNT / 3;
  try
  {
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
  }
  catch (...)
  {
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

  EXPECT_THROW(ctx.CreateNode(ScType::Const), utils::ExceptionCritical);

  ctx.Destroy();
  ScMemory::LogMute();
  ScMemory::Shutdown();
  ScMemory::LogUnmute();
}

TEST(SmallScMemoryTest, FullMemory2)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  params.max_loaded_segments = 2;

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();

  ScMemoryContext ctx;

  ScAddrList addrs;
  ScAddrList tempAddrs;

  size_t count = params.max_loaded_segments * SC_SEGMENT_ELEMENTS_COUNT / 3;
  sc_storage_start_new_process();
  try
  {
    for (size_t i = 0; i < count; ++i)
    {
      ScAddr const node = ctx.CreateNode(ScType::Const);
      if (!node.IsValid() && !ctx.IsElement(node))
        break;
      addrs.push_back(node);

      ScAddr const link = ctx.CreateLink();
      if (!link.IsValid() && !ctx.IsElement(link))
        break;
      tempAddrs.push_back(link);

      ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, link);
      if (!edge.IsValid() && !ctx.IsElement(edge))
        break;

      tempAddrs.push_back(edge);
    }
  }
  catch (...)
  {
  }
  sc_storage_end_new_process();

  sc_storage_start_new_process();
  count = tempAddrs.size();
  for (size_t i = 0; i < count; ++i)
  {
    ScAddr addr = tempAddrs.back();
    EXPECT_TRUE(ctx.IsElement(addr));
    EXPECT_TRUE(ctx.EraseElement(addr));
    tempAddrs.pop_back();
  }

  for (ScAddr const & addr : addrs)
  {
    EXPECT_TRUE(ctx.IsElement(addr));
  }

  for (size_t i = 0; i < count; ++i)
  {
    ScAddr const node = ctx.CreateNode(ScType::Const);
    EXPECT_TRUE(node.IsValid());
    EXPECT_TRUE(ctx.IsElement(node));
    EXPECT_TRUE(ctx.EraseElement(node));
  }

  for (ScAddr const & addr : addrs)
  {
    EXPECT_TRUE(ctx.IsElement(addr));
  }
  sc_storage_end_new_process();

  sc_storage_start_new_process();
  for (size_t i = 0; i < count; ++i)
  {
    ScAddr const node = ctx.CreateNode(ScType::Const);
    EXPECT_TRUE(node.IsValid());
    EXPECT_TRUE(ctx.IsElement(node));
  }

  for (ScAddr const & addr : addrs)
  {
    EXPECT_TRUE(ctx.IsElement(addr));
  }

  EXPECT_THROW(ctx.CreateNode(ScType::Const), utils::ExceptionCritical);

  for (ScAddr const & addr : addrs)
  {
    EXPECT_TRUE(ctx.IsElement(addr));
  }
  sc_storage_end_new_process();

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
  EXPECT_TRUE(sc_storage_is_initialized());
  ScMemory::LogUnmute();

  ScMemoryContext ctx;
  EXPECT_TRUE(ctx.IsValid());

  EXPECT_THROW(ctx.CreateNode(ScType::Const), utils::ExceptionCritical);
  EXPECT_THROW(ctx.CreateNode(ScType::Const), utils::ExceptionCritical);

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

  ScMemoryContext ctx;

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

TEST(ScMemoryDumper, DumpMemory)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  params.dump_memory = SC_TRUE;
  params.dump_memory_period = 4;
  params.dump_memory_statistics = SC_FALSE;

  params.max_loaded_segments = 1;

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();

  ScMemoryContext ctx;
  ctx.Save();
  ctx.Destroy();

  auto previousScMemorySaveTime = std::filesystem::last_write_time("repo/segments.scdb");
  sleep(10);
  auto currentScMemorySaveTime = std::filesystem::last_write_time("repo/segments.scdb");
  EXPECT_NE(previousScMemorySaveTime, currentScMemorySaveTime);
  previousScMemorySaveTime = currentScMemorySaveTime;
  sleep(10);
  currentScMemorySaveTime = std::filesystem::last_write_time("repo/segments.scdb");
  EXPECT_NE(previousScMemorySaveTime, currentScMemorySaveTime);
  previousScMemorySaveTime = currentScMemorySaveTime;
  sleep(10);
  currentScMemorySaveTime = std::filesystem::last_write_time("repo/segments.scdb");
  EXPECT_NE(previousScMemorySaveTime, currentScMemorySaveTime);

  ScMemory::LogMute();
  ScMemory::Shutdown();
  ScMemory::LogUnmute();
}

TEST(ScMemoryDumper, DumpMemoryStatistics)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  params.dump_memory = SC_FALSE;
  params.dump_memory_period = 4;
  params.dump_memory_statistics = SC_TRUE;
  params.dump_memory_statistics_period = 4;

  params.max_loaded_segments = 1;

  ScMemory::LogMute();
  ScMemory::Initialize(params);
  ScMemory::LogUnmute();

  ScMemoryContext ctx;
  ctx.Save();
  ctx.Destroy();

  auto const & previousScMemorySaveTime = std::filesystem::last_write_time("repo/segments.scdb");
  sleep(10);
  auto const & currentScMemorySaveTime = std::filesystem::last_write_time("repo/segments.scdb");
  EXPECT_EQ(previousScMemorySaveTime, currentScMemorySaveTime);

  ScMemory::LogMute();
  ScMemory::Shutdown();
  ScMemory::LogUnmute();
}
