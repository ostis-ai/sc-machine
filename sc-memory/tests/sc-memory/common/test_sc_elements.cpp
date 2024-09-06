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

  ScAddr const node = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.GenerateLink();
  EXPECT_TRUE(link.IsValid());

  ScAddr const arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(arcAddr.IsValid());

  EXPECT_TRUE(ctx.IsElement(node));
  EXPECT_TRUE(ctx.IsElement(link));
  EXPECT_TRUE(ctx.IsElement(arcAddr));

  EXPECT_EQ(ctx.GetArcSourceElement(arcAddr), node);
  EXPECT_EQ(ctx.GetArcTargetElement(arcAddr), link);

  EXPECT_EQ(ctx.GetElementType(node), ScType::NodeConst);
  EXPECT_EQ(ctx.GetElementType(link), ScType::LinkConst);
  EXPECT_EQ(ctx.GetElementType(arcAddr), ScType::EdgeAccessConstPosPerm);

  EXPECT_TRUE(ctx.SetElementSubtype(node, ScType::NodeVar));
  EXPECT_EQ(ctx.GetElementType(node), ScType::NodeVar);

  EXPECT_TRUE(ctx.SetElementSubtype(node, ScType::NodeConstStruct));
  EXPECT_EQ(ctx.GetElementType(node), ScType::NodeConstStruct);

  EXPECT_TRUE(ctx.SetElementSubtype(link, ScType::LinkVar));
  EXPECT_EQ(ctx.GetElementType(link), ScType::LinkVar);

  EXPECT_TRUE(ctx.SetElementSubtype(arcAddr, ScType::EdgeAccessConstNegPerm));
  EXPECT_EQ(ctx.GetElementType(arcAddr), ScType::EdgeAccessConstNegPerm);

  EXPECT_FALSE(ctx.SetElementSubtype(node, ScType::EdgeAccessConstFuzPerm));

  EXPECT_TRUE(ctx.EraseElement(node));
  EXPECT_TRUE(ctx.IsElement(link));
  EXPECT_FALSE(ctx.IsElement(node));
  EXPECT_FALSE(ctx.IsElement(arcAddr));
}

