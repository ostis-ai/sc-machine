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
#define COMMAND_AGENT		0
#define QUESTION_AGENT		1


	template<size_t AgentType>
	class Agent
	{
	public:
		explicit Agent(sc::Addr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max)
			: mCmdClassAddr(cmdClassAddr)
			, mMemoryCtx(accessLvl, name)
		{
		}
		
		virtual ~Agent()
		{
		}

		sc_result run(sc::Addr const & startArcAddr);

	protected:
		virtual sc_result runImpl(sc::Addr const & requestAddr, sc::Addr const & resultAddr) = 0;

	protected:
		sc::Addr mCmdClassAddr;
		sc::MemoryContext mMemoryCtx;
	};

#define AGENT_NAME_TYPE(__Name__) __Name__##Type
#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__/*##(__LINE__)##(__FILE__)*/

#define DECLARE_AGENT(__AgentName__, __AgentType__) \
	template<size_t AgentType> \
	class AGENT_NAME_CLASS(__AgentName__) : public iot::Agent<AgentType> \
	{ \
	public: \
	explicit AGENT_NAME_CLASS(__AgentName__)(sc::Addr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_min) \
	: Agent(cmdClassAddr, name, accessLvl) {} \
	private: \
	virtual sc_result runImpl(sc::Addr const & requestAddr, sc::Addr const & resultAddr); \
	}; \
	typedef AGENT_NAME_CLASS(__AgentName__)<__AgentType__> AGENT_NAME_TYPE(__AgentName__);

#define IMPLEMENT_AGENT(__AgentName__, __AgentType__) \
	DECLARE_AGENT(__AgentName__, __AgentType__) \
	template <> sc_result AGENT_NAME_CLASS(__AgentName__)<__AgentType__>::runImpl(sc::Addr const & requestAddr, sc::Addr const & resultAddr)

#define RUN_AGENT(__AgentName__, __CmdClassAddr__, __AccessLvl__, __ArcAddr__) \
	AGENT_NAME_TYPE(__AgentName__) AGENT_NAME_INST(__AgentName__)(__CmdClassAddr__, #__AgentName__, __AccessLvl__); \
	return AGENT_NAME_INST(__AgentName__).run(__ArcAddr__);

}
	
