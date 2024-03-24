make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/common
    NAME sc-memory-common-tests
    DEPENDS sc-memory sc-core
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-common-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/containers
    NAME sc-memory-containers-tests
    DEPENDS sc-memory sc-core
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-containers-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/fs-storage
    NAME sc-fs-storage-tests
    DEPENDS sc-memory sc-core
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-fs-storage-tests)
endif()

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/fs-storage/test_defines.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/fs-storage/test_defines.hpp"
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/agents
    NAME sc-memory-agents-tests
    DEPENDS sc-memory
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-agents-tests)
endif()

sc_codegen(sc-memory-agents-tests ${CMAKE_CURRENT_LIST_DIR}/agents)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/events
    NAME sc-memory-events-tests
    DEPENDS sc-memory
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-events-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/scs
    NAME sc-memory-scs-tests
    DEPENDS sc-memory
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-scs-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/templates
    NAME sc-memory-template-tests
    DEPENDS sc-memory
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-template-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/codegen
    NAME sc-memory-codegen-tests
    DEPENDS sc-memory
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-codegen-tests)
endif()

sc_codegen(sc-memory-codegen-tests ${CMAKE_CURRENT_LIST_DIR}/codegen)