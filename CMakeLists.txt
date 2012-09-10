project (sc-machine)
site_name (www.ostis.net)
cmake_minimum_required (VERSION 2.6)

set(SC_MACHINE_ROOT ${CMAKE_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)


add_subdirectory(sc-memory)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	if(MSVC)
		message(STATUS "Compiler: MSVC, version: " ${MSVC_VERSION})
	endif(MSVC)
	if(BORLAND) 
		message(STATUS "Compiler: BCC") 
	endif(BORLAND) 
	else(${CMAKE_SYSTEM_NAME} STREQUAL Linux) 
		message(STATUS "Only GCC is supported on Linux")
	endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows) 

message(STATUS "CMake generates " ${CMAKE_GENERATOR})

