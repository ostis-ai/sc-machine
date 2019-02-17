set(SC_CORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "*.c" ".h")

add_library (sc-core SHARED ${SOURCES})

include_directories(${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-core ${GLIB2_LIBRARIES})
target_compile_definitions(sc-core PRIVATE "-DSC_MEMORY_SELF_BUILD")
