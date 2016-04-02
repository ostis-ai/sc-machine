/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/sc_object.hpp"
#include "wrap/sc_memory.hpp"
#include "wrap/generated/sc_agent.generated.hpp"

#define KPM_COMMAND_AGENT		0
#define KPM_QUESTION_AGENT		1

/// TODO: Implement meta data StaitClass linke in UE4

/* Call this function before agent module usage. 
 * If module initialized, then returns true; otherwise returns false 
 */
_SC_EXTERN bool ScAgentInit();

class ScAgent : public ScObject
{
	friend class ScAgentRunner;

	SC_CLASS()
	SC_GENERATED_BODY()

public:
	_SC_EXTERN explicit ScAgent(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max);
			
	_SC_EXTERN virtual ~ScAgent();
    	
protected:
	_SC_EXTERN sc_result run(ScAddr const & startArcAddr);

	_SC_EXTERN ScAddr getParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, sc_type paramType);

    _SC_EXTERN virtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr) = 0;

public:
	static _SC_EXTERN ScAddr const & GetCommandInitiatedAddr();

protected:
    ScAddr mCmdClassAddr;

    ScMemoryContext mMemoryCtx;

private:
	SC_PROPERTY(Keynode, SysIdtf("command_initiated"))
	static ScAddr msCommandInitiatedAddr;

	SC_PROPERTY(Keynode, SysIdtf("command_in_progress"))
	static ScAddr msCommandProgressdAddr;

	SC_PROPERTY(Keynode, SysIdtf("command_finished"))
	static ScAddr msCommandFinishedAddr;

	SC_PROPERTY(Keynode, SysIdtf("nrel_result"))
	static ScAddr msNrelResult;

private:
    ScAddr mCmdAddr;
};

#define AGENT_NAME_TYPE(__Name__) __Name__##Type
#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__/*##(__LINE__)##(__FILE__)*/


/// TODO: Deprecated
// Old style compatibility
class ScAgentRunner
{
public:
	sc_result operator() (ScAgent & agent, ScAddr & arcAddr)
	{
		return agent.run(arcAddr);
	}
};

#define DECLARE_AGENT(__AgentName__, __AgentType__) \
	class AGENT_NAME_CLASS(__AgentName__) : public ScAgent \
	{ \
	public: \
    explicit AGENT_NAME_CLASS(__AgentName__)(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_min) \
    : ScAgent(cmdClassAddr, name, accessLvl) {} \
	private: \
    virtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr); \
	}; \
	typedef AGENT_NAME_CLASS(__AgentName__) AGENT_NAME_TYPE(__AgentName__);

#define IMPLEMENT_AGENT(__AgentName__, __AgentType__) \
	DECLARE_AGENT(__AgentName__, __AgentType__) \
    sc_result AGENT_NAME_CLASS(__AgentName__)::runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr)

#define RUN_AGENT(__AgentName__, __CmdClassAddr__, __AccessLvl__, __ArcAddr__) \
	AGENT_NAME_TYPE(__AgentName__) AGENT_NAME_INST(__AgentName__)(__CmdClassAddr__, #__AgentName__, __AccessLvl__); \
	return ScAgentRunner()(AGENT_NAME_INST(__AgentName__), __ArcAddr__);
/// end Deprecated	




#define SC_AGENT_IMPLEMENTATION(__AgentName__) \
	SC_COMBINE(ScFileID, _, __AgentName__, _impl) \
	sc_result __AgentName__##::runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr)

#define SC_AGENT_REGISTER(__AgentName__) \
	__AgentName__##::initGlobal(); \
	__AgentName__##::registerHandler();

#define SC_AGENT_UNREGISTER(__AgentName__)	__AgentName__##::unregisterHandler();