TEST_F(ScMemoryTest, InvalidElements)
{
  ScMemoryContext ctx;

  ScAddr const node{475585172};
  EXPECT_FALSE(ctx.IsElement(node));

  ScAddr const arcAddr{454};
  EXPECT_FALSE(ctx.IsElement(arcAddr));

  EXPECT_FALSE(ctx.EraseElement(node));
  EXPECT_THROW(ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcSourceElement(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcTargetElement(node), utils::ExceptionInvalidParams);

  EXPECT_THROW(ctx.GetConnectorIncidentElements(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementType(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSubtype(node, ScType::NodeConst), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(node, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(node, ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSystemIdentifier("identifier", node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementOutgoingArcsCount(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementIncomingArcsCount(node), utils::ExceptionInvalidParams);

  EXPECT_FALSE(ctx.EraseElement(arcAddr));
  EXPECT_THROW(ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, arcAddr, arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcSourceElement(arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcTargetElement(arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetConnectorIncidentElements(arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementType(arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSubtype(arcAddr, ScType::NodeConst), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(arcAddr, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(arcAddr, ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSystemIdentifier("identifier", arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementOutgoingArcsCount(arcAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementIncomingArcsCount(arcAddr), utils::ExceptionInvalidParams);

  EXPECT_FALSE(ctx.EraseElement(ScAddr::Empty));
  EXPECT_THROW(
      ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, ScAddr::Empty, ScAddr::Empty),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcSourceElement(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcTargetElement(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetConnectorIncidentElements(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementType(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSubtype(ScAddr::Empty, ScType::NodeConst), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(ScAddr::Empty, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(ScAddr::Empty, ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetElementSystemIdentifier("identifier", ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementOutgoingArcsCount(ScAddr::Empty), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetElementIncomingArcsCount(ScAddr::Empty), utils::ExceptionInvalidParams);

  EXPECT_THROW(ctx.ResolveElementSystemIdentifier("****"), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SearchElementBySystemIdentifier("****"), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SearchLinksByContent(ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SearchLinksByContentSubstring(ScStreamPtr()), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SearchLinksContentsByContentSubstring(ScStreamPtr()), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryTest, NotEdge)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_THROW(ctx.GetArcSourceElement(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetArcTargetElement(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetConnectorIncidentElements(node), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryTest, NotLink)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_THROW(ctx.GetLinkContent(node), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.SetLinkContent(node, ""), utils::ExceptionInvalidParams);
  EXPECT_THROW(ctx.GetLinkContent(node), utils::ExceptionInvalidParams);
}

TEST_F(ScMemoryTest, CreateDeleteCountArcs)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.GenerateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.GenerateLink();
  EXPECT_TRUE(link.IsValid());

  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 0u);

  ScAddr arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 1u);

  arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 2u);

  EXPECT_TRUE(ctx.EraseElement(arcAddr));
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 1u);

  arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);
  EXPECT_TRUE(arcAddr.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 2u);
}

TEST_F(ScMemoryTest, CreateDeleteCountArcs2)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.GenerateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.GenerateLink();
  EXPECT_TRUE(link.IsValid());

  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 0u);

  ScAddr arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);

  ScAddr const relation = ctx.GenerateNode(ScType::NodeConstRole);
  ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, relation, arcAddr);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(relation), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(relation), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(arcAddr), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(arcAddr), 1u);

  EXPECT_TRUE(ctx.EraseElement(arcAddr));
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(relation), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(relation), 0u);
}

TEST_F(ScMemoryTest, CreateDeleteCountEdges)
{
  ScMemoryContext ctx;

  ScAddr const node = ctx.GenerateNode(ScType::Const);
  EXPECT_TRUE(node.IsValid());

  ScAddr const link = ctx.GenerateLink();
  EXPECT_TRUE(link.IsValid());

  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 0u);

  ScAddr edge1 = ctx.GenerateConnector(ScType::EdgeUCommonConst, node, link);
  EXPECT_TRUE(edge1.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 1u);

  ScAddr edge2 = ctx.GenerateConnector(ScType::EdgeUCommonConst, node, link);
  EXPECT_TRUE(edge2.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 2u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 2u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 2u);

  EXPECT_TRUE(ctx.EraseElement(edge1));
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 1u);

  EXPECT_TRUE(ctx.EraseElement(edge2));
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 0u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 0u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 0u);

  ScIterator3Ptr it3 = ctx.Iterator3(node, ScType::EdgeUCommonConst, link);
  EXPECT_FALSE(it3->Next());

  it3 = ctx.Iterator3(link, ScType::EdgeUCommonConst, node);
  EXPECT_FALSE(it3->Next());

  ScAddr edge3 = ctx.GenerateConnector(ScType::EdgeUCommonConst, node, link);
  EXPECT_TRUE(edge3.IsValid());
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(node), 1u);
  EXPECT_EQ(ctx.GetElementOutgoingArcsCount(link), 1u);
  EXPECT_EQ(ctx.GetElementIncomingArcsCount(link), 1u);
}

TEST_F(ScMemoryTest, GenerateConnectors)
{
  ScMemoryContext ctx;

  ScAddr const nodeAddr = ctx.GenerateNode(ScType::NodeConst);
  EXPECT_TRUE(nodeAddr.IsValid());
  ScAddr const classAddr = ctx.GenerateNode(ScType::NodeConstClass);
  EXPECT_TRUE(classAddr.IsValid());
  ScAddr const arcAddr1 = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr);
  EXPECT_TRUE(arcAddr1.IsValid());

  ScAddr const linkAddr = ctx.GenerateLink(ScType::LinkConst);
  EXPECT_TRUE(linkAddr.IsValid());
  ScAddr const classLinkAddr = ctx.GenerateLink(ScType::LinkClass);
  EXPECT_TRUE(classLinkAddr.IsValid());
  ScAddr const arcAddr2 = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, classLinkAddr, linkAddr);
  EXPECT_TRUE(arcAddr2.IsValid());

  ScAddr const edgeAddr = ctx.GenerateConnector(ScType::EdgeUCommonConst, nodeAddr, linkAddr);
  EXPECT_TRUE(edgeAddr.IsValid());

  EXPECT_TRUE(ctx.CheckConnector(classAddr, nodeAddr, ScType::EdgeAccessConstPosPerm));
  EXPECT_TRUE(ctx.CheckConnector(classLinkAddr, linkAddr, ScType::EdgeAccessConstPosPerm));
  EXPECT_TRUE(ctx.CheckConnector(nodeAddr, linkAddr, ScType::EdgeUCommonConst));
  EXPECT_TRUE(ctx.CheckConnector(linkAddr, nodeAddr, ScType::EdgeUCommonConst));
}

TEST_F(ScMemoryTest, EraseEdgesBetweenTwoNodesByOneIterator)
{
  ScAddr const classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

  size_t const edgesCount = 5;
  for (size_t i = 0; i < edgesCount; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr);

  size_t i = 0;
  ScIterator3Ptr const it = m_ctx->Iterator3(classAddr, ScType::EdgeAccessConstPosPerm, nodeAddr);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(1));
    ++i;
  }

  EXPECT_EQ(i, edgesCount);
  EXPECT_EQ(m_ctx->GetElementOutgoingArcsCount(classAddr), 0u);
  EXPECT_EQ(m_ctx->GetElementIncomingArcsCount(nodeAddr), 0u);
}

TEST_F(ScMemoryTest, EraseEdgesBetweenTwoNodesByTwoIterators)
{
  ScAddr const classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

  size_t const edgesCount = 6;
  for (size_t i = 0; i < edgesCount; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr);

  size_t i = 0;
  ScIterator3Ptr it = m_ctx->Iterator3(classAddr, ScType::EdgeAccessConstPosPerm, nodeAddr);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(1));
    ++i;

    if (i == edgesCount / 2)
      break;
  }

  it = m_ctx->Iterator3(classAddr, ScType::EdgeAccessConstPosPerm, nodeAddr);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(1));
    ++i;
  }

  EXPECT_EQ(i, edgesCount);
  EXPECT_EQ(m_ctx->GetElementOutgoingArcsCount(classAddr), 0u);
  EXPECT_EQ(m_ctx->GetElementIncomingArcsCount(nodeAddr), 0u);
}

