add_subdirectory(sc-core)
add_subdirectory(sc-memory)

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
