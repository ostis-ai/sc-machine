set(SCP_SRC "${SC_KPM_ROOT}/scp")

set(SOURCES
        "scp.cpp"
        "scpKeynodes.cpp"
)
set(HEADERS
        "scp.hpp"
        "scpKeynodes.hpp"
)

add_definitions(-DSCP_MATH)
add_definitions(-DSCP_STRING)

include_directories(${SCP_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library (scp SHARED ${SOURCES} ${HEADERS})
add_dependencies(scp sc-memory-cpp sc-kpm-common)
target_link_libraries(scp sc-memory-cpp sc-kpm-common)

sc_codegen(scp ${SCP_SRC})
