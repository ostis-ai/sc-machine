/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_wait.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"

namespace
{
// waiters threads
struct WaitTestData
{
  WaitTestData(ScMemoryContext & ctx, ScAddr const & addr, ScAddr const & addrFrom = ScAddr())
    : m_context(ctx)
    , m_addr(addr)
    , m_addrFrom(addrFrom)
    , m_isDone(false)
  {
  }

  ScMemoryContext & m_context;
  ScAddr m_addr;
  ScAddr m_addrFrom;
  bool m_isDone;
};
gpointer emit_event_thread(gpointer data)
{
  WaitTestData * d = (WaitTestData*)data;
  g_usleep(500000);	// sleep to run some later

  ScAddr const node = d->m_addrFrom.IsValid() ? d->m_addrFrom : d->m_context.CreateNode(*ScType::NodeConst);
  ScAddr const edge = d->m_context.CreateEdge(*ScType::EdgeAccessConstPosPerm, node, d->m_addr);

  d->m_isDone = edge.IsValid();

  return nullptr;
}
}

UNIT_TEST(waiter)
{
  ScAgentInit(true);
  ScMemoryContext ctx(sc_access_lvl_make_min, "waiter");

  const ScAddr addr = ctx.CreateNode(ScType::NodeConst);
  SC_CHECK(addr.IsValid(), ());

  SUBTEST_START(WaitValid)
  {
    WaitTestData data(ctx, addr);
    GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
    SC_CHECK_NOT_EQUAL(thread, nullptr, ());

    SC_CHECK(ScWait<ScEventAddInputEdge>(ctx, addr).Wait(), ("Waiter timeout"));
    SC_CHECK(data.m_isDone, ("Waiter finished, but flag is false"));
  }
  SUBTEST_END()

  SUBTEST_START(WaitTimeOut)
  {
    SC_CHECK(!ScWait<ScEventAddOutputEdge>(ctx, addr).Wait(1000), ());
  }
  SUBTEST_END()

  SUBTEST_START(WaitCondValid)
  {
    WaitTestData data(ctx, addr);
    GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
    SC_CHECK_NOT_EQUAL(thread, nullptr, ());

    ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, [](ScAddr const & listenAddr,
                                                ScAddr const & edgeAddr,
                                                ScAddr const & otherAddr)
    {
      return true;
    });

    SC_CHECK(waiter.Wait(), ("Waiter timeout"));
    SC_CHECK(data.m_isDone, ("Waiter finished, but failed"));
  }
  SUBTEST_END()

  SUBTEST_START(WaitCondValidFalse)
  {
    WaitTestData data(ctx, addr);
    GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
    SC_CHECK_NOT_EQUAL(thread, nullptr, ());

    ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, [](ScAddr const & listenAddr,
                                                ScAddr const & edgeAddr,
                                                ScAddr const & otherAddr)
    {
      return false;
    });

    SC_CHECK(!waiter.Wait(), ());
    SC_CHECK(data.m_isDone, ());
  }
  SUBTEST_END()

  SUBTEST_START(WaitActionFinished)
  {
    WaitTestData data(ctx, addr, ScAgentAction::GetCommandFinishedAddr());
    GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
    SC_CHECK_NOT_EQUAL(thread, nullptr, ());

    ScWaitActionFinished waiter(ctx, addr);

    SC_CHECK(waiter.Wait(), ("Waiter timeout"));
    SC_CHECK(data.m_isDone, ("Waiter finished"));
  }
  SUBTEST_END()
}
