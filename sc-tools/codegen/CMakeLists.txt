set(SC_CODEGEN_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB SOURCES CONFIGURE_DEPENDS
    "Parser/*.cpp" "Parser/*.hpp"
    "Parser/*/*.cpp" "Parser/*/*.hpp"
)

if(${APPLE})
    set(BOOST_LIBS_LIST Boost::filesystem Boost::program_options)
else()
    set(BOOST_LIBS_LIST boost_filesystem boost_program_options boost_system)
endif()

add_executable(sc-code-generator ${SOURCES})
target_link_libraries(sc-code-generator
    LINK_PRIVATE ${LIBCLANG_LIBRARY}
    LINK_PRIVATE ${BOOST_LIBS_LIST}
)
target_include_directories(sc-code-generator PRIVATE ${SC_CODEGEN_DIR}/Parser)
set_target_properties(sc-code-generator PROPERTIES COMPILE_FLAGS ${LIBCLANG_CXXFLAGS})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-code-generator)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(NOT ${SC_USE_SANITIZER} STREQUAL "none")
        target_compile_options(sc-code-generator PRIVATE -fno-sanitize=${SC_USE_SANITIZER})
        target_link_options(sc-code-generator PRIVATE -fno-sanitize=${SC_USE_SANITIZER})
    endif()
endif()
