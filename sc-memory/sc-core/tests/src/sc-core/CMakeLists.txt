make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units/containers
    NAME sc-memory-containers-tests
    DEPENDS ${glib_LIBRARIES} sc-memory
    INCLUDES ${glib_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory-containers-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units/fs-storage
    NAME sc-fs-storage-tests
    DEPENDS ${glib_LIBRARIES} sc-memory
    INCLUDES ${glib_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-fs-storage-tests)
endif()

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/units/fs-storage/test_defines.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/units/fs-storage/test_defines.hpp"
)

