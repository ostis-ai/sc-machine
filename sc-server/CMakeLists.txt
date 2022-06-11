set (SC_SERVER_DIR "${SC_MACHINE_ROOT}/sc-server")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_subdirectory(src/sc-server-impl)

add_executable(sc-server ${SOURCES} ${HEADERS})
target_include_directories(sc-server
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/sc-json-socker-server
  PRIVATE ${SC_MEMORY_SRC} 
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )

target_link_libraries(sc-server sc-memory sc-json-socket-server ${BOOST_LIBS_LIST})

add_dependencies(sc-server
  sc-json-socket-server
  sc-kpm
  sc-memory
)

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-server)
endif ()
