/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/utils/sc_log.hpp"
#include "../generated/sc_agent.generated.hpp"

#define KPM_COMMAND_AGENT		0
#define KPM_QUESTION_AGENT		1

/* Call this function before agent module usage. 
 * If module initialized, then returns true; otherwise returns false 
 */
_SC_EXTERN bool ScAgentInit();

class ScAgent : public ScObject
{
	SC_CLASS()
	SC_GENERATED_BODY()

public:
	_SC_EXTERN explicit ScAgent(char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max);
	_SC_EXTERN virtual ~ScAgent();

protected:
	_SC_EXTERN sc_result runNativeEvent(ScAddr const & addr, ScAddr const & );

	_SC_EXTERN virtual sc_result run(ScAddr const & listedAddr, ScAddr const & argAddr);

protected:
	ScMemoryContext mMemoryCtx;
};

class ScAgentAction : public ScAgent
{
	friend class ScAgentRunner;

	SC_CLASS()
	SC_GENERATED_BODY()

public:
	_SC_EXTERN explicit ScAgentAction(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max);
			
	_SC_EXTERN virtual ~ScAgentAction();
    	
protected:
	_SC_EXTERN virtual sc_result run(ScAddr const & listedAddr, ScAddr const & startArcAddr) override;

	_SC_EXTERN ScAddr getParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, sc_type paramType);

    _SC_EXTERN virtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr) = 0;

public:
	static _SC_EXTERN ScAddr const & GetCommandInitiatedAddr();

protected:
    ScAddr mCmdClassAddr;

private:
	SC_PROPERTY(Keynode("command_initiated"), ForceCreate)
	static ScAddr msCommandInitiatedAddr;

	SC_PROPERTY(Keynode("command_in_progress"), ForceCreate)
	static ScAddr msCommandProgressdAddr;

	SC_PROPERTY(Keynode("command_finished"), ForceCreate)
	static ScAddr msCommandFinishedAddr;

	SC_PROPERTY(Keynode("nrel_result"), ForceCreate)
	static ScAddr msNrelResult;

private:
    ScAddr mCmdAddr;
};

#define AGENT_NAME_TYPE(__Name__) __Name__##Type
#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__/*##(__LINE__)##(__FILE__)*/


#define SC_AGENT_IMPLEMENTATION(__AgentName__) \
	SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
    sc_result __AgentName__::run(ScAddr const & listenAddr, ScAddr const & argAddr)

#define SC_AGENT_ACTION_IMPLEMENTATION(__AgentName__) \
	SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
    sc_result __AgentName__::runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr)



#define SC_AGENT_REGISTER(__AgentName__) \
	SC_OBJECT_INIT_GLOBAL_CALL(__AgentName__) \
    __AgentName__::registerHandler();

#define SC_AGENT_UNREGISTER(__AgentName__)	__AgentName__::unregisterHandler();
