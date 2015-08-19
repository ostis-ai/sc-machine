file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library (search SHARED ${SOURCES_CPP} ${SOURCES_C} ${HEADERS})

include_directories("${SC_KPM_ROOT}/search" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(search sc-memory sc-kpm-common)
target_link_libraries(search sc-kpm-common)


install_targets("/lib/sc-memory/extensions" search)


