file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (sc-task SHARED ${SOURCES})

target_include_directories(sc-task PRIVATE ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-task sc-memory sc-kpm-common)
target_link_libraries(sc-task sc-memory sc-kpm-common)
