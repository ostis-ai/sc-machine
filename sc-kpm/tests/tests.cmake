make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-agents
    NAME sc-kpm-core-agents-tests
    DEPENDS sc-memory sc-search
    INCLUDES ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-utils
    NAME sc-kpm-agent-common-utils-tests
    DEPENDS sc-memory sc-agents-common sc-kpm-common
    INCLUDES ${SC_MEMORY_SRC} ${SC_KPM_SRC} ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

sc_codegen(sc-kpm-agent-common-utils-tests ${CMAKE_CURRENT_LIST_DIR}/sc-utils)
