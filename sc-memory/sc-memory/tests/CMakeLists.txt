if(${SC_BUILD_BENCH})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/performance)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sc-memory)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/scs)
