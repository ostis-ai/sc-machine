file(GLOB_RECURSE SOURCES "*.c" "*.h" "*.cpp" "*.hpp")

include_directories(${SC_CONFIG_UTILS_SRC} ${GLIB2_INCLUDE_DIRS})

add_library(sc-config-utils SHARED ${SOURCES})

target_include_directories(sc-config-utils
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE ${GLIB2_INCLUDE_DIRS}
)
target_link_libraries(sc-config-utils sc-memory ${GLIB2_LIBRARIES})
if(${SC_COVERAGE})
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_link_libraries(sc-config-utils gcov)
	else()
        message(SEND_ERROR "Currently, code coverage is supported only with GCC as a compiler")
    endif()
endif()

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-config-utils)
endif()
