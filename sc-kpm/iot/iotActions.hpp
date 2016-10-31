/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include <set>

#include "iotActions.generated.hpp"

namespace iot
{
	class ActionManager : public ScObject
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
                : nextRunTime(inNextRunTime)
				, period(inPeriod)
                , action(inAction)

			{
			}
			
			bool operator < (Task const & other) const
			{ 
				return nextRunTime < other.nextRunTime;
			}
		};

	public:
		explicit ActionManager();
		~ActionManager();

		void initialize();
		void shutdown();

		void appendActionPeriodical(ScAddr const & action, uint32_t period);
		void appendAction(ScAddr const & action, uint64_t runTime);
		void tick();

		bool isRunning() const;

		static ActionManager * getInstance();
		
	protected:
		void addPeriodicalAction(ScAddr const & actionAddr);
		void addTimeSpecifiedAction(ScAddr const & actionAddr);

	private:
		bool mIsInitialized;
		bool mIsRunning;

		ScMemoryContext * mMemoryCtx;

		typedef std::set<ActionManager::Task> tTaskSet;
		tTaskSet mTaskSet;

	public:
		SC_PROPERTY(Keynode("action_periodical"), ForceCreate)
		static ScAddr msActionPeriodical;

		SC_PROPERTY(Keynode("action_time_specified"), ForceCreate)
		static ScAddr msActionTimeSpecified;

	private:
		static ActionManager * msInstance;
	};

	class ANewPeriodicalActionAgent : public ScAgent
	{
		SC_CLASS(Agent, Event(ActionManager::msActionPeriodical, SC_EVENT_ADD_OUTPUT_ARC))
		SC_GENERATED_BODY()

	};
}
