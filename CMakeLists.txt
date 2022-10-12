cmake_minimum_required(VERSION 2.8.12)
project(sc-machine)
site_name(www.ostis.net)
cmake_policy(SET CMP0048 NEW)

set(SC_MACHINE_ROOT ${CMAKE_CURRENT_LIST_DIR})
include(${SC_MACHINE_ROOT}/macro/macros.cmake)
project_version(SC_MACHINE "0.7.0-Rebirth")

set(CMAKE_MODULE_PATH "${SC_MACHINE_ROOT}/cmake")
# CMAKE_BINARY_DIR is a path to the top level of the build tree (i.e to the "build" folder). Placing a "bin" folder nearby
set(SC_BIN_PATH "${CMAKE_BINARY_DIR}/../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_BIN_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH})
set(SC_EXTENSIONS_DIRECTORY "${SC_BIN_PATH}/extensions")
message("Binary output dir:" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(SC_MACHINE_THIRDPARTY_PATH "${SC_MACHINE_ROOT}/thirdparty")

option(SC_AUTO_TEST "Flag to build for automation testing" OFF)
option(SC_BUILD_TESTS "Flag to build unit tests" OFF)
option(SC_BUILD_BENCH "Flag to build benchmark" OFF)

set(SC_FILE_MEMORY "Dictionary" CACHE STRING "Sc-fs-storage type")

code_coverage(SC_COVERAGE "Flag to generate coverage report" OFF "-g -O0 --coverage")
option(SC_CLANG_FORMAT_CODE "Flag to add clangformat and clangformat_check targets" OFF)
sanitizer(SC_USE_SANITIZER "Build with specified sanitizer" "none")

set(SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory")
set(SC_KPM_SRC "${SC_MACHINE_ROOT}/sc-kpm")
set(SC_TOOLS_SRC "${SC_MACHINE_ROOT}/sc-tools")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

option(AUTO_CCACHE "Use ccache to speed up rebuilds" ON)

if(${SC_AUTO_TEST})
    add_definitions(-DSC_BUILD_AUTO_TESTS)
endif()

if(${SC_BUILD_TESTS})
    include(${CMAKE_MODULE_PATH}/tests.cmake)
endif()

if(${SC_CLANG_FORMAT_CODE})
    include(${CMAKE_MODULE_PATH}/ClangFormat.cmake)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -std=c++14")
    if (${SC_USE_SANITIZER} STREQUAL "none" AND NOT ${SC_COVERAGE})
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    endif()
    if(${SC_AUTO_TEST})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated-declarations")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
    if(${SC_USE_SANITIZER} STREQUAL "none" AND NOT ${SC_COVERAGE})
	    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3") ## Optimize
    endif()
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s") ## Strip binary
elseif (NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM AND ${AUTO_CCACHE})
    message(STATUS "Using ${CCACHE_PROGRAM} as compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")  # requires at least CMake 3.9 to be any use
    set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

if(${SC_FILE_MEMORY} STREQUAL "Dictionary")
    message("Build with sc-dictionary fs-storage")
    add_definitions(-DSC_DICTIONARY_FS_STORAGE)
elseif(${SC_FILE_MEMORY} STREQUAL "Rocksdb")
    message("Build with sc-rocksdb fs-storage")
    add_definitions(-DSC_ROCKSDB_FS_STORAGE)
endif()

# for multi-config builds (e.g. msvc)
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${SC_EXTENSIONS_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${SC_EXTENSIONS_DIRECTORY})
endforeach(OUTPUT_CONFIG CMAKE_CONFIGURATION_TYPES)

include("${SC_MACHINE_ROOT}/dependencies.cmake")
sc_target_dependencies("${SC_MACHINE_ROOT}/packages.config.in" ${SC_MACHINE_THIRDPARTY_PATH} {$SC_MACHINE_BIN_PATH})

message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

add_subdirectory(${SC_MACHINE_ROOT}/thirdparty)

add_subdirectory(${SC_MACHINE_ROOT}/sc-memory)
add_subdirectory(${SC_MACHINE_ROOT}/sc-kpm)
add_subdirectory(${SC_MACHINE_ROOT}/sc-tools)
