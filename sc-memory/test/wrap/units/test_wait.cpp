/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_wait.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

namespace
{
	// waiters threads
	struct WaitTestData
	{
		WaitTestData(ScMemoryContext & ctx, ScAddr const & addr, ScAddr const & addrFrom = ScAddr())
			: mContext(ctx)
			, mAddr(addr)
            , mAddrFrom(addrFrom)
			, mIsDone(false)
		{
		}

		ScMemoryContext & mContext;
		ScAddr mAddr;
        ScAddr mAddrFrom;
		bool mIsDone;
	};
	gpointer emit_event_thread(gpointer data)
	{
		WaitTestData * d = (WaitTestData*)data;
		g_usleep(500000);	// sleep to run some later

		ScAddr const node = d->mAddrFrom.isValid() ? d->mAddrFrom : d->mContext.createNode(*ScType::NODE_CONST);
		ScAddr const edge = d->mContext.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, node, d->mAddr);

		d->mIsDone = edge.isValid();

		return nullptr;
	}
}

UNIT_TEST(waiter)
{
    ScAgentInit(true);
	ScMemoryContext ctx(sc_access_lvl_make_min, "waiter");

	const ScAddr addr = ctx.createNode(ScType::NODE_CONST);
	SC_CHECK(addr.isValid(), ());

	SUBTEST_START(WaitValid)
	{
		WaitTestData data(ctx, addr);
		GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
		SC_CHECK_NOT_EQUAL(thread, nullptr, ());

		SC_CHECK(ScWait<ScEventAddInputEdge>(ctx, addr).Wait(), ("Waiter timeout"));
		SC_CHECK(data.mIsDone, ("Waiter finished, but flag is false"));
	}
	SUBTEST_END

	SUBTEST_START(WaitTimeOut)
	{
		SC_CHECK(!ScWait<ScEventAddOutputEdge>(ctx, addr).Wait(1000), ());
	}
	SUBTEST_END

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
		SC_CHECK(data.mIsDone, ("Waiter finished, but failed"));
	}
	SUBTEST_END

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
		SC_CHECK(data.mIsDone, ());
	}
	SUBTEST_END

    SUBTEST_START(WaitActionFinished)
    {
        WaitTestData data(ctx, addr, ScAgentAction::GetCommandFinishedAddr());
        GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
        SC_CHECK_NOT_EQUAL(thread, nullptr, ());

        ScWaitActionFinished waiter(ctx, addr);

        SC_CHECK(waiter.Wait(), ("Waiter timeout"));
        SC_CHECK(data.mIsDone, ("Waiter finished"));
    }
    SUBTEST_END
}
