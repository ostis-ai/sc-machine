/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"

#include "wrap/sc_memory.hpp"

namespace iot
{
	class Agent
	{
	public:
		explicit Agent(sc::Addr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max);
		virtual ~Agent();

		sc_result run(sc::Addr const & startArcAddr);

	protected:
		virtual void runImpl(sc::Addr const & cmdAddr) = 0;

	protected:
		sc::Addr mCmdClassAddr;
		sc::MemoryContext mMemoryCtx;
	};

#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__##__LINE__##__FILE__

#define DECLARE_AGENT(__AgentName__) \
	class AGENT_NAME_CLASS(__AgentName__) : public iot::Agent \
	{ \
	public: \
	explicit AGENT_NAME_CLASS(__AgentName__)(sc::Addr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max) \
	: Agent(cmdClassAddr, name, accessLvl) {} \
	private: \
	virtual void runImpl(sc::Addr const & cmdAddr); \
	};

#define IMPLEMENT_AGENT(__AgentName__) \
	DECLARE_AGENT(__AgentName__) \
	void AGENT_NAME_CLASS(__AgentName__)::runImpl(sc::Addr const & cmdAddr)


#define RUN_AGENT(__AgentName__, __CmdClassAddr__, __AccessLvl__, __ArcAddr__) \
	AGENT_NAME_CLASS(__AgentName__) AGENT_NAME_INST(__AgentName__)(__CmdClassAddr__, #__AgentName__, __AccessLvl__); \
	AGENT_NAME_INST(__AgentName__).run(__ArcAddr__);


}
	
