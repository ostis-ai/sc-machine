add_custom_target(sc-builder-tests-kb-generator ALL
    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-builder" -i "${CMAKE_CURRENT_SOURCE_DIR}/repo.path" -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-builder-test-repo" --clear
    DEPENDS sc-builder
)

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-builder-tests
    DEPENDS sc-builder-lib
    INCLUDES ${SC_BUILDER_SRC}
)
add_dependencies(sc-builder-tests sc-builder-tests-kb-generator)
target_compile_definitions(sc-builder-tests PRIVATE
    SC_BUILDER_KB_BIN="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-builder-test-repo"
    SC_BUILDER_REPO_PATH="${CMAKE_CURRENT_SOURCE_DIR}/repo.path"
    SC_BUILDER_KB="${CMAKE_CURRENT_SOURCE_DIR}/kb"
    SC_BUILDER_INI="${CMAKE_CURRENT_SOURCE_DIR}/sc-builder-test.ini"
    SC_BUILDER_TEST_REPOS="${CMAKE_CURRENT_SOURCE_DIR}/repos"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder-tests)
endif()
