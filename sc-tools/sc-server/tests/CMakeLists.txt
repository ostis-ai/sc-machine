configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/test_defines.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/units/api/test_defines.hpp"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/test_defines.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/units/performance/units/test_defines.hpp"
)

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/api
    NAME sc-server-tests
    DEPENDS sc-server-lib
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-tests)
endif()

if(${SC_BUILD_BENCH})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/units/performance)
endif()
