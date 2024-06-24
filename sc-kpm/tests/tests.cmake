set(SC_UTILS_TEST_AGENTS_SRC "${CMAKE_CURRENT_LIST_DIR}/test-agents/")
set(SC_UTILS_TEST_AGENTS_SOURCES
    "${SC_UTILS_TEST_AGENTS_SRC}/FinishActionTestAgent.cpp"
    "${SC_UTILS_TEST_AGENTS_SRC}/FinishActionTestAgent.hpp"
)
add_library(sc-utils-test-agents STATIC ${SC_UTILS_TEST_AGENTS_SOURCES})
target_link_libraries(sc-utils-test-agents LINK_PUBLIC sc-agents-common)
target_include_directories(sc-utils-test-agents PUBLIC ${SC_UTILS_TEST_AGENTS_SRC})

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-agents
    NAME sc-kpm-core-agents-tests
    DEPENDS sc-memory sc-search
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-core-agents-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-utils
    NAME sc-kpm-agent-common-utils-tests
    DEPENDS sc-utils-test-agents sc-kpm-common sc-search sc-utils
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/translators
    NAME sc-kpm-translators-tests
    DEPENDS sc-ui
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

add_definitions(-DSC_KPM_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
