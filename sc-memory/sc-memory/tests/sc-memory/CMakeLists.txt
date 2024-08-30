make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/common
    NAME sc-memory-common-tests
    DEPENDS ${glib_LIBRARIES} sc-memory
    INCLUDES ${glib_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-common-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/agents
    NAME sc-memory-agents-tests
    DEPENDS sc-memory
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-agents-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/events
    NAME sc-memory-events-tests
    DEPENDS sc-memory
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-events-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/scs-helper
    NAME sc-memory-scs-tests
    DEPENDS sc-memory
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-scs-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/templates
    NAME sc-memory-template-tests
    DEPENDS sc-memory
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-template-tests)
endif()
