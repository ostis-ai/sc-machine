set(SCP_SRC "${SC_KPM_ROOT}/scp")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

set(CMAKE_C_FLAGS_DEBUG "-DSCP_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-DSCP_DEBUG")
add_definitions(-DSCP_MATH)
add_definitions(-DSCP_STRING)

include_directories(${SCP_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library (scp SHARED ${SOURCES} ${HEADERS})
add_dependencies(scp sc-memory-cpp sc-kpm-common)
target_link_libraries(scp sc-memory-cpp sc-kpm-common)

sc_codegen(scp ${SCP_SRC})
