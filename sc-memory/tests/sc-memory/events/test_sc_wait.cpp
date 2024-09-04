#include <gtest/gtest.h>

#include "sc-memory/sc_agent.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_event_wait.hpp"

#include "sc_test.hpp"

namespace
{

// waiters threads
struct WaitTestData
{
  WaitTestData(ScAddr const & addr, ScAddr const & addrFrom = ScAddr())
    : m_addr(addr)
    , m_addrFrom(addrFrom)
    , m_isDone(false)
  {
  }

  ScAddr m_addr;
  ScAddr m_addrFrom;
  bool m_isDone;
};

void EmitEvent(WaitTestData & data)
{
  ScMemoryContext ctx;

  ScAddr const node = data.m_addrFrom.IsValid() ? data.m_addrFrom : ctx.CreateNode(ScType::NodeConst);
  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, data.m_addr);

  ASSERT_TRUE(node.IsValid());
  ASSERT_TRUE(edge.IsValid());

  data.m_isDone = edge.IsValid();
}

}  // namespace

class ScWaiterTest : public ScMemoryTest
{
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_addr = m_ctx->CreateNode(ScType::NodeConst);
    ASSERT_TRUE(m_addr.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_addr;
};

TEST_F(ScWaiterTest, Smoke)
{
  EXPECT_TRUE(m_addr.IsValid());
}

TEST_F(ScWaiterTest, EventWaiter)
{
  WaitTestData data(m_addr);
  auto eventWaiter = m_ctx->GenerateEventWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      });
  bool hasWaited = false;
  EXPECT_TRUE(eventWaiter->Wait(
      5000,
      [&hasWaited]()
      {
        hasWaited = true;
      }));
  EXPECT_TRUE(hasWaited);
}

TEST_F(ScWaiterTest, EventWaiterNotSuccess)
{
  EXPECT_FALSE(
      m_ctx->GenerateEventWaiter<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>(m_addr)->Wait(1000));
}

TEST_F(ScWaiterTest, EventWaiterWithEventTypeKeynode)
{
  WaitTestData data(m_addr);
  auto eventWaiter = m_ctx->GenerateEventWaiter(
      ScKeynodes::sc_event_after_generate_incoming_arc,
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      });
  bool hasWaited = false;
  EXPECT_TRUE(eventWaiter->Wait(
      5000,
      [&hasWaited]()
      {
        hasWaited = true;
      }));
  EXPECT_TRUE(hasWaited);
}

TEST_F(ScWaiterTest, ConditionWaiter)
{
  WaitTestData data(m_addr);
  auto waiter = m_ctx->GenerateConditionWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const &) -> bool
      {
        return true;
      });

  EXPECT_TRUE(waiter->Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaiterTest, ConditionWaiterNotSuccess)
{
  WaitTestData data(m_addr);

  auto waiter = m_ctx->GenerateConditionWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const &) -> bool
      {
        return false;
      });

  bool result = true;
  EXPECT_FALSE(waiter->Wait(
      2000,
      [&result]()
      {
        result = true;
      },
      [&result]()
      {
        result = false;
      }));
  EXPECT_FALSE(result);
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaiterTest, ConditionWaiterWithEventTypeKeynode)
{
  WaitTestData data(m_addr);
  auto waiter = m_ctx->GenerateConditionWaiter(
      ScKeynodes::sc_event_after_generate_incoming_arc,
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScElementaryEvent const &) -> bool
      {
        return true;
      });

  EXPECT_TRUE(waiter->Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaiterTest, InvalidWaiters)
{
  ScAddr nodeAddr;

  EXPECT_THROW(
      m_ctx->GenerateEventWaiter<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateEventWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateEventWaiter<ScEventAfterGenerateEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateEventWaiter<ScEventBeforeEraseOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateEventWaiter<ScEventBeforeEraseIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateEventWaiter<ScEventBeforeEraseEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->GenerateEventWaiter<ScEventBeforeEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->GenerateEventWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->GenerateEventWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidWaitersWithCondition)
{
  ScAddr nodeAddr;

  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventAfterGenerateOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventAfterGenerateEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventBeforeEraseOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventBeforeEraseIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventBeforeEraseEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->GenerateConditionWaiter<ScEventBeforeEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(
      m_ctx->GenerateConditionWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidEventsFotWaiters)
{
  ScAddr nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr eventClassAddr;
  EXPECT_THROW(m_ctx->GenerateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr.Reset();
  eventClassAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->GenerateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  eventClassAddr = ScKeynodes::sc_event_before_change_link_content;
  EXPECT_THROW(m_ctx->GenerateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->GenerateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidEventsFotWaitersWithConditions)
{
  ScAddr nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr eventClassAddr;
  EXPECT_THROW(m_ctx->GenerateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr.Reset();
  eventClassAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->GenerateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  eventClassAddr = ScKeynodes::sc_event_before_change_link_content;
  EXPECT_THROW(m_ctx->GenerateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->GenerateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}
