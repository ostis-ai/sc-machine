set(SC_UI_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-ui SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_UI_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-ui sc-memory sc-kpm-common)
target_link_libraries(sc-ui sc-kpm-common)
