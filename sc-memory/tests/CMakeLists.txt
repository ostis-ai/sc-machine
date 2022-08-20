if(${SC_BUILD_BENCH})
    add_subdirectory(performance)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/sc-memory/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/scs/tests.cmake)
