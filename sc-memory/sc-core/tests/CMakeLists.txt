make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units/common
    NAME sc-core-common-tests
    DEPENDS ${GLIB2_LIBRARIES} sc-memory
    INCLUDES ${GLIB2_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core-common-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units/containers
    NAME sc-core-containers-tests
    DEPENDS ${GLIB2_LIBRARIES} sc-memory
    INCLUDES ${GLIB2_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core-containers-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units/fs-storage
    NAME sc-core-fs-storage-tests
    DEPENDS ${GLIB2_LIBRARIES} sc-memory
    INCLUDES ${GLIB2_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core-fs-storage-tests)
endif()

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/test_defines.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/units/fs-storage/test_defines.hpp"
)
