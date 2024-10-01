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

  ScAddr const node = data.m_addrFrom.IsValid() ? data.m_addrFrom : ctx.GenerateNode(ScType::ConstNode);
  ScAddr const arcAddr = ctx.GenerateConnector(ScType::ConstPermPosArc, node, data.m_addr);

  ASSERT_TRUE(node.IsValid());
  ASSERT_TRUE(arcAddr.IsValid());

  data.m_isDone = arcAddr.IsValid();
}

}  // namespace

class ScWaiterTest : public ScMemoryTest
{
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_addr = m_ctx->GenerateNode(ScType::ConstNode);
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
  auto eventWaiter = m_ctx->CreateEventWaiter<ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc>>(
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
  EXPECT_FALSE(m_ctx->CreateEventWaiter<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(m_addr)->Wait(1000));
}

TEST_F(ScWaiterTest, EventWaiterWithEventTypeKeynode)
{
  WaitTestData data(m_addr);
  auto eventWaiter = m_ctx->CreateEventWaiter(
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
  auto waiter = m_ctx->CreateConditionWaiter<ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc> const &) -> bool
      {
        return true;
      });

  EXPECT_TRUE(waiter->Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaiterTest, ConditionWaiterNotSuccess)
{
  WaitTestData data(m_addr);

  auto waiter = m_ctx->CreateConditionWaiter<ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc> const &) -> bool
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
  auto waiter = m_ctx->CreateConditionWaiter(
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
      m_ctx->CreateEventWaiter<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventAfterGenerateIncomingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventAfterGenerateEdge<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventBeforeEraseOutgoingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventBeforeEraseIncomingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventBeforeEraseEdge<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventBeforeEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidWaitersWithCondition)
{
  ScAddr nodeAddr;

  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventAfterGenerateOutgoingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventAfterGenerateIncomingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventAfterGenerateEdge<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventBeforeEraseOutgoingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventBeforeEraseIncomingArc<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventBeforeEraseEdge<ScType::MembershipArc>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateConditionWaiter<ScEventBeforeEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventBeforeChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidEventsFotWaiters)
{
  ScAddr nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr eventClassAddr;
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr.Reset();
  eventClassAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  eventClassAddr = ScKeynodes::sc_event_before_change_link_content;
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidEventsFotWaitersWithConditions)
{
  ScAddr nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr eventClassAddr;
  EXPECT_THROW(m_ctx->CreateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr.Reset();
  eventClassAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  eventClassAddr = ScKeynodes::sc_event_before_change_link_content;
  EXPECT_THROW(m_ctx->CreateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->GenerateNode(ScType::ConstNode);
  EXPECT_THROW(m_ctx->CreateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}
