#include <gtest/gtest.h>

#include "sc-memory/sc_agent.hpp"

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

TEST_F(ScWaitTest, smoke)
{
  EXPECT_TRUE(m_addr.IsValid());
}

TEST_F(ScWaitTest, valid)
{
  WaitTestData data(m_addr);
  ScWaitEvent<ScEventAddInputArc> waiter(*m_ctx, m_addr);
  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });
  EXPECT_TRUE(waiter.Wait(
      5000,
      [&data]()
      {
        data.m_isDone = true;
      }));
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, TimeOut)
{
  EXPECT_FALSE(ScWaitEvent<ScEventAddOutputArc>(*m_ctx, m_addr).Wait(1000));
}

TEST_F(ScWaitTest, CondValid)
{
  WaitTestData data(m_addr);
  ScWaitCondition<ScEventAddInputArc> waiter(
      *m_ctx,
      m_addr,
      [](ScEventAddInputArc const &) -> sc_bool
      {
        return SC_TRUE;
      });

  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });

  EXPECT_TRUE(waiter.Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, CondValidFalse)
{
  WaitTestData data(m_addr);

  ScWaitCondition<ScEventAddInputArc> waiter(
      *m_ctx,
      m_addr,
      [](ScEventAddInputArc const &) -> sc_bool
      {
        return SC_FALSE;
      });

  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });

  sc_bool result = SC_TRUE;
  EXPECT_FALSE(waiter.Wait(
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

TEST_F(ScWaitTest, ActionFinished)
{
  WaitTestData data(m_addr, ScKeynodes::action_finished);

  ScWaitActionFinished waiter(*m_ctx, m_addr);
  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });

  EXPECT_TRUE(waiter.Wait(
      5000,
      [&data]()
      {
        data.m_isDone = true;
      }));
  EXPECT_TRUE(data.m_isDone);
}
