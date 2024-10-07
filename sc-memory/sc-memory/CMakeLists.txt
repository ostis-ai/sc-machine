set(SC_MACHINE_ANTRL_DIR "${SC_MACHINE_THIRDPARTY_PATH}/antlr")
set(SC_MEMORY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

find_package(Java)

add_custom_command(
    OUTPUT "${SC_MEMORY_SRC}/scs/scsLexer.cpp" "${SC_MEMORY_SRC}/scs/scsLexer.h" 
           "${SC_MEMORY_SRC}/scs/scsParser.cpp" "${SC_MEMORY_SRC}/scs/scsParser.h"
    COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${SC_MACHINE_ANTRL_DIR}/antlr-4.7.1-complete.jar" scs.g4 -no-listener -package scs
    WORKING_DIRECTORY "${SC_MEMORY_SRC}/scs"
    DEPENDS "${SC_MEMORY_SRC}/scs/scs.g4"
)

add_custom_target(SCsParser ALL
    DEPENDS "${SC_MEMORY_SRC}"
    SOURCES "${SC_MEMORY_SRC}/scs/scs.g4"
)

set(SOURCES_PARSER
    "${SC_MEMORY_SRC}/scs/scsLexer.cpp"
    "${SC_MEMORY_SRC}/scs/scsLexer.h"
    "${SC_MEMORY_SRC}/scs/scsParser.cpp"
    "${SC_MEMORY_SRC}/scs/scsParser.h"
)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.hpp" "src/*.cpp"
    "src/*/*.hpp" "src/*/*.cpp"
    "include/sc-memory/*.hpp"
    "include/sc-memory/*/*.hpp"
    "include/sc-memory/*/*.tpp"
)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

set(GLIB2_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})

add_library(sc-memory SHARED ${SOURCES} ${SOURCES_PARSER})
target_link_libraries(sc-memory
    LINK_PRIVATE ${GLIB2_LIBRARIES}
    LINK_PUBLIC sc-core
    LINK_PRIVATE antlr4_static
)
target_include_directories(sc-memory
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${SC_MACHINE_ANTRL_DIR}/runtime/src
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_dependencies(sc-memory 
    sc-core
    antlr4_static
    SCsParser
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
