project (sc_storage)
site_name (www.ostis.net)
cmake_minimum_required (VERSION 2.6)

set(SC_STORE_ROOT ${CMAKE_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)
add_subdirectory(test)

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

