file(GLOB SOURCES_C "*.c")
file(GLOB SOURCES_CPP "*.cpp")
file(GLOB HEADERS "*.h")

set(SCP_ROOT ${SC_KPM_ROOT}/scp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin/extensions)

set(CMAKE_C_FLAGS_DEBUG "-DSCP_DEBUG")

add_definitions(-DSCP_MATH)
add_definitions(-DSCP_STRING)

add_library (scp SHARED ${SOURCES_CPP} ${SOURCES_C} ${HEADERS})

include_directories("${SCP_ROOT}/scp_lib" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(scp scp_lib sc-memory)

target_link_libraries(scp scp_lib sc-memory)

add_subdirectory(scp_lib)