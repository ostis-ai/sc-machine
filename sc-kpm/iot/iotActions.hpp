/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"
#include "wrap/kpm/sc_agent.hpp"

#include <set>

#include "iotActions.generated.hpp"

namespace iot
{
	class PeriodicalTaskManager : public ScObject
	{
		SC_CLASS()
		SC_GENERATED_BODY()

	private:
		struct Task
		{
			uint64_t nextRunTime;
			uint64_t period;
			ScAddr action;

			Task(ScAddr const & inAction, uint64_t const & inPeriod = 0, uint64_t inNextRunTime = 0) 
				: action(inAction)
				, period(inPeriod)
				, nextRunTime(inNextRunTime)
			{
			}
			
			bool operator < (Task const & other) const
			{ 
				return nextRunTime < other.nextRunTime;
			}
		};

	public:
		explicit PeriodicalTaskManager();
		~PeriodicalTaskManager();

		void initialize();
		void shutdown();

		void appendAction(ScAddr const & action, uint32_t period);

		static PeriodicalTaskManager * getInstance();
		
	private:
		bool mIsInitialized;

		ScMemoryContext * mMemoryCtx;

		typedef std::set<PeriodicalTaskManager::Task> tTaskSet;
		tTaskSet mTaskSet;

	public:
		SC_PROPERTY(Keynode, SysIdtf("action_periodical"))
		static ScAddr msActionPeriodical;

	private:
		static PeriodicalTaskManager * msInstance;
	};

	class ANewPeriodicalActionAgent : public ScAgent
	{
		SC_CLASS(Agent, Event(PeriodicalTaskManager::msActionPeriodical, SC_EVENT_ADD_OUTPUT_ARC))
		SC_GENERATED_BODY()

	};
}
