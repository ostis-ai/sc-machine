set(SC_SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "*.cpp" "*.c")
file(GLOB_RECURSE HEADERS "*.hpp" "*.h")

add_executable(sc-server ${SOURCES} ${HEADERS})
target_include_directories(sc-server
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}
  PRIVATE ${SC_MEMORY_SRC}
  PRIVATE ${SC_SERVER_DIR}/../sc-config-utils
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )

target_link_libraries(sc-server sc-memory sc-config-utils ${BOOST_LIBS_LIST})

add_dependencies(sc-server
  sc-memory
  sc-config-utils
)

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-server)
endif ()
