configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/units/test_defines.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/units/test_defines.hpp"
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME sc-builder-tests
    DEPENDS sc-builder-lib sc-memory sc-config-utils
    INCLUDES ${SC_MEMORY_SRC} $
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder-tests)
endif()
