cmake_minimum_required(VERSION 2.8)
project(sc-machine)
site_name(www.ostis.net)
cmake_policy(SET CMP0048 NEW)

set(SC_MACHINE_ROOT ${CMAKE_CURRENT_LIST_DIR})
include(${SC_MACHINE_ROOT}/macro/macros.cmake)
project_version(SC_MACHINE "0.6.1")
project_config("${SC_MACHINE_ROOT}/config" "sc-machine.ini.in" "sc-machine.ini")

set(CMAKE_MODULE_PATH "${SC_MACHINE_ROOT}/cmake")
set(SC_BIN_PATH "${SC_MACHINE_ROOT}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_BIN_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH})
set(SC_EXTENSIONS_DIRECTORY "${SC_BIN_PATH}/extensions")
set(SC_MACHINE_THIRDPARTY_PATH "${SC_MACHINE_ROOT}/thirdparty")

auto_tests(SC_AUTO_TEST "Flag to build for automation testing" OFF)
build_tests(SC_BUILD_TESTS "Flag to build unit tests" ON ${CMAKE_MODULE_PATH})
option(SC_BUILD_BENCH "Flag to build benchmark" OFF)
option(SC_BUILD_SCTP "Flag to turn on/off sctp protocol support" ON)

code_coverage(SC_COVERAGE "Flag to generate coverage report" OFF "-g -O0 --coverage")
clang_format(SC_CLANG_FORMAT_CODE "Flag to add clangformat and clangformat_check targets" OFF ${CMAKE_MODULE_PATH})
no_sanitizer(SC_USE_SANITIZER "Build with specified sanitizer")
codegen(SC_CODEGEN_TOOL "Tool to generate template code" ${SC_BIN_PATH} ${CMAKE_MODULE_PATH})
logger(SC_MACHINE_LOG "Memory messages logger" "${SC_MACHINE_ROOT}/log/sc-machine.log" Debug Console)

compiler(CMAKE_CXX_COMPILER_ID ${SC_COVERAGE} ${SC_USE_SANITIZER} ${SC_AUTO_TEST})

set(SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory")
set(SC_KPM_SRC "${SC_MACHINE_ROOT}/sc-kpm")
set(SC_TOOLS_SRC "${SC_MACHINE_ROOT}/tools")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

include("${SC_MACHINE_ROOT}/dependencies.cmake")
sc_target_dependencies("${SC_MACHINE_ROOT}/packages.config.in" ${SC_MACHINE_THIRDPARTY_PATH} {$SC_MACHINE_BIN_PATH})

add_subdirectory(${SC_MACHINE_ROOT}/thirdparty)

add_subdirectory(${SC_MACHINE_ROOT}/sc-memory)
add_subdirectory(${SC_MACHINE_ROOT}/sc-kpm)
add_subdirectory(${SC_MACHINE_ROOT}/tools)

if (${SC_BUILD_SCTP})
	add_subdirectory(${SC_MACHINE_ROOT}/sc-network)
endif()
