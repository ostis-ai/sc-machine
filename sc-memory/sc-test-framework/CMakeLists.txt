file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (sc-test-framework SHARED ${SOURCES})

target_include_directories(sc-test-framework
    PRIVATE ${SC_MEMORY_SRC}
    )

add_dependencies(sc-test-framework
    sc-memory
    )