TEST_F(ScMemoryTest, EraseEdgesBetweenSomeNodesByOneIterator)
{
  ScAddr const classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const nodeAddr1 = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::NodeConst);

  size_t const edgesCount = 6;
  for (size_t i = 0; i < edgesCount / 2; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr1);

  for (size_t i = 0; i < edgesCount / 2; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr2);

  size_t i = 0;
  ScIterator3Ptr const it = m_ctx->Iterator3(classAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(1));
    ++i;
  }

  EXPECT_EQ(i, edgesCount);
  EXPECT_EQ(m_ctx->GetElementOutgoingArcsCount(classAddr), 0u);
  EXPECT_EQ(m_ctx->GetElementIncomingArcsCount(nodeAddr1), 0u);
  EXPECT_EQ(m_ctx->GetElementIncomingArcsCount(nodeAddr2), 0u);
}

TEST_F(ScMemoryTest, EraseEdgesFromNodeByOneIterator)
{
  ScAddr const classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

  size_t const edgesCount = 5;
  for (size_t i = 0; i < edgesCount; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr);

  size_t i = 0;
  ScIterator3Ptr const it = m_ctx->Iterator3(classAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(1));
    ++i;
  }

  EXPECT_EQ(i, edgesCount);
  EXPECT_EQ(m_ctx->GetElementOutgoingArcsCount(classAddr), 0u);
  EXPECT_EQ(m_ctx->GetElementIncomingArcsCount(nodeAddr), 0u);
}

