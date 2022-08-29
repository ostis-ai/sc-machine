set(SC_CORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "*.c" "*.h" "*.cpp" "*.hpp")

add_library(sc-core SHARED ${SOURCES})

target_include_directories(sc-core PRIVATE ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-core ${GLIB2_LIBRARIES})

if(${SC_FILE_MEMORY} MATCHES "Rocksdb")
    target_include_directories(sc-core PRIVATE ${RocksDB_INCLUDE_DIR})
    target_link_libraries(sc-core ${RocksDB_LIBRARIES})
endif()

if(${SC_COVERAGE})
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	    target_link_libraries(sc-core gcov)
    else()
        message(SEND_ERROR "Currently, code coverage is supported only with GCC as a compiler")
    endif()
endif()

target_compile_definitions(sc-core PRIVATE "-DSC_MEMORY_SELF_BUILD")

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core)
endif()
