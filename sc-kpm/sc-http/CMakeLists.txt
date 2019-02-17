
file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (sc-http SHARED ${SOURCES})
add_dependencies(sc-http sc-memory)
target_link_libraries(sc-http sc-memory)

sc_codegen(sc-http ${CMAKE_CURRENT_LIST_DIR})