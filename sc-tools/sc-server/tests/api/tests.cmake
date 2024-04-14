configure_file(
    "${SC_SERVER_ROOT}/tests/test_defines.hpp.in"
    "${SC_SERVER_ROOT}/tests/test_defines.hpp"
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME sc-server-tests
    DEPENDS sc-server-lib
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-tests)
endif()
