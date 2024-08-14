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

class ScWaitTest : public ScMemoryTest
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

TEST_F(ScWaitTest, Smoke)
{
  EXPECT_TRUE(m_addr.IsValid());
}

TEST_F(ScWaitTest, Valid)
{
  WaitTestData data(m_addr);
  auto eventWaiter = m_ctx->CreateEventWaiter<ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      });
  EXPECT_TRUE(eventWaiter->Wait(
      5000,
      [&data]()
      {
        data.m_isDone = true;
      }));
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, TimeOut)
{
  EXPECT_FALSE(m_ctx->CreateEventWaiter<ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>>(m_addr)->Wait(1000));
}

TEST_F(ScWaitTest, CondValid)
{
  WaitTestData data(m_addr);
  auto waiter = m_ctx->CreateEventWaiterWithCondition<ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const &) -> sc_bool
      {
        return SC_TRUE;
      });

  EXPECT_TRUE(waiter->Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, CondValidFalse)
{
  WaitTestData data(m_addr);

  auto waiter = m_ctx->CreateEventWaiterWithCondition<ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const &) -> sc_bool
      {
        return SC_FALSE;
      });

  sc_bool result = SC_TRUE;
  EXPECT_FALSE(waiter->Wait(
      2000,
      [&result]()
      {
        result = SC_TRUE;
      },
      [&result]()
      {
        result = SC_FALSE;
      }));
  EXPECT_FALSE(result);
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, InvalidWaiters)
{
  ScAddr nodeAddr{23124323};

  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventAddOutputArc<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventAddInputArc<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventAddEdge<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventRemoveOutputArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventRemoveInputArc<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventRemoveEdge<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventRemoveElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaitTest, InvalidWaitersWithCondition)
{
  ScAddr nodeAddr{23124323};

  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventAddOutputArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventAddInputArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventAddEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventRemoveOutputArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventRemoveInputArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventRemoveEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventRemoveElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(
      m_ctx->CreateEventWaiterWithCondition<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaitTest, InvalidEventsFotWaiters)
{
  ScAddr nodeAddr{23124323};
  ScAddr eventClassAddr{23124323};
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaitTest, InvalidEventsFotWaitersWithConditions)
{
  ScAddr nodeAddr{23124323};
  ScAddr eventClassAddr{23124323};
  EXPECT_THROW(m_ctx->CreateEventWaiterWithCondition(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateEventWaiterWithCondition(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}
