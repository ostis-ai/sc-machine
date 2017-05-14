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

  data.m_isDone = edge.IsValid();
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
    WaitTestData data(addr);
    ScWaitEvent<ScEventAddInputEdge> waiter(ctx, addr);
    waiter.SetOnWaitStartDelegate([&data]() {
      EmitEvent(data);
    });
    
    SC_CHECK(waiter.Wait(), ("Waiter timeout"));
    SC_CHECK(data.m_isDone, ("Waiter finished, but flag is false"));
  }
  SUBTEST_END()

  SUBTEST_START(WaitTimeOut)
  {
    SC_CHECK(!ScWaitEvent<ScEventAddOutputEdge>(ctx, addr).Wait(1000), ());
  }
  SUBTEST_END()

  SUBTEST_START(WaitCondValid)
  {
    WaitTestData data(addr);
    ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, [](ScAddr const & listenAddr,
                                                ScAddr const & edgeAddr,
                                                ScAddr const & otherAddr)
    {
      return true;
    });

    waiter.SetOnWaitStartDelegate([&data]() {
      EmitEvent(data);
    });

    SC_CHECK(waiter.Wait(), ("Waiter timeout"));
    SC_CHECK(data.m_isDone, ("Waiter finished, but failed"));
  }
  SUBTEST_END()

  SUBTEST_START(WaitCondValidFalse)
  {
    WaitTestData data(addr);

    ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, [](ScAddr const & listenAddr,
                                                ScAddr const & edgeAddr,
                                                ScAddr const & otherAddr)
    {
      return false;
    });

    waiter.SetOnWaitStartDelegate([&data]() {
      EmitEvent(data);
    });

    SC_CHECK(!waiter.Wait(2000), ());
    SC_CHECK(data.m_isDone, ());
  }
  SUBTEST_END()

  SUBTEST_START(WaitActionFinished)
  {
    WaitTestData data(addr, ScAgentAction::GetCommandFinishedAddr());
    
    ScWaitActionFinished waiter(ctx, addr);
    waiter.SetOnWaitStartDelegate([&data]() {
      EmitEvent(data);
    });

    SC_CHECK(waiter.Wait(), ("Waiter timeout"));
    SC_CHECK(data.m_isDone, ("Waiter finished"));
  }
  SUBTEST_END()
}
