configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/test_defines.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/units/test_defines.hpp"
)

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-builder-tests
    DEPENDS sc-builder-lib
    INCLUDES ${SC_BUILDER_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder-tests)
endif()
