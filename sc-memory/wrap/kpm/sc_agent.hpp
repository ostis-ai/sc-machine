/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/sc_memory.hpp"

#define KPM_COMMAND_AGENT		0
#define KPM_QUESTION_AGENT		1


/* Call this function before agent module usage. 
 * If module initialized, then returns true; otherwise returns false 
 */
_SC_EXTERN bool ScAgentInit();

template <size_t AgentType>
class ScAgent
{
public:
    _SC_EXTERN explicit ScAgent(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_max)
		: mCmdClassAddr(cmdClassAddr)
		, mMemoryCtx(accessLvl, name)
	{
	}
		
    _SC_EXTERN virtual ~ScAgent()
	{
	}

    _SC_EXTERN sc_result run(ScAddr const & startArcAddr);

    _SC_EXTERN ScAddr getParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, sc_type paramType)
	{
		ScIterator5Ptr iter = mMemoryCtx.iterator5(cmdAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(paramType),
			SC_TYPE(sc_type_arc_pos_const_perm),
			relationAddr);

		if (!iter->next())
            return ScAddr();

		return iter->value(2);
	}

protected:
    _SC_EXTERN virtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr) = 0;

protected:
    ScAddr mCmdClassAddr;
    ScMemoryContext mMemoryCtx;

private:
    ScAddr mCmdAddr;
};

#define AGENT_NAME_TYPE(__Name__) __Name__##Type
#define AGENT_NAME_CLASS(__Name__) __Name__##_Agent
#define AGENT_NAME_INST(__Name__) __Name__/*##(__LINE__)##(__FILE__)*/

#define DECLARE_AGENT(__AgentName__, __AgentType__) \
	template<size_t AgentType> \
	class AGENT_NAME_CLASS(__AgentName__) : public ScAgent<AgentType> \
	{ \
	public: \
    explicit AGENT_NAME_CLASS(__AgentName__)(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl = sc_access_lvl_make_min) \
    : ScAgent(cmdClassAddr, name, accessLvl) {} \
	private: \
    virtual sc_result runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr); \
	}; \
	typedef AGENT_NAME_CLASS(__AgentName__)<__AgentType__> AGENT_NAME_TYPE(__AgentName__);

#define IMPLEMENT_AGENT(__AgentName__, __AgentType__) \
	DECLARE_AGENT(__AgentName__, __AgentType__) \
    template <> sc_result AGENT_NAME_CLASS(__AgentName__)<__AgentType__>::runImpl(ScAddr const & requestAddr, ScAddr const & resultAddr)

#define RUN_AGENT(__AgentName__, __CmdClassAddr__, __AccessLvl__, __ArcAddr__) \
	AGENT_NAME_TYPE(__AgentName__) AGENT_NAME_INST(__AgentName__)(__CmdClassAddr__, #__AgentName__, __AccessLvl__); \
	return AGENT_NAME_INST(__AgentName__).run(__ArcAddr__);

	