TEST_F(ScMemoryTest, EraseEdgesToNodeByOneIterator)
{
  ScAddr const classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::NodeConst);

  size_t const edgesCount = 5;
  for (size_t i = 0; i < edgesCount; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr);

  size_t i = 0;
  ScIterator3Ptr const it = m_ctx->Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, nodeAddr);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(1));
    ++i;
  }

  EXPECT_EQ(i, edgesCount);
  EXPECT_EQ(m_ctx->GetElementOutgoingArcsCount(classAddr), 0u);
  EXPECT_EQ(m_ctx->GetElementIncomingArcsCount(nodeAddr), 0u);
}

TEST_F(ScMemoryTest, EraseEdgeTargetsBetweenSomeNodesByOneIterator)
{
  ScAddr const classAddr = m_ctx->GenerateNode(ScType::NodeConstClass);
  ScAddr const nodeAddr1 = m_ctx->GenerateNode(ScType::NodeConst);
  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::NodeConst);

  size_t const edgesCount = 6;
  for (size_t i = 0; i < edgesCount / 2; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr1);

  for (size_t i = 0; i < edgesCount / 2; ++i)
    m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, classAddr, nodeAddr2);

  size_t i = 0;
  ScIterator3Ptr const it = m_ctx->Iterator3(classAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while (it->Next())
  {
    m_ctx->EraseElement(it->Get(2));
    ++i;
  }

  EXPECT_EQ(i, 2u);
  EXPECT_EQ(m_ctx->GetElementOutgoingArcsCount(classAddr), 0u);
  EXPECT_FALSE(m_ctx->IsElement(nodeAddr1));
  EXPECT_FALSE(m_ctx->IsElement(nodeAddr2));
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
      ScAddr const node = ctx.GenerateNode(ScType::Const);
      if (!node.IsValid() && !ctx.IsElement(node))
        break;

      ScAddr const link = ctx.GenerateLink();
      if (!link.IsValid() && !ctx.IsElement(link))
        break;

      ScAddr const arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);
      if (!arcAddr.IsValid() && !ctx.IsElement(arcAddr))
        break;
      addrs.push_back(arcAddr);
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
    ScAddr const node = ctx.GenerateNode(ScType::Const);
    EXPECT_TRUE(node.IsValid());
    EXPECT_TRUE(ctx.IsElement(node));
  }

  EXPECT_THROW(ctx.GenerateNode(ScType::Const), utils::ExceptionCritical);

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
      ScAddr const node = ctx.GenerateNode(ScType::Const);
      if (!node.IsValid() && !ctx.IsElement(node))
        break;
      addrs.push_back(node);

      ScAddr const link = ctx.GenerateLink();
      if (!link.IsValid() && !ctx.IsElement(link))
        break;
      tempAddrs.push_back(link);

      ScAddr const arcAddr = ctx.GenerateConnector(ScType::EdgeAccessConstPosPerm, node, link);
      if (!arcAddr.IsValid() && !ctx.IsElement(arcAddr))
        break;

      tempAddrs.push_back(arcAddr);
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
    ScAddr const node = ctx.GenerateNode(ScType::Const);
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
    ScAddr const node = ctx.GenerateNode(ScType::Const);
    EXPECT_TRUE(node.IsValid());
    EXPECT_TRUE(ctx.IsElement(node));
  }

  for (ScAddr const & addr : addrs)
  {
    EXPECT_TRUE(ctx.IsElement(addr));
  }

  EXPECT_THROW(ctx.GenerateNode(ScType::Const), utils::ExceptionCritical);

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

  EXPECT_THROW(ctx.GenerateNode(ScType::Const), utils::ExceptionCritical);
  EXPECT_THROW(ctx.GenerateNode(ScType::Const), utils::ExceptionCritical);

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
  ScAddr node = ctx.GenerateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  node = ctx.GenerateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  EXPECT_TRUE(ctx.EraseElement(node));
  EXPECT_FALSE(ctx.IsElement(node));
  node = ctx.GenerateNode(ScType::Const);
  EXPECT_TRUE(ctx.IsElement(node));
  sc_storage_end_new_process();

  sc_storage_start_new_process();
  node = ctx.GenerateNode(ScType::Const);
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
