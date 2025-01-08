/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/sc_event.hpp>
#include <sc-memory/sc_event_subscription.hpp>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_timer.hpp>

#include "event_test_utils.hpp"

#include <atomic>
#include <thread>
#include <mutex>

TEST(ScEventQueueTest, EventsQueueDestroy)
{
  sc_memory_params params;
  sc_memory_params_clear(&params);
  params.clear = SC_TRUE;
  params.storage = "repo";
  params.log_level = "Debug";

  ScMemory::Initialize(params);

  ScAgentContext ctx;

  ScAddr const node = ctx.GenerateNode(ScType::ConstNode);
  ScAddr const node2 = ctx.GenerateNode(ScType::ConstNode);
  ScAddr const node3 = ctx.GenerateNode(ScType::ConstNode);

  size_t count = 10;

  auto eventSubscription =
      ctx.CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          node,
          [node, node2, count](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            bool result = false;
            ScMemoryContext localCtx;
            ScIterator3Ptr it = localCtx.CreateIterator3(node, ScType::ConstPermPosArc, ScType::Unknown);
            while (it->Next())
              result = true;

            EXPECT_TRUE(result);
            for (size_t i = 0; i < count; ++i)
              localCtx.GenerateConnector(ScType::ConstPermPosArc, node2, node);
          });

  eventSubscription = ctx.CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
      node2,
      [node3, node2, count](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.CreateIterator3(node2, ScType::ConstPermPosArc, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
        for (size_t i = 0; i < count; ++i)
          localCtx.GenerateConnector(ScType::ConstPermPosArc, node3, node2);
      });

  eventSubscription = ctx.CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
      node3,
      [node3](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
      {
        bool result = false;
        ScMemoryContext localCtx;
        ScIterator3Ptr it = localCtx.CreateIterator3(node3, ScType::ConstPermPosArc, ScType::Unknown);
        while (it->Next())
          result = true;

        EXPECT_TRUE(result);
      });

  for (size_t i = 0; i < count; ++i)
    ctx.GenerateConnector(ScType::ConstPermPosArc, node, node2);

  sleep(5);

  ctx.Destroy();
  ScMemory::Shutdown();
}

double const kTestTimeout = 0.1;

