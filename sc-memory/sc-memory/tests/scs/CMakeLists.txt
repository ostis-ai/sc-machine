make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME scs-tests
    DEPENDS sc-memory nlohmann_json::nlohmann_json
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(scs-tests)
endif()
