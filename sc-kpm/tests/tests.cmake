set(SC_UTILS_TEST_AGENTS_SRC "${CMAKE_CURRENT_LIST_DIR}/test-agents/")
set(SC_UTILS_TEST_AGENTS_SOURCES
    "${SC_UTILS_TEST_AGENTS_SRC}/FinishActionTestAgent.cpp"
    "${SC_UTILS_TEST_AGENTS_SRC}/FinishActionTestAgent.hpp"
)
add_library(sc-utils-test-agents ${SC_UTILS_TEST_AGENTS_SOURCES})
include_directories(
    ${SC_UTILS_TEST_AGENTS_SRC}
    ${SC_MEMORY_SRC}
    ${SC_KPM_SRC}
)
target_link_libraries(sc-utils-test-agents sc-memory sc-agents-common)
sc_codegen_ex(sc-utils-test-agents ${SC_UTILS_TEST_AGENTS_SRC} ${SC_UTILS_TEST_AGENTS_SRC}/generated)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-agents
    NAME sc-kpm-core-agents-tests
    DEPENDS sc-memory sc-search
    INCLUDES ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-utils
    NAME sc-kpm-agent-common-utils-tests
    DEPENDS sc-memory sc-agents-common sc-kpm-common sc-utils-test-agents
    INCLUDES ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

sc_codegen(sc-kpm-agent-common-utils-tests ${CMAKE_CURRENT_LIST_DIR}/sc-utils)
