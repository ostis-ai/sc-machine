#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define sc_agents_common_hpp_17_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "SCAgentsCommonModule::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define sc_agents_common_hpp_17_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "SCAgentsCommonModule::_InitStaticInternal"); \
    bool result = true; \
    return result; \
}


#define sc_agents_common_hpp_17_decl \
public:\
	sc_result InitializeGenerated()\
	{\
		if (!ScKeynodes::Init())\
			return SC_RESULT_ERROR;\
		if (!ScAgentInit(false))\
			return SC_RESULT_ERROR;\
		return InitializeImpl();\
	}\
	sc_result ShutdownGenerated()\
	{\
		return ShutdownImpl();\
	}\
	sc_uint32 GetLoadPriorityGenerated()\
	{\
		return 50;\
	}

#define sc_agents_common_hpp_SCAgentsCommonModule_impl 

#undef ScFileID
#define ScFileID sc_agents_common_hpp

