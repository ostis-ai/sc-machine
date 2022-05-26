set(SC_BASE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "*.h")

add_library (sc-base SHARED ${SOURCES})
set_target_properties(sc-base PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(sc-base
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  PRIVATE ${SC_MEMORY_SRC}
  )
target_link_libraries(sc-base ${GLIB2_LIBRARIES})
if(${SC_COVERAGE})
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	  target_link_libraries(sc-base gcov)
  else()
    message(SEND_ERROR "Currently, code coverage is supported only with GCC as a compiler")
  endif()
endif()

target_compile_definitions(sc-base PRIVATE "-DSC_MEMORY_SELF_BUILD")

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-base)
endif ()