template <ScType const & subscriptionConnectorType, ScType const & eventConnectorType>
bool TestEventSubscriptionGenerateConnector(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->GenerateConnector(eventConnectorType, nodeAddr2, nodeAddr1);
    EXPECT_TRUE(arcAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAfterGenerateConnector<subscriptionConnectorType> const & event)
  {
    EXPECT_EQ(event.GetConnector(), arcAddr);
    EXPECT_EQ(event.GetConnectorType(), eventConnectorType);
    auto [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
    EXPECT_TRUE(elementAddr1 == nodeAddr1 || elementAddr2 == nodeAddr1);
    EXPECT_TRUE(elementAddr1 == nodeAddr2 || elementAddr2 == nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventAfterGenerateConnector<subscriptionConnectorType>>(
          nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionGenerateConnectorAndInitiateEvent)
{
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateConnector<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateConnector<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateConnector<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateConnector<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateConnector<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateConnector<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionGenerateIncomingArc(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->GenerateConnector(eventArcType, nodeAddr2, nodeAddr1);
    EXPECT_TRUE(arcAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAfterGenerateIncomingArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetArc(), arcAddr);
    EXPECT_EQ(event.GetArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr2);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr1);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventAfterGenerateIncomingArc<subscriptionArcType>>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionGenerateIncomingArcAndInitiateEvent)
{
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateIncomingArc<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateIncomingArc<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionGenerateOutgoingArc(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->GenerateConnector(eventArcType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(arcAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAfterGenerateOutgoingArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetArc(), arcAddr);
    EXPECT_EQ(event.GetArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr1);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<subscriptionArcType>>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionGenerateOutgoingArcAndInitiateEvent)
{
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateOutgoingArc<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateOutgoingArc<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionEdgeType, ScType const & eventEdgeType>
bool TestEventSubscriptionGenerateEdge(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr edgeAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    edgeAddr = ctx->GenerateConnector(eventEdgeType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(edgeAddr.IsValid());
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventAfterGenerateEdge<subscriptionEdgeType> const & event)
  {
    EXPECT_EQ(event.GetEdge(), edgeAddr);
    EXPECT_EQ(event.GetEdgeType(), eventEdgeType);
    auto [elementAddr1, elementAddr2] = event.GetEdgeIncidentElements();
    EXPECT_TRUE(elementAddr1 == nodeAddr1 || elementAddr2 == nodeAddr1);
    EXPECT_TRUE(elementAddr1 == nodeAddr2 || elementAddr2 == nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventAfterGenerateEdge<subscriptionEdgeType>>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionGenerateEdgeAndInitiateEvent)
{
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionGenerateEdge<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateEdge<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateEdge<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionGenerateEdge<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionGenerateEdge<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionConnectorType, ScType const & eventConnectorType>
bool TestEventSubscriptionEraseConnector(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->GenerateConnector(eventConnectorType, nodeAddr2, nodeAddr1);
    EXPECT_TRUE(arcAddr.IsValid());

    ctx->EraseElement(arcAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseConnector<subscriptionConnectorType> const & event)
  {
    EXPECT_EQ(event.GetConnector(), arcAddr);
    EXPECT_TRUE(ctx->IsElement(arcAddr));
    EXPECT_EQ(event.GetConnectorType(), eventConnectorType);
    auto [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
    EXPECT_TRUE(elementAddr1 == nodeAddr1 || elementAddr2 == nodeAddr1);
    EXPECT_TRUE(elementAddr1 == nodeAddr2 || elementAddr2 == nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventBeforeEraseConnector<subscriptionConnectorType>>(
          nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionEraseConnectorAndInitiateEvent)
{
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseConnector<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseConnector<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseConnector<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseConnector<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseConnector<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseConnector<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionEraseIncomingArc(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->GenerateConnector(eventArcType, nodeAddr2, nodeAddr1);
    EXPECT_TRUE(arcAddr.IsValid());

    ctx->EraseElement(arcAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseIncomingArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetArc(), arcAddr);
    EXPECT_TRUE(ctx->IsElement(arcAddr));
    EXPECT_EQ(event.GetArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr2);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr1);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventBeforeEraseIncomingArc<subscriptionArcType>>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionEraseIncomingArcAndInitiateEvent)
{
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseIncomingArc<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseIncomingArc<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionArcType, ScType const & eventArcType>
bool TestEventSubscriptionEraseOutgoingArc(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr arcAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    arcAddr = ctx->GenerateConnector(eventArcType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(arcAddr.IsValid());

    ctx->EraseElement(arcAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseOutgoingArc<subscriptionArcType> const & event)
  {
    EXPECT_EQ(event.GetArc(), arcAddr);
    EXPECT_TRUE(ctx->IsElement(arcAddr));
    EXPECT_EQ(event.GetArcType(), eventArcType);
    EXPECT_EQ(event.GetArcSourceElement(), nodeAddr1);
    EXPECT_EQ(event.GetArcTargetElement(), nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<subscriptionArcType>>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionEraseOutgoingArcAndInitiateEvent)
{
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseOutgoingArc<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseOutgoingArc<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

template <ScType const & subscriptionEdgeType, ScType const & eventEdgeType>
bool TestEventSubscriptionEraseEdge(ScAgentContext * ctx)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [&]()
  {
    nodeAddr1 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  ScAddr nodeAddr2;
  ScAddr edgeAddr;
  auto const & EmitEvent = [&]()
  {
    nodeAddr2 = ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr2.IsValid());

    edgeAddr = ctx->GenerateConnector(eventEdgeType, nodeAddr1, nodeAddr2);
    EXPECT_TRUE(edgeAddr.IsValid());

    ctx->EraseElement(edgeAddr);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseEdge<subscriptionEdgeType> const & event)
  {
    EXPECT_EQ(event.GetEdge(), edgeAddr);
    EXPECT_TRUE(ctx->IsElement(edgeAddr));
    EXPECT_EQ(event.GetEdgeType(), eventEdgeType);
    auto [elementAddr1, elementAddr2] = event.GetEdgeIncidentElements();
    EXPECT_TRUE(elementAddr1 == nodeAddr1 || elementAddr2 == nodeAddr1);
    EXPECT_TRUE(elementAddr1 == nodeAddr2 || elementAddr2 == nodeAddr2);
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      ctx->CreateElementaryEventSubscription<ScEventBeforeEraseEdge<subscriptionEdgeType>>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  return isDone;
}

TEST_F(ScEventTest, GenerateEventSubscriptionEraseEdgeAndInitiateEvent)
{
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::Unknown, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::Const, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstPermPosArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::Unknown, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::Const, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstCommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::MembershipArc, ScType::ConstPermPosArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::CommonArc, ScType::ConstCommonArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstPermPosArc, ScType::ConstCommonArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstCommonArc, ScType::ConstPermPosArc>(&*m_ctx)));

  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstPermPosArc, ScType::MembershipArc>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstCommonArc, ScType::CommonArc>(&*m_ctx)));

  EXPECT_TRUE((TestEventSubscriptionEraseEdge<ScType::Unknown, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseEdge<ScType::Const, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseEdge<ScType::ConstCommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_TRUE((TestEventSubscriptionEraseEdge<ScType::CommonEdge, ScType::ConstCommonEdge>(&*m_ctx)));
  EXPECT_FALSE((TestEventSubscriptionEraseEdge<ScType::ConstCommonEdge, ScType::CommonEdge>(&*m_ctx)));
}

TEST_F(ScEventTest, GenerateEventSubscriptionEraseElementAndInitiateEvent)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [this, &nodeAddr1]()
  {
    nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  auto const & EmitEvent = [this, &nodeAddr1]()
  {
    m_ctx->EraseElement(nodeAddr1);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseElement const & event)
  {
    EXPECT_TRUE(m_ctx->IsElement(nodeAddr1));
    isDone = true;
  };

  GenerateNode();

  auto eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseElement>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, GenerateEventSubscriptionEraseElementAndInitiateEventAndGetRemovedElementType)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [this, &nodeAddr1]()
  {
    nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  auto const & EmitEvent = [this, &nodeAddr1]()
  {
    m_ctx->EraseElement(nodeAddr1);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseElement const & event)
  {
    EXPECT_EQ(event.GetSubscriptionElement(), nodeAddr1);
    EXPECT_TRUE(m_ctx->IsElement(nodeAddr1));
    EXPECT_NO_THROW(m_ctx->GetElementType(nodeAddr1));

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseElement>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, GenerateEventSubscriptionWithStaticEventEraseElementAndInitiateEventAndThrowException)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [this, &nodeAddr1]()
  {
    nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  auto const & EmitEvent = [this, &nodeAddr1]()
  {
    m_ctx->EraseElement(nodeAddr1);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeEraseElement const &)
  {
    isDone = true;
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Test exception");
  };

  GenerateNode();

  auto eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseElement>(nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, GenerateEventSubscriptionWithDynamicEventEraseElementAndInitiateEventAndGetRemovedElementType)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [this, &nodeAddr1]()
  {
    nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  auto const & EmitEvent = [this, &nodeAddr1]()
  {
    m_ctx->EraseElement(nodeAddr1);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScElementaryEvent const & event)
  {
    EXPECT_TRUE(m_ctx->IsElement(nodeAddr1));
    m_ctx->GetElementType(nodeAddr1);

    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription(ScKeynodes::sc_event_before_erase_element, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, GenerateEventSubscriptionWithDynamicEventEraseElementAndInitiateEventAndThrowException)
{
  ScAddr nodeAddr1;
  auto const & GenerateNode = [this, &nodeAddr1]()
  {
    nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(nodeAddr1.IsValid());
  };

  auto const & EmitEvent = [this, &nodeAddr1]()
  {
    m_ctx->EraseElement(nodeAddr1);
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScElementaryEvent const &)
  {
    isDone = true;
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Test exception");
  };

  GenerateNode();

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription(ScKeynodes::sc_event_before_erase_element, nodeAddr1, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, GenerateEventSubscriptionChangeLinkContentAndInitiateEventSetNewContent)
{
  ScAddr linkAddr;
  auto const & GenerateNode = [this, &linkAddr]()
  {
    linkAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
    m_ctx->SetLinkContent(linkAddr, "old content");
    EXPECT_TRUE(linkAddr.IsValid());
  };

  auto const & EmitEvent = [this, &linkAddr]()
  {
    m_ctx->SetLinkContent(linkAddr, "new content");
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeChangeLinkContent const & event)
  {
    EXPECT_EQ(event.GetSubscriptionElement(), linkAddr);
    isDone = true;
  };

  GenerateNode();

  auto eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventBeforeChangeLinkContent>(linkAddr, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, GenerateEventSubscriptionChangeLinkContentAndInitiateEventSetTheSameContent)
{
  ScAddr linkAddr;
  auto const & GenerateNode = [this, &linkAddr]()
  {
    linkAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
    m_ctx->SetLinkContent(linkAddr, "old content");
    EXPECT_TRUE(linkAddr.IsValid());
  };

  auto const & EmitEvent = [this, &linkAddr]()
  {
    m_ctx->SetLinkContent(linkAddr, "old content");
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeChangeLinkContent const & event)
  {
    EXPECT_EQ(event.GetSubscriptionElement(), linkAddr);
    isDone = true;
  };

  GenerateNode();

  auto eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventBeforeChangeLinkContent>(linkAddr, OnEvent);
  ScTimer timer(kTestTimeout);

  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
}

TEST_F(ScEventTest, InvalidSubscriptions)
{
  ScAddr nodeAddr;

  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateConnector<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateIncomingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateEdge<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseConnector<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseIncomingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseEdge<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeChangeLinkContent>(nodeAddr, {}),
      utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeChangeLinkContent>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
}

TEST_F(ScEventTest, InvalidEvents)
{
  ScAddr nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr eventClassAddr;
  EXPECT_THROW(m_ctx->CreateElementaryEventSubscription(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr.Reset();
  eventClassAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateElementaryEventSubscription(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  eventClassAddr = ScKeynodes::sc_event_before_change_link_content;
  EXPECT_THROW(m_ctx->CreateElementaryEventSubscription(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateElementaryEventSubscription(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScEventTest, SetRemoveDelegateFuncForSubscriptionWithStaticEventType)
{
  ScAddr linkAddr;
  auto const & GenerateNode = [this, &linkAddr]()
  {
    linkAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
    m_ctx->SetLinkContent(linkAddr, "old content");
    EXPECT_TRUE(linkAddr.IsValid());
  };

  auto const & EmitEvent = [this, &linkAddr]()
  {
    m_ctx->SetLinkContent(linkAddr, "new content");
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScEventBeforeChangeLinkContent const & event)
  {
    EXPECT_EQ(event.GetSubscriptionElement(), linkAddr);
    isDone = true;
  };

  GenerateNode();
  auto eventSubscription = m_ctx->CreateElementaryEventSubscription<ScEventBeforeChangeLinkContent>(linkAddr, {});

  ScTimer timer(kTestTimeout);
  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(isDone);

  eventSubscription->SetDelegate(OnEvent);

  timer = ScTimer(kTestTimeout);
  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
  isDone = false;

  eventSubscription->RemoveDelegate();

  timer = ScTimer(kTestTimeout);
  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(isDone);
}

TEST_F(ScEventTest, SetRemoveDelegateFuncForSubscriptionWithDynamicEventType)
{
  ScAddr linkAddr;
  auto const & GenerateNode = [this, &linkAddr]()
  {
    linkAddr = m_ctx->GenerateLink(ScType::ConstNodeLink);
    m_ctx->SetLinkContent(linkAddr, "old content");
    EXPECT_TRUE(linkAddr.IsValid());
  };

  auto const & EmitEvent = [this, &linkAddr]()
  {
    m_ctx->SetLinkContent(linkAddr, "new content");
  };

  bool isDone = false;
  auto const & OnEvent = [&](ScElementaryEvent const & event)
  {
    EXPECT_EQ(event.GetSubscriptionElement(), linkAddr);
    isDone = true;
  };

  GenerateNode();

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription(ScKeynodes::sc_event_before_change_link_content, linkAddr, {});

  ScTimer timer(kTestTimeout);
  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(isDone);

  eventSubscription->SetDelegate(OnEvent);

  timer = ScTimer(kTestTimeout);
  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(isDone);
  isDone = false;

  eventSubscription->RemoveDelegate();

  timer = ScTimer(kTestTimeout);
  EmitEvent();

  while (!isDone && !timer.IsTimeOut())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(isDone);
}

TEST_F(ScEventTest, DestroyOrder)
{
  ScAddr const node = m_ctx->GenerateNode(ScType::Unknown);
  EXPECT_TRUE(node.IsValid());

  auto evt = m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(
      node, [](ScEventAfterGenerateOutgoingArc<ScType::MembershipArc> const &) {});

  m_ctx.reset();

  // delete event after context
}

TEST_F(ScEventTest, EventsLock)
{
  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const node2 = m_ctx->GenerateNode(ScType::ConstNode);

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          node,
          [node](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            bool result = false;
            ScMemoryContext localCtx;
            ScIterator3Ptr it = localCtx.CreateIterator3(node, ScType::ConstPermPosArc, ScType::Unknown);
            while (it->Next())
              result = true;

            EXPECT_TRUE(result);
          });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, node2);
}

TEST_F(ScEventTest, ParallelGenerateConnectors)
{
  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const node2 = m_ctx->GenerateNode(ScType::ConstNode);

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          node,
          [node](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event)
          {
            bool result = false;
            ScMemoryContext localCtx;
            ScIterator3Ptr it = localCtx.CreateIterator3(node, ScType::ConstPermPosArc, ScType::Unknown);
            while (it->Next())
              result = true;

            for (size_t i = 0; i < 10000; i++)
              localCtx.GenerateConnector(ScType::ConstPermPosArc, node, event.GetArcTargetElement());

            EXPECT_TRUE(result);
          });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, node2);
}

TEST_F(ScEventTest, ParallelGenerateEraseConnectors)
{
  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const node2 = m_ctx->GenerateNode(ScType::ConstNode);

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          node,
          [node](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            ScMemoryContext localCtx;
            ScIterator3Ptr it = localCtx.CreateIterator3(node, ScType::ConstPermPosArc, ScType::Unknown);
            while (it->Next())
              localCtx.EraseElement(it->Get(1));
          });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, node2);
}

TEST_F(ScEventTest, ParallelGenerateEraseConnectors2)
{
  ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const node2 = m_ctx->GenerateNode(ScType::ConstNode);

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          node,
          [](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event)
          {
            ScMemoryContext localCtx;
            localCtx.EraseElement(event.GetArc());
          });

  for (size_t i = 0; i < 10000; i++)
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, node2);
}

TEST_F(ScEventTest, PendEvents)
{
  /* Main idea of test: generate two sets with N elements, and add arcs to relations.
   * Every time, when event emits, we should check, that whole construction exist
   */
  ScAddr const set1 = m_ctx->GenerateNode(ScType::ConstNodeClass);
  ScAddr const rel = m_ctx->GenerateNode(ScType::ConstNodeNonRole);

  static size_t const el_num = 1 << 10;
  std::vector<ScAddr> elements(el_num);
  for (size_t i = 0; i < el_num; ++i)
  {
    ScAddr const a = m_ctx->GenerateNode(ScType::ConstNode);
    EXPECT_TRUE(a.IsValid());
    elements[i] = a;
  }

  // generate template for pending events check
  ScTemplate templ;
  for (auto const & a : elements)
  {
    templ.Quintuple(set1, ScType::VarCommonArc, a >> "_el", ScType::VarPermPosArc, rel);
  }

  std::atomic_uint eventsCount(0);
  std::atomic_uint passedCount(0);

  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstCommonArc>>(
          set1,
          [&passedCount, &eventsCount, &set1, &elements, &rel](
              ScEventAfterGenerateOutgoingArc<ScType::ConstCommonArc> const &)
          {
            std::shared_ptr<ScTemplate> checkTempl(new ScTemplate());
            size_t step = 100;
            size_t testNum = el_num / step - 1;
            for (size_t i = 0; i < testNum; ++i)
            {
              checkTempl->Quintuple(
                  set1, ScType::VarCommonArc, elements[i * step] >> "_el", ScType::VarPermPosArc, rel);
            }

            ScMemoryContext localCtx;
            EXPECT_TRUE(localCtx.IsValid());

            ScTemplateSearchResult res;
            EXPECT_TRUE(localCtx.SearchByTemplate(*checkTempl, res));

            if (res.Size() == 1)
              passedCount.fetch_add(1);

            eventsCount.fetch_add(1);
          });

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(templ, genResult);

  // wait all events
  while (eventsCount.load() < el_num)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  EXPECT_EQ(passedCount, el_num);
}

TEST_F(ScEventTest, BlockEventsAndNotEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  std::atomic_bool isCalled = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          nodeAddr,
          [&isCalled](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            isCalled = true;
          });

  m_ctx->BeginEventsBlocking();

  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr, nodeAddr2);

  m_ctx->EndEventsBlocking();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);
}

TEST_F(ScEventTest, BlockEventsAndEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  std::atomic_bool isCalled = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          nodeAddr,
          [&isCalled](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            isCalled = true;
          });

  m_ctx->BeginEventsBlocking();

  ScAddr nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr, nodeAddr2);

  m_ctx->EndEventsBlocking();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);

  nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(isCalled);
}

TEST_F(ScEventTest, BlockEventsGuardAndNotEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  std::atomic_bool isCalled = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          nodeAddr,
          [&isCalled](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            isCalled = true;
          });

  ScMemoryContextEventsBlockingGuard guard(*m_ctx);

  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_FALSE(isCalled);
}

TEST_F(ScEventTest, BlockEventsGuardAndEmitAfter)
{
  ScAddr const nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);

  std::atomic_bool isCalled = false;
  auto eventSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
          nodeAddr,
          [&isCalled](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const &)
          {
            isCalled = true;
          });
  {
    ScMemoryContextEventsBlockingGuard guard(*m_ctx);

    ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr, nodeAddr2);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(isCalled);
  }

  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr, nodeAddr2);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(isCalled);
}

TEST_F(ScEventTest, TwoSubscriptionsForOneArcErasure)
{
  ScAddr nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  bool isDelayedCalled = false;
  auto delayedSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc>>(
          nodeAddr1,
          [&isDelayedCalled, this](auto const & event)
          {
            isDelayedCalled = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            EXPECT_TRUE(m_ctx->IsElement(event.GetArc()));
            auto const & [sourceAddr, targetAddr] = m_ctx->GetConnectorIncidentElements(event.GetArc());
            EXPECT_TRUE(m_ctx->IsElement(sourceAddr));
            EXPECT_TRUE(m_ctx->IsElement(targetAddr));
            auto const & [source2Addr, target2Addr] = m_ctx->GetConnectorIncidentElements(targetAddr);
            EXPECT_TRUE(m_ctx->IsElement(source2Addr));
            EXPECT_TRUE(m_ctx->IsElement(target2Addr));
            auto const & [source3Addr, target3Addr] = m_ctx->GetConnectorIncidentElements(target2Addr);
            EXPECT_TRUE(m_ctx->IsElement(source3Addr));
            EXPECT_TRUE(m_ctx->IsElement(target3Addr));
            auto const & [source4Addr, target4Addr] = m_ctx->GetConnectorIncidentElements(target3Addr);
            EXPECT_TRUE(m_ctx->IsElement(source4Addr));
            EXPECT_TRUE(m_ctx->IsElement(target4Addr));
            EXPECT_TRUE(m_ctx->GetElementType(target4Addr).IsNode());
          });
  bool isInstantCalled = false;
  auto instantSubscription =
      m_ctx->CreateElementaryEventSubscription<ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc>>(
          nodeAddr1,
          [&isInstantCalled](auto const &)
          {
            isInstantCalled = true;
          });

  ScAddr const nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const nodeAddr3 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const nodeAddr4 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const nodeAddr5 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr const & arcAddr1 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr3, nodeAddr2);
  ScAddr const & arcAddr2 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr4, arcAddr1);
  ScAddr const & arcAddr3 = m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr5, arcAddr2);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, nodeAddr1, arcAddr3);
  m_ctx->EraseElement(nodeAddr2);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_TRUE(isInstantCalled);
  EXPECT_TRUE(isDelayedCalled);
}
