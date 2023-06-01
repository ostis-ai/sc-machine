file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(sc-ui SHARED ${SOURCES})

target_include_directories(sc-ui 
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(nlohmann_json)
add_dependencies(sc-ui sc-memory sc-core sc-kpm-common)
target_link_libraries(sc-ui PRIVATE sc-memory sc-kpm-common nlohmann_json::nlohmann_json)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-ui)
endif()
