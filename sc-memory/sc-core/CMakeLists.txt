set(SC_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.c" "*.h"
    "*/*.c" "*/*.h"
    "*/*/*.c" "*/*/*.h"
    "*/*/*/*.c" "*/*/*/*.h"
)

add_library(sc-core SHARED ${SOURCES})
target_link_libraries(sc-core LINK_PUBLIC ${GLIB2_LIBRARIES})
target_include_directories(sc-core PUBLIC ${GLIB2_INCLUDE_DIRS})

if(${SC_COVERAGE})
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	    target_link_libraries(sc-core LINK_PRIVATE gcov)
    else()
        message(SEND_ERROR "Currently, code coverage is supported only with GCC as a compiler")
    endif()
endif()

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core)
endif()
