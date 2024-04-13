file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.c" "*/*.h" "*/*.cpp" "*/*.hpp"
)

add_library(sc-config-utils SHARED ${SOURCES})
target_link_libraries(sc-config-utils LINK_PUBLIC sc-memory)
target_include_directories(sc-config-utils PUBLIC ${SC_MEMORY_SRC})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-config-utils)
endif()
