file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.h"
    "src/*/*.cpp" "src/*/*.h"
)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(sc-kpm-ui SHARED ${SOURCES})
target_link_libraries(sc-kpm-ui
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-kpm-common
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_include_directories(sc-kpm-ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-ui)
endif()

if(${SC_BUILD_TESTS})
    set(SC_KPM_UI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_subdirectory(tests)
endif()
