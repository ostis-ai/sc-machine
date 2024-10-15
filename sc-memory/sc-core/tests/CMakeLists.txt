FindGLIB()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/common
    NAME sc-core-common-tests
    DEPENDS ${glib_LIBRARIES} sc-memory
    INCLUDES ${glib_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core-common-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/containers
    NAME sc-core-containers-tests
    DEPENDS ${glib_LIBRARIES} sc-memory
    INCLUDES ${glib_INCLUDE_DIRS} ${SC_CORE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core-containers-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units/fs-storage
    NAME sc-core-fs-storage-tests
    DEPENDS ${glib_LIBRARIES} sc-memory
    INCLUDES ${glib_INCLUDE_DIRS} ${SC_CORE_SRC}
)
target_compile_definitions(sc-core-fs-storage-tests PRIVATE
    SC_DEPRECATED_DICTIONARY_FS_MEMORY_PATH="${CMAKE_CURRENT_SOURCE_DIR}/units/fs-storage/deprecated-kb"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core-fs-storage-tests)
endif()
