
file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (sc-python SHARED ${SOURCES})

target_include_directories(sc-python PRIVATE ${SC_MEMORY_SRC})
target_link_libraries(sc-python sc-memory)

add_dependencies(sc-python sc-memory)

sc_codegen(sc-python ${CMAKE_CURRENT_LIST_DIR})

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-python)
endif ()
