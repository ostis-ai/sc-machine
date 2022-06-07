set(SC_JSON_SOCKET_SERVER_SRC "${SC_SERVER_DIR}/sc-json-socket-server")

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

include_directories(${SC_JSON_SOCKET_SERVER_SRC} ${SC_MEMORY_SRC} ${SC_KPM_SRC})

add_library(sc-json-socket-server SHARED ${SOURCES})
target_link_libraries(sc-json-socket-server sc-memory)

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-json-socket-server)
endif ()
