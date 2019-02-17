file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.h" "*.hpp")

add_library (sc-search SHARED ${SOURCES})

target_include_directories(sc-search
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(sc-search sc-memory sc-kpm-common)
target_link_libraries(sc-search sc-kpm-common)
