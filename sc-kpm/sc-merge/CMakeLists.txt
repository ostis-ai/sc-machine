
file(GLOB_RECURSE SOURCES "*.c" "*.h")

add_library (sc-merge SHARED ${SOURCES})

target_include_directories(sc-merge
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(sc-merge sc-core sc-kpm-common)
target_link_libraries(sc-merge sc-kpm-common)
