/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-memory/sc_wait.hpp"
#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-test-framework/sc_test_unit.hpp"

namespace
{

// waiters threads
struct WaitTestData
{
  WaitTestData(ScAddr const & addr, ScAddr const & addrFrom = ScAddr())
        : m_addr(addr), m_addrFrom(addrFrom), m_isDone(false)
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

TEST_CASE("waiter", "[test wait]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScAgentInit(true);
  ScMemoryContext ctx(sc_access_lvl_make_min, "waiter");

  try
  {
    const ScAddr addr = ctx.CreateNode(ScType::NodeConst);
    REQUIRE(addr.IsValid());

    SECTION("WaitValid")
    {
      SUBTEST_START("WaitValid")
      {
        try
        {
          WaitTestData data(addr);
          ScWaitEvent<ScEventAddInputEdge> waiter(ctx, addr);
          waiter.SetOnWaitStartDelegate(
                [&data]()
                {
                  EmitEvent(data);
                });

          REQUIRE(waiter.Wait());
          REQUIRE(data.m_isDone);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"WaitValid\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("WaitTimeOut")
    {
      SUBTEST_START("WaitTimeOut")
      {
        try
        {
          REQUIRE_FALSE(ScWaitEvent<ScEventAddOutputEdge>(ctx, addr).Wait(1000));
        } catch (...)
        {
          SC_LOG_ERROR("Test \"WaitTimeOut\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("WaitCondValid")
    {
      SUBTEST_START("WaitCondValid")
      {
        try
        {
          WaitTestData data(addr);
          ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, [](
                ScAddr const & listenAddr,
                ScAddr const & edgeAddr,
                ScAddr const & otherAddr)
          {
            return true;
          });

          waiter.SetOnWaitStartDelegate(
                [&data]()
                {
                  EmitEvent(data);
                });

          REQUIRE(waiter.Wait());
          REQUIRE(data.m_isDone);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"WaitCondValid\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("WaitCondValidFalse")
    {
      SUBTEST_START("WaitCondValidFalse")
      {
        try
        {
          WaitTestData data(addr);

          ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, [](
                ScAddr const & listenAddr,
                ScAddr const & edgeAddr,
                ScAddr const & otherAddr)
          {
            return false;
          });

          waiter.SetOnWaitStartDelegate(
                [&data]()
                {
                  EmitEvent(data);
                });

          REQUIRE_FALSE(waiter.Wait(2000));
          REQUIRE(data.m_isDone);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"WaitCondValidFalse\" failed")
        }
      }
      SUBTEST_END()
    }

    SECTION("WaitActionFinished")
    {
      SUBTEST_START("WaitActionFinished")
      {
        try
        {
          WaitTestData data(addr, ScAgentAction::GetCommandFinishedAddr());

          ScWaitActionFinished waiter(ctx, addr);
          waiter.SetOnWaitStartDelegate(
                [&data]()
                {
                  EmitEvent(data);
                });

          REQUIRE(waiter.Wait());
          REQUIRE(data.m_isDone);
        } catch (...)
        {
          SC_LOG_ERROR("Test \"WaitActionFinished\" failed")
        }
      }
      SUBTEST_END()
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"waiter\" failed")
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
