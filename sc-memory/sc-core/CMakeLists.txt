set(SC_CORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "*.c" "*.h" "*.cpp" "*.hpp")

add_library (sc-core SHARED ${SOURCES})

target_include_directories(sc-core 
  PRIVATE ${GLIB2_INCLUDE_DIRS} 
  PRIVATE ${RocksDB_INCLUDE_DIR}
  PRIVATE ${SC_MEMORY_SRC}/sc-base
  )
target_link_libraries(sc-core ${GLIB2_LIBRARIES} ${RocksDB_LIBRARIES} sc-base)
add_dependencies(sc-core sc-base)

if(${SC_COVERAGE})
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	  target_link_libraries(sc-core gcov)
  else()
    message(SEND_ERROR "Currently, code coverage is supported only with GCC as a compiler")
  endif()
endif()

target_compile_definitions(sc-core PRIVATE "-DSC_MEMORY_SELF_BUILD")

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-core)
endif ()

