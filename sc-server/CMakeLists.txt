set (SC_SERVER_DIR "${SC_MACHINE_ROOT}/sc-server")

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

add_executable(sc-server ${SOURCES} ${HEADERS})
target_include_directories(sc-server 
  PRIVATE ${SC_MEMORY_SRC} 
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )
target_link_libraries(sc-server sc-memory ${Boost_LIBRARIES})

add_dependencies(sc-server
  sc-kpm
  sc-memory
)

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-server)
endif ()
