set(SC_SEARCH_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-search SHARED ${SOURCES_CPP} ${SOURCES_C} ${HEADERS})

include_directories(${SC_SEARCH_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-search sc-memory sc-kpm-common)
target_link_libraries(sc-search sc-kpm-common)
