if(${SC_BUILD_BENCH})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sc-memory)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/scs)
