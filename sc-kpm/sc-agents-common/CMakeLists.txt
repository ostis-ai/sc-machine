include_directories(${SC_KPM_SRC})

set(SC_AGENTS_COMMON_SRC "${SC_KPM_ROOT}/sc-agents-common")

set(SOURCES
		"sc_agents_common.cpp"
		"keynodes/coreKeynodes.cpp"
		"utils/GenerationUtils.cpp"
		"utils/IteratorUtils.cpp"
		"utils/AgentUtils.cpp"
		"utils/CommonUtils.cpp"
		"utils/LogicRuleUtils.cpp"
		)

set(HEADERS
		"sc_agents_common.hpp"
		"keynodes/coreKeynodes.hpp"
		"utils/GenerationUtils.hpp"
		"utils/IteratorUtils.hpp"
		"utils/AgentUtils.hpp"
		"utils/CommonUtils.hpp"
		"utils/LogicRuleUtils.hpp"
		)

include_directories(${SC_AGENTS_COMMON_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library(sc-agents-common SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sc-agents-common sc-memory-cpp sc-kpm-common)
add_dependencies(sc-agents-common sc-memory-cpp sc-kpm-common)

sc_codegen_ex(sc-agents-common ${SC_AGENTS_COMMON_SRC} "${SC_AGENTS_COMMON_SRC}/generated")
target_compile_definitions(sc-agents-common PRIVATE "-DSC_AGENTS_COMMON_SELF_BUILD")

