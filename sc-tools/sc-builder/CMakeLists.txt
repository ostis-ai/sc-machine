set(SC_BUILDER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/builder_defines.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/builder_defines.hpp"
)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.hpp"
    "include/sc-builder/*.hpp"
)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

find_package(LibXml2 REQUIRED)

add_library(sc-builder-lib SHARED ${SOURCES})
target_link_libraries(sc-builder-lib
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-config
    LINK_PRIVATE ${LIBXML2_LIBRARIES}
)
target_include_directories(sc-builder-lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${LIBXML2_INCLUDE_DIR}
)

add_executable(sc-builder "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(sc-builder LINK_PRIVATE sc-builder-lib)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder)
    target_clangformat_setup(sc-builder-lib)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()

