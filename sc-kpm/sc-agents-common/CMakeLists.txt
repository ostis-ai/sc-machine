include_directories(${SC_KPM_SRC})

set(SC_AGENTS_COMMON_SRC "${SC_KPM_ROOT}/sc-agents-common")

set(SOURCES
		"sc_agents_common.cpp"
		"common/keynodes/CoreKeynodes.cpp"
		"common/agents/ANumberComparison.cpp"
		"common/utils/IteratorUtils.cpp"
		"common/utils/AgentUtils.cpp"
		"common/utils/CommonUtils.cpp"
		"common/utils/LogicRuleUtils.cpp"
		"inference/keynodes/InferenceKeynodes.cpp"
		"inference/agents/AReverseInference.cpp"
		"inference/agents/ATemplateGeneration.cpp"
		"inference/manager/DirectInferenceManager.cpp"
		)

set(HEADERS
		"sc_agents_common.hpp"
		"common/keynodes/CoreKeynodes.hpp"
		"common/agents/ANumberComparison.hpp"
		"common/utils/IteratorUtils.hpp"
		"common/utils/AgentUtils.hpp"
		"common/utils/CommonUtils.hpp"
		"common/utils/LogicRuleUtils.hpp"
		"inference/keynodes/InferenceKeynodes.hpp"
		"inference/agents/AReverseInference.hpp"
		"inference/agents/ATemplateGeneration.hpp"
		"inference/manager/InferenceManager.hpp"
		"inference/manager/DirectInferenceManager.hpp"
		"inference/model/SolutionTreeNode.hpp"
		)

include_directories(${SC_AGENTS_COMMON_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library(sc-agents-common SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sc-agents-common sc-memory-cpp sc-kpm-common)
add_dependencies(sc-agents-common sc-memory-cpp sc-kpm-common)

sc_codegen_ex(sc-agents-common ${SC_AGENTS_COMMON_SRC} "${SC_AGENTS_COMMON_SRC}/generated")
target_compile_definitions(sc-agents-common PRIVATE "-DSC_AGENTS_COMMON_SELF_BUILD")

