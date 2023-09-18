set(SC_SEARCH_SRC "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.h" "*.hpp")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${SC_KPM_SRC}
    ${SC_MEMORY_SRC}
    ${GLIB2_INCLUDE_DIRS}
)

find_package(nlohmann_json 3.2.0 REQUIRED)
add_library(sc-search SHARED ${SOURCES})
target_link_libraries(sc-search sc-agents-common sc-kpm-common sc-memory nlohmann_json::nlohmann_json)

sc_codegen_ex(sc-search ${SC_SEARCH_SRC} "${SC_SEARCH_SRC}/generated")
target_compile_definitions(sc-search PRIVATE "-DSC_SEARCH_SELF_BUILD")

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-search)
endif()
