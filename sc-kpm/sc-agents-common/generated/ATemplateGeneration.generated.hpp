#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define ATemplateGeneration_hpp_21_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "ATemplateGeneration::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define ATemplateGeneration_hpp_21_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "ATemplateGeneration::_InitStaticInternal"); \
    bool result = true; \
    return result; \
}


#define ATemplateGeneration_hpp_21_decl \
private:\
	typedef ScAgent Super;\
protected: \
	ATemplateGeneration(char const * name, sc_uint8 accessLvl) : Super(name, accessLvl) {}\
	virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override; \
private:\
	static std::unique_ptr<ScEvent> ms_event;\
    static std::unique_ptr<ScMemoryContext> ms_context;\
public: \
	static bool handler_emit(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)\
	{\
		ATemplateGeneration Instance("ATemplateGeneration", sc_access_lvl_make_min);\
		return Instance.Run(addr, edgeAddr, otherAddr) == SC_RESULT_OK;\
	}\
	static void RegisterHandler()\
	{\
		SC_ASSERT(!ms_event.get(), ());\
		SC_ASSERT(!ms_context.get(), ());\
		ms_context.reset(new ScMemoryContext(sc_access_lvl_make_min, "handler_ATemplateGeneration"));\
		ms_event.reset(new ScEvent(*ms_context, CoreKeynodes::question_use_logic_rule, ScEvent::Type::AddOutputEdge, &ATemplateGeneration::handler_emit));\
        if (ms_event.get())\
        {\
            SC_LOG_INFO("Register agent ATemplateGeneration");\
        }\
        else\
        {\
            SC_LOG_ERROR("Can't register agent ATemplateGeneration");\
        }\
	}\
	static void UnregisterHandler()\
	{\
		ms_event.reset();\
		ms_context.reset();\
	}

#define ATemplateGeneration_hpp_ATemplateGeneration_impl \
std::unique_ptr<ScEvent> ATemplateGeneration::ms_event;\
std::unique_ptr<ScMemoryContext> ATemplateGeneration::ms_context;

#undef ScFileID
#define ScFileID ATemplateGeneration_hpp

