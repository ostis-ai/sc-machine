/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_wait.hpp"

namespace
{
	// waiters threads
	struct WaitTestData
	{
		WaitTestData(ScMemoryContext & ctx, const ScAddr & addr)
			: mContext(ctx)
			, mAddr(addr)
			, mIsDone(false)
		{
		}

		ScMemoryContext & mContext;
		ScAddr mAddr;
		bool mIsDone;
	};
	gpointer emit_event_thread(gpointer data)
	{
		WaitTestData * d = (WaitTestData*)data;
		g_usleep(2000000);	// sleep for a second

		const ScAddr node = d->mContext.createNode(*ScType::NODE_CONST);
		const ScAddr arc = d->mContext.createEdge(*ScType::EDGE_ACCESS_CONST_POS_PERM, node, d->mAddr);

		d->mIsDone = arc.isValid();

		return nullptr;
	}
}

UNIT_TEST(waiter)
{
	ScMemoryContext ctx(sc_access_lvl_make_min);

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

		auto checkTrue = [](const ScAddr & addr, const ScAddr & arg) {
			return true;
		};

		ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, SC_WAIT_CHECK(checkTrue));

		SC_CHECK(waiter.Wait(), ("Waiter timeout"));
		SC_CHECK(data.mIsDone, ("Waiter finished, but failed"));
	}
	SUBTEST_END

	SUBTEST_START(WaitCondValidFalse)
	{
		WaitTestData data(ctx, addr);
		GThread * thread = g_thread_try_new(0, emit_event_thread, (gpointer)&data, 0);
		SC_CHECK_NOT_EQUAL(thread, nullptr, ());

		auto checkFalse = [](const ScAddr & addr, const ScAddr & arg) {
			return false;
		};

		ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, SC_WAIT_CHECK(checkFalse));

		SC_CHECK(!waiter.Wait(), ());
		SC_CHECK(data.mIsDone, ());
	}
	SUBTEST_END
}
