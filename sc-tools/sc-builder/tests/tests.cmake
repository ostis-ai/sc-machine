configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/units/test_defines.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/units/test_defines.hpp"
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME sc-builder-tests
    DEPENDS sc-builder-lib sc-memory sc-config-utils
    INCLUDES ${SC_MEMORY_SRC} $
)
