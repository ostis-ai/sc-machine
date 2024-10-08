add_custom_target(sc-server-tests-kb-generator ALL
    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-builder" -i "${CMAKE_CURRENT_SOURCE_DIR}/kb" -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-server-test-repo" --clear
    DEPENDS sc-builder
)

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/api
    NAME sc-server-tests
    DEPENDS sc-server-lib
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
add_dependencies(sc-server-tests sc-server-tests-kb-generator)
target_compile_definitions(sc-server-tests PRIVATE
    SC_SERVER_REPO_PATH="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-server-test-repo"
    SC_SERVER_EXTENSIONS="${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/kb"
    SC_SERVER_INI="${CMAKE_CURRENT_SOURCE_DIR}/sc-server-test.ini"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-tests)
endif()

if(${SC_BUILD_BENCH})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/units/performance)
endif()
