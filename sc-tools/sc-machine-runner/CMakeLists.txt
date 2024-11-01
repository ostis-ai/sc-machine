file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(sc-machine-runner SHARED ${SOURCES})
target_link_libraries(sc-machine-runner
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-config
)
target_include_directories(sc-machine-runner
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(sc-machine "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(sc-machine LINK_PRIVATE sc-machine-runner)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-machine-runner)
    target_clangformat_setup(sc-machine)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
