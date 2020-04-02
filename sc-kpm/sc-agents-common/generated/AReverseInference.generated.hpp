#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define AReverseInference_hpp_22_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "AReverseInference::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define AReverseInference_hpp_22_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "AReverseInference::_InitStaticInternal"); \
    bool result = true; \
    return result; \
}


#define AReverseInference_hpp_22_decl \
private:\
	typedef ScAgent Super;\
protected: \
	AReverseInference(char const * name, sc_uint8 accessLvl) : Super(name, accessLvl) {}\
	virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override; \
private:\
	static std::unique_ptr<ScEvent> ms_event;\
    static std::unique_ptr<ScMemoryContext> ms_context;\
public: \
	static bool handler_emit(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)\
	{\
		AReverseInference Instance("AReverseInference", sc_access_lvl_make_min);\
		return Instance.Run(addr, edgeAddr, otherAddr) == SC_RESULT_OK;\
	}\
	static void RegisterHandler()\
	{\
		SC_ASSERT(!ms_event.get(), ());\
		SC_ASSERT(!ms_context.get(), ());\
		ms_context.reset(new ScMemoryContext(sc_access_lvl_make_min, "handler_AReverseInference"));\
		ms_event.reset(new ScEvent(*ms_context, CoreKeynodes::question_reverse_inference, ScEvent::Type::AddOutputEdge, &AReverseInference::handler_emit));\
        if (ms_event.get())\
        {\
            SC_LOG_INFO("Register agent AReverseInference");\
        }\
        else\
        {\
            SC_LOG_ERROR("Can't register agent AReverseInference");\
        }\
	}\
	static void UnregisterHandler()\
	{\
		ms_event.reset();\
		ms_context.reset();\
	}

#define AReverseInference_hpp_AReverseInference_impl \
std::unique_ptr<ScEvent> AReverseInference::ms_event;\
std::unique_ptr<ScMemoryContext> AReverseInference::ms_context;

#undef ScFileID
#define ScFileID AReverseInference_hpp

