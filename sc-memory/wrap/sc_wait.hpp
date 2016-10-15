/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"
#include "sc_event.hpp"

extern "C"
{
#include <glib.h>
}

/* Class implements event wait logic.
 * Should be alive, while Memory context is alive.
 */
template <typename EventClassT>
class ScWait
{
	class WaiterImpl
	{
	public:
		WaiterImpl()
			: mIsResolved(false)
		{
			g_mutex_init(&mMutex);
			g_cond_init(&mCond);
		}

		~WaiterImpl()
		{
			g_mutex_clear(&mMutex);
			g_cond_clear(&mCond);
		}
	
		void Resolve()
		{
			g_mutex_lock(&mMutex);
			mIsResolved = true;
			g_cond_signal(&mCond);
			g_mutex_unlock(&mMutex);
		}

		bool Wait(uint64_t timeout_ms)
		{
			gint64 endTime;

			g_mutex_lock(&mMutex);
			endTime = g_get_monotonic_time() + timeout_ms * G_TIME_SPAN_MILLISECOND;
			while (!mIsResolved)
			{
				if (!g_cond_wait_until(&mCond, &mMutex, endTime))
				{
					g_mutex_unlock(&mMutex);
					return false;
				}
			}
			g_mutex_unlock(&mMutex);

			return true;
		}

	private:
		GMutex mMutex;
		GCond mCond;
		bool mIsResolved : 1;
	};

public:
	explicit ScWait(const ScMemoryContext & ctx, const ScAddr & addr)
		: mEvent(ctx, addr, std::bind(&ScWait<EventClassT>::OnEvent, this, std::placeholders::_1, std::placeholders::_2))
	{
	}

	virtual ~ScWait()
	{
	}

	bool Wait(uint64_t timeout_ms = 5000)
	{
		return mWaiterImpl.Wait(timeout_ms);
	}

protected:
	bool OnEvent(const ScAddr & addr, const ScAddr & arg)
	{
		if (OnEventImpl(addr, arg))
		{
			mWaiterImpl.Resolve();
			return true;
		}
		return false;
	}

	virtual bool OnEventImpl(const ScAddr & addr, const ScAddr & arg) { return true; }

private:
	EventClassT mEvent;
	WaiterImpl mWaiterImpl;
};


template<typename EventClassT>
class ScWaitCondition final : public ScWait <EventClassT>
{
public:

	typedef std::function<bool(const ScAddr &, const ScAddr &)> tDelegateCheckFunc;

	explicit ScWaitCondition(const ScMemoryContext & ctx, const ScAddr & addr, tDelegateCheckFunc func)
        : ScWait<EventClassT>(ctx, addr)
		, mCheckFunc(func)
	{
	}

private:
	virtual bool OnEventImpl(const ScAddr & addr, const ScAddr & arg) override
	{
		return mCheckFunc(addr, arg);
	}

private:
	tDelegateCheckFunc mCheckFunc;
};

#define SC_WAIT_CHECK(_func) std::bind(_func, std::placeholders::_1, std::placeholders::_2)
#define SC_WAIT_CHECK_MEMBER(_class, _func) std::bind(_class, _func, std::placeholders::_1, std::placeholders::_2)
