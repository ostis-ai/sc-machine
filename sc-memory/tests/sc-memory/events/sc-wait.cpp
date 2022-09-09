#include <gtest/gtest.h>

#include "sc-memory/kpm/sc_agent.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_wait.hpp"

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
  ScMemoryContext ctx(sc_access_lvl_make_min, "EmitEvent");

  ScAddr const node = data.m_addrFrom.IsValid() ? data.m_addrFrom : ctx.CreateNode(ScType::NodeConst);
  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, data.m_addr);

  ASSERT_TRUE(node.IsValid());
  ASSERT_TRUE(edge.IsValid());

  data.m_isDone = edge.IsValid();
}

}

class ScWaitTest : public ScMemoryTest
{
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    ScAgentInit(true);

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

TEST_F(ScWaitTest, smoke)
{
  EXPECT_TRUE(m_addr.IsValid());
}

TEST_F(ScWaitTest, valid)
{
  WaitTestData data(m_addr);
  ScWaitEvent<ScEventAddInputEdge> waiter(*m_ctx, m_addr);
  waiter.SetOnWaitStartDelegate([&data]() {
    EmitEvent(data);
  });

  EXPECT_TRUE(waiter.Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, TimeOut)
{
  EXPECT_FALSE(ScWaitEvent<ScEventAddOutputEdge>(*m_ctx, m_addr).Wait(1000));
}

TEST_F(ScWaitTest, CondValid)
{
  WaitTestData data(m_addr);
  ScWaitCondition<ScEventAddInputEdge> waiter(*m_ctx, m_addr,
      [](ScAddr const &, ScAddr const &, ScAddr const &)
  {
    return true;
  });

  waiter.SetOnWaitStartDelegate([&data]()
  {
    EmitEvent(data);
  });

  EXPECT_TRUE(waiter.Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, CondValidFalse)
{
  WaitTestData data(m_addr);

  ScWaitCondition<ScEventAddInputEdge> waiter(*m_ctx, m_addr,
    [](ScAddr const &, ScAddr const &, ScAddr const &)
  {
    return false;
  });

  waiter.SetOnWaitStartDelegate([&data]() {
    EmitEvent(data);
  });

  EXPECT_FALSE(waiter.Wait(2000));
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, ActionFinishedViaWaitStartDelegate)
{
  WaitTestData data(m_addr, ScAgentAction::GetCommandFinishedAddr());

  ScWaitActionFinished waiter(*m_ctx, m_addr);
  waiter.SetOnWaitStartDelegate([&data]() {
    EmitEvent(data);
  });

  EXPECT_TRUE(waiter.Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, ActionFinished)
{
  WaitTestData data(m_addr, ScAgentAction::GetCommandFinishedAddr());

  ScWaitActionFinished waiter(*m_ctx, m_addr);

  EXPECT_TRUE(waiter.Wait(5000, [&data]() {
        EmitEvent(data);
      }));
  EXPECT_TRUE(data.m_isDone);
}
