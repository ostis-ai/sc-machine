#include <gtest/gtest.h>

#include "sc-memory/sc_event.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_timer.hpp"

#include "event_test_utils.hpp"

#include <atomic>
#include <thread>
#include <mutex>

TEST(ScEventQueueTest, EventsQueueDestroy)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);
  params.clear = SC_TRUE;
  params.repo_path = "repo";
  params.log_level = "Debug";

  ScMemory::Initialize(params);

  ScMemoryContext ctx;

  ScAddr const node = ctx.CreateNode(ScType::NodeConst);
  ScAddr const node2 = ctx.CreateNode(ScType::NodeConst);
  ScAddr const node3 = ctx.CreateNode(ScType::NodeConst);

  size_t count = 10;

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt(
      ctx,
      node,
      [node, node2, count](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
        for (size_t i = 0; i < count; ++i)
          localCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, node2, node);
      });

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt2(
      ctx,
      node2,
      [node3, node2, count](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(node2, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
        for (size_t i = 0; i < count; ++i)
          localCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, node3, node2);
      });

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt3(
      ctx,
      node3,
      [node3](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(node3, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
      });

  for (size_t i = 0; i < count; ++i)
    ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);

  sleep(5);

  ctx.Destroy();
  ScMemory::Shutdown();
}

double const kTestTimeout = 0.1;

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionAddInputArc(ScMemoryContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [&]()
  {
    nodeAddr1 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->CreateEdge(eventArcType, nodeAddr2, nodeAddr1);
    EXPECT_TRUE(arcAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAddInputArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetAddedArc(), arcAddr);
    EXPECT_EQ(event.GetAddedArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr2);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr1);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionAddInputArc<subscriptionArcType> subscription(*ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, CreateEventSubscriptionAddInputArcAndInitiateEvent)
{
  EXPECT_TRUE(
      (TestEventSubscriptionAddInputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionAddInputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionAddInputArc<ScType::EdgeAccess, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionAddInputArc<ScType::EdgeDCommon, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeDCommonConst, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccess>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommon>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeUCommon, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddInputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommon>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionAddOutputArc(ScMemoryContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [&]()
  {
    nodeAddr1 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->CreateEdge(eventArcType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(arcAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAddOutputArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetAddedArc(), arcAddr);
    EXPECT_EQ(event.GetAddedArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr1);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionAddOutputArc<subscriptionArcType> subscription(*ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, CreateEventSubscriptionAddOutputArcAndInitiateEvent)
{
  EXPECT_TRUE(
      (TestEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionAddOutputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionAddOutputArc<ScType::EdgeAccess, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionAddOutputArc<ScType::EdgeDCommon, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeDCommonConst, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccess>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommon>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeUCommon, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddOutputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommon>(&*m_ctx)));
}

template <ScType const & subscriptionEdgeType, ScType const & eventEdgeType>
bool TestEventSubscriptionAddEdge(ScMemoryContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [&]()
  {
    nodeAddr1 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr edgeAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr2.IsValid());

    edgeAddr = ctx->CreateEdge(eventEdgeType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(edgeAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAddEdge<subscriptionEdgeType> const & event)
  {
    EXPECT_EQ(event.GetAddedEdge(), edgeAddr);
    EXPECT_EQ(event.GetAddedEdgeType(), eventEdgeType);
    EXPECT_TRUE(event.GetEdgeSourceElement() == nodeAddr1 || event.GetEdgeSourceElement() == nodeAddr2);
    EXPECT_TRUE(event.GetEdgeTargetElement() == nodeAddr1 || event.GetEdgeTargetElement() == nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionAddEdge<subscriptionEdgeType> subscription(*ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, CreateEventSubscriptionAddEdgeAndInitiateEvent)
{
  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeDCommonConst, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeAccess, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeDCommon, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeDCommonConst, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccess>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeDCommonConst, ScType::EdgeDCommon>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionAddEdge<ScType::EdgeUCommonConst, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionAddEdge<ScType::EdgeUCommon, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionAddEdge<ScType::EdgeUCommonConst, ScType::EdgeUCommon>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionRemoveInputArc(ScMemoryContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [&]()
  {
    nodeAddr1 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->CreateEdge(eventArcType, nodeAddr2, nodeAddr1);
    EXPECT_TRUE(arcAddr.IsValid());

    ctx->EraseElement(arcAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventRemoveInputArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetRemovedArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr2);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr1);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionRemoveInputArc<subscriptionArcType> subscription(*ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, CreateEventSubscriptionRemoveInputArcAndInitiateEvent)
{
  EXPECT_TRUE(
      (TestEventSubscriptionRemoveInputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionRemoveInputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionRemoveInputArc<ScType::EdgeAccess, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionRemoveInputArc<ScType::EdgeDCommon, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE(
      (TestEventSubscriptionRemoveInputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst>(&*m_ctx)));
  EXPECT_FALSE(
      (TestEventSubscriptionRemoveInputArc<ScType::EdgeDCommonConst, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveInputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccess>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveInputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommon>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveInputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveInputArc<ScType::EdgeUCommon, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveInputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommon>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionRemoveOutputArc(ScMemoryContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [&]()
  {
    nodeAddr1 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->CreateEdge(eventArcType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(arcAddr.IsValid());

    ctx->EraseElement(arcAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventRemoveOutputArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetRemovedArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr1);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionRemoveOutputArc<subscriptionArcType> subscription(*ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, CreateEventSubscriptionRemoveOutputArcAndInitiateEvent)
{
  EXPECT_TRUE(
      (TestEventSubscriptionRemoveOutputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeAccess, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeDCommon, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE(
      (TestEventSubscriptionRemoveOutputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst>(&*m_ctx)));
  EXPECT_FALSE(
      (TestEventSubscriptionRemoveOutputArc<ScType::EdgeDCommonConst, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccess>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeDCommonConst, ScType::EdgeDCommon>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeUCommon, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveOutputArc<ScType::EdgeUCommonConst, ScType::EdgeUCommon>(&*m_ctx)));
}

template <ScType const & subscriptionEdgeType, ScType const & eventEdgeType>
bool TestEventSubscriptionRemoveEdge(ScMemoryContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [&]()
  {
    nodeAddr1 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr edgeAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr2.IsValid());

    edgeAddr = ctx->CreateEdge(eventEdgeType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(edgeAddr.IsValid());

    ctx->EraseElement(edgeAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventRemoveEdge<subscriptionEdgeType> const & event)
  {
    EXPECT_EQ(event.GetRemovedEdgeType(), eventEdgeType);
    EXPECT_TRUE(event.GetEdgeSourceElement() == nodeAddr1 || event.GetEdgeSourceElement() == nodeAddr2);
    EXPECT_TRUE(event.GetEdgeTargetElement() == nodeAddr1 || event.GetEdgeTargetElement() == nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionRemoveEdge<subscriptionEdgeType> subscription(*ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, CreateEventSubscriptionRemoveEdgeAndInitiateEvent)
{
  EXPECT_FALSE(
      (TestEventSubscriptionRemoveEdge<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeDCommonConst, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeAccess, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeDCommon, ScType::EdgeDCommonConst>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeDCommonConst, ScType::EdgeAccessConstPosPerm>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeAccessConstPosPerm, ScType::EdgeAccess>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeDCommonConst, ScType::EdgeDCommon>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionRemoveEdge<ScType::EdgeUCommonConst, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionRemoveEdge<ScType::EdgeUCommon, ScType::EdgeUCommonConst>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionRemoveEdge<ScType::EdgeUCommonConst, ScType::EdgeUCommon>(&*m_ctx)));
}

TEST_F(ScEventTest, CreateEventSubscriptionRemoveElementAndInitiateEvent)
{
  ScAddr nodeAddr1;
  auto const & CreateNode = [this, &nodeAddr1]()
  {
    nodeAddr1 = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  auto const & EmitEvent = [this, &nodeAddr1]()
  {
    m_ctx->EraseElement(nodeAddr1);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventRemoveElement const & event)
  {
    EXPECT_FALSE(m_ctx->IsElement(nodeAddr1));
    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionRemoveElement subscription(*m_ctx, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, CreateEventSubscriptionChangeLinkContentAndInitiateEvent)
{
  ScAddr linkAddr;
  auto const & CreateNode = [this, &linkAddr]()
  {
    linkAddr = m_ctx->CreateLink(ScType::LinkConst);
    m_ctx->SetLinkContent(linkAddr, "old content");
    EXPECT_TRUE(linkAddr.IsValid());
  };

  auto const & EmitEvent = [this, &linkAddr]()
  {
    m_ctx->SetLinkContent(linkAddr, "new content");
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventChangeLinkContent const & event)
  {
    EXPECT_EQ(event.GetSubscriptionElement(), linkAddr);
    isDone = true;
  };

  CreateNode();

  ScEventSubscriptionChangeLinkContent subscription(*m_ctx, linkAddr, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, InvalidSubscriptions)
{
  ScAddr nodeAddr{23124323};

  EXPECT_THROW(
      ScEventSubscriptionAddOutputArc<ScType::EdgeAccess>(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(ScEventSubscriptionAddInputArc<ScType::EdgeAccess>(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(ScEventSubscriptionAddEdge<ScType::EdgeAccess>(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      ScEventSubscriptionRemoveOutputArc<ScType::EdgeAccess>(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      ScEventSubscriptionRemoveInputArc<ScType::EdgeAccess>(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(ScEventSubscriptionRemoveEdge<ScType::EdgeAccess>(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(ScEventSubscriptionRemoveElement(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(ScEventSubscriptionChangeLinkContent(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(ScEventSubscriptionChangeLinkContent(*m_ctx, nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScEventTest, DestroyOrder)
{
  ScAddr const node = m_ctx->CreateNode(ScType::Unknown);
  EXPECT_TRUE(node.IsValid());

  auto * evt = new ScEventSubscriptionAddOutputArc<ScType::EdgeAccess>(
      *m_ctx, node, [](ScEventAddOutputArc<ScType::EdgeAccess> const &) {});

  m_ctx.reset();

  // delete event after context
  delete evt;
}

TEST_F(ScEventTest, EventsLock)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt(
      *m_ctx,
      node,
      [node](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, ParallelCreateEdges)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt(
      *m_ctx,
      node,
      [node](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const & event)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          result = true;

        for (size_t i = 0; i < 10000; i++)
          localCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, event.GetArcTargetElement());

        EXPECT_TRUE(result);
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, ParallelCreateRemoveEdges)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt(
      *m_ctx,
      node,
      [node](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
        while (it->Next())
          localCtx.EraseElement(it->Get(1));
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, ParallelCreateRemoveEdges2)
{
  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const node2 = m_ctx->CreateNode(ScType::NodeConst);

  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> evt(
      *m_ctx,
      node,
      [](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const & event)
      {
        ScMemoryContext localCtx;
        localCtx.EraseElement(event.GetAddedArc());
      });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, node2);
}

TEST_F(ScEventTest, PendEvents)
{
  /* Main idea of test: create two sets with N elements, and add edges to relations.
   * Everytime, when event emits, we should check, that whole construction exist
   */
  ScAddr const set1 = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddr const rel = m_ctx->CreateNode(ScType::NodeConstNoRole);

  static size_t const el_num = 1 << 10;
  std::vector<ScAddr> elements(el_num);
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = m_ctx->CreateNode(ScType::NodeConst);
    EXPECT_TRUE(a.IsValid());
    elements[i] = a;
  }

  // create template for pending events check
  ScTemplate templ;
  for (auto const & a : elements)
  {
    templ.Quintuple(set1, ScType::EdgeDCommonVar, a >> "_el", ScType::EdgeAccessVarPosPerm, rel);
  }

  std::atomic_uint eventsCount(0);
  std::atomic_uint passedCount(0);

  ScEventSubscriptionAddOutputArc<ScType::EdgeDCommonConst> evt(
      *m_ctx,
      set1,
      [&passedCount, &eventsCount, &set1, &elements, &rel](ScEventAddOutputArc<ScType::EdgeDCommonConst> const &)
      {
        std::shared_ptr<ScTemplate> checkTempl(new ScTemplate());
        size_t step = 100;
        size_t testNum = el_num / step - 1;
        for (size_t i = 0; i < testNum; ++i)
        {
          checkTempl->Quintuple(
              set1, ScType::EdgeDCommonVar, elements[i * step] >> "_el", ScType::EdgeAccessVarPosPerm, rel);
        }

        ScMemoryContext localCtx;
        EXPECT_TRUE(localCtx.IsValid());

        ScTemplateSearchResult res;
        EXPECT_TRUE(localCtx.HelperSearchTemplate(*checkTempl, res));

        if (res.Size() == 1)
          passedCount.fetch_add(1);

        eventsCount.fetch_add(1);
      });

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(templ, genResult));

  // wait all events
  while (eventsCount.load() < el_num)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  EXPECT_EQ(passedCount, el_num);
}

TEST_F(ScEventTest, BlockEventsAndNotEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->CreateNode(ScType::NodeConst);

  std::atomic_bool isCalled = false;
  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> event(
      *m_ctx,
      nodeAddr,
      [&isCalled](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        isCalled = true;
      });

  m_ctx->BeingEventsBlocking();

  ScAddr const nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  m_ctx->EndEventsBlocking();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);
}

TEST_F(ScEventTest, BlockEventsAndEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->CreateNode(ScType::NodeConst);

  std::atomic_bool isCalled = false;
  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> event(
      *m_ctx,
      nodeAddr,
      [&isCalled](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        isCalled = true;
      });

  m_ctx->BeingEventsBlocking();

  ScAddr nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  m_ctx->EndEventsBlocking();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);

  nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(isCalled);
}

TEST_F(ScEventTest, BlockEventsGuardAndNotEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->CreateNode(ScType::NodeConst);

  std::atomic_bool isCalled = false;
  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> event(
      *m_ctx,
      nodeAddr,
      [&isCalled](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        isCalled = true;
      });

  ScMemoryContextEventsBlockingGuard guard(*m_ctx);

  ScAddr const nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);
}

TEST_F(ScEventTest, BlockEventsGuardAndEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->CreateNode(ScType::NodeConst);

  std::atomic_bool isCalled = false;
  ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> event(
      *m_ctx,
      nodeAddr,
      [&isCalled](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const &)
      {
        isCalled = true;
      });
  {
    ScMemoryContextEventsBlockingGuard guard(*m_ctx);

    ScAddr const nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(isCalled);
  }

  ScAddr const nodeAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(isCalled);
}
