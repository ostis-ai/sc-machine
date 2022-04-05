set(SC_CORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "*.c" "*.h" "*.cpp" "*.hpp")

add_library (sc-core SHARED ${SOURCES})

target_include_directories(sc-core 
  PRIVATE ${GLIB2_INCLUDE_DIRS} 
  PRIVATE ${RocksDB_INCLUDE_DIR}
  )
target_link_libraries(sc-core ${GLIB2_LIBRARIES} ${RocksDB_LIBRARIES} gcov)
target_compile_definitions(sc-core PRIVATE "-DSC_MEMORY_SELF_BUILD")

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-core)
endif ()

