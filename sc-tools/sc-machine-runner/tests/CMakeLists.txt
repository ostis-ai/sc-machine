make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-machine-tests
    DEPENDS sc-machine-runner
)
target_compile_definitions(sc-machine-tests PRIVATE
    SC_MACHINE_REPO_PATH="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-machine-test-repo"
    SC_MACHINE_EXTENSIONS="${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/kb"
    SC_MACHINE_INI="${CMAKE_CURRENT_SOURCE_DIR}/sc-machine-test.ini"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-machine-tests)
endif()
