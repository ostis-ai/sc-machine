file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_library (iot SHARED ${SOURCES} ${HEADERS})


include_directories("${SC_KPM_ROOT}/iot" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(iot sc-memory sc-kpm-common)
target_link_libraries(iot sc-kpm-common)

