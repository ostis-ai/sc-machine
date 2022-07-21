set(SC_BUILDER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

list(REMOVE_ITEM SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.hpp"
		)

add_executable(sc-builder
		"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.hpp"
		)

add_library(sc-builder-lib SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${SC_BUILDER_ROOT}/../sc-config-utils)
target_link_libraries(sc-builder-lib sc-memory )
target_link_libraries(sc-builder sc-builder-lib sc-config-utils)

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-builder)
endif ()

install_targets("/bin" sc-builder sc-builder-lib)
sc_codegen(sc-builder-lib ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (${SC_BUILD_TESTS})
  include (tests/tests.cmake)
endif()
