
set(SCP_ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin/extensions)

set(CMAKE_C_FLAGS_DEBUG "-DSCP_DEBUG")

add_definitions(-DSCP_MATH)
add_definitions(-DSCP_STRING)

add_library (scp SHARED ${SOURCES_CPP} ${SOURCES_C} ${HEADERS})

include_directories("${SCP_ROOT}/scp_lib" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(scp sc-core)

target_link_libraries(scp sc-core)
