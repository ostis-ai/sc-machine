project (sc-machine)
site_name (www.ostis.net)
cmake_minimum_required (VERSION 2.6)

set(SC_MACHINE_ROOT ${CMAKE_SOURCE_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)

set(SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory/src")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")        ## Optimize
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")  ## Strip binary
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")

# find dependencies
if (${UNIX})
	include(FindPkgConfig)
	pkg_check_modules (GLIB2 REQUIRED glib-2.0)
	pkg_check_modules (GLIB2_MODULE REQUIRED gmodule-2.0)
	
	set (GLIB2_INCLUDE_DIRS "${GLIB2_INCLUDE_DIRS}" "/usr/lib/x86_64-linux-gnu/glib-2.0/include/" "${GLIB2_MODULE}")
	set (GLIB2_LIBRARIES "${GLIB2_LIBRARIES}" "${GLIB2_MODULE_LIBRARIES}")

        pkg_check_modules(REDIS QUIET hiredis)
        if (NOT DEFINED ${REDIS_INCLUDE_DIRS})
            pkg_check_modules(REDIS REQUIRED libhiredis)
        endif()


endif(${UNIX})

add_subdirectory(sc-memory)
add_subdirectory(sc-fm)
add_subdirectory(sc-kpm)
add_subdirectory(sc-network)
add_subdirectory(tools)

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
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

