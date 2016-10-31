project (sc-machine)
site_name (www.ostis.net)
cmake_minimum_required (VERSION 2.8)

set(SC_MACHINE_ROOT ${CMAKE_SOURCE_DIR})
set(SC_MACHINE_THIRDPARTY_PATH "${SC_MACHINE_ROOT}/thirdparty")
set(SC_MACHINE_ANTRL_DIR "${SC_MACHINE_THIRDPARTY_PATH}/antlr")

# codegen
if (${UNIX})
    set(SC_CODEGEN_TOOL "${SC_MACHINE_ROOT}/bin/sc-code-generator")
elseif (${WIN32})
    set(SC_CODEGEN_TOOL "${SC_MACHINE_ROOT}/bin/sc-code-generator.exe")
endif()
include("cmake/codegen.cmake")

find_package(Java)

set(CMAKE_MODULE_PATH "${SC_MACHINE_ROOT}/cmake")

set (JAVA_EXE ${Java_JAVA_EXECUTABLE})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin)

set(SC_EXTENSION_DIRECTORY "${SC_MACHINE_ROOT}/bin/extensions")

# for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${SC_MACHINE_ROOT}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${SC_MACHINE_ROOT}/bin )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
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

    if (NOT DEFINED LIBCLANG_LIBRARIES OR NOT DEFINED LIBCLANG_CXXFLAGS OR NOT DEFINED LIBCLANG_LIBDIR)
        find_package(LibClang REQUIRED)
    endif ()

    pkg_check_modules(LIBCURL REQUIRED libcurl)
   
    include_directories("${SC_MACHINE_THIRDPARTY_PATH}")

    add_definitions(-DSC_PLATFORM_UNIX)

elseif (${WIN32})

	set(PACKAGES_PATH "${CMAKE_BINARY_DIR}/packages")

	if (${MSVC_VERSION} EQUAL "1800")
		set (MSVC_SUFFIX "vc120")
	elseif (${MSVC_VERSION} EQUAL "1900")
		set (MSVC_SUFFIX "vc140")
	endif()

	set (BOOST_VERSION "1.61.0.0")
	configure_file("${SC_MACHINE_ROOT}/packages.config.in" "${CMAKE_BINARY_DIR}/packages.config")

	file(MAKE_DIRECTORY ${PACKAGES_PATH})
	execute_process(COMMAND "${SC_MACHINE_THIRDPARTY_PATH}/nuget.exe" install ${CMAKE_BINARY_DIR}/packages.config WORKING_DIRECTORY ${PACKAGES_PATH})

	

	set (GLIB_PATH "${PACKAGES_PATH}/glib.2.36.2.11/build/native")
	set (REDIS_PATH "${SC_MACHINE_THIRDPARTY_PATH}/libhiredis/")
	set (BOOST_PATH "${PACKAGES_PATH}/boost.${BOOST_VERSION}/lib/native")

	# CURL
    set (LIBCURL_PATH "${SC_MACHINE_THIRDPARTY_PATH}/curl")
    set (LIBCURL_RUNTIME_PATH "${LIBCURL_PATH}/bin")
    set (LIBCURL_INCLUDE_DIRS "${LIBCURL_PATH}/include")
    set (LIBCURL_LIBRARIES "${LIBCURL_PATH}/lib/libcurl.lib" "WSock32.lib" "Ws2_32.lib")

    set (LIBCLANG_INCLUDE_DIRS "${SC_MACHINE_THIRDPARTY_PATH}/clang/include")
    set (LIBCLANG_LIBRARY "libclang")

	set (GLIB2_INCLUDE_DIRS "${GLIB_PATH}/include"
							"${GLIB_PATH}/include/glib")
	set (REDIS_INCLUDE_DIRS "${REDIS_PATH}")

	include_directories("${SC_MACHINE_ANTRL_DIR}/include"
						"${BOOST_PATH}/include"
						"${SC_MACHINE_THIRDPARTY_PATH}")

	link_directories(
                     "${SC_MACHINE_THIRDPARTY_PATH}/clang/lib"
					 "${GLIB_PATH}/lib/v110/x64/Release/dynamic/"
					 "${REDIS_PATH}"
					 "${SC_MACHINE_ANTRL_DIR}"
					 "${PACKAGES_PATH}/boost_filesystem-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
					 "${PACKAGES_PATH}/boost_system-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
					 "${PACKAGES_PATH}/boost_program_options-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
					 "${PACKAGES_PATH}/boost_regex-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/")


	
	set (GLIB2_LIBRARIES "gio-2.0.lib" "glib-2.0.lib" "gmodule-2.0.lib" "gobject-2.0.lib" "gthread-2.0.lib")
	if (${MSVC_VERSION} EQUAL "1800")
		set (REDIS_LIBRARIES "hiredis.vc120.lib" "WSock32.lib" "Ws2_32.lib")
	elseif (${MSVC_VERSION} EQUAL "1900")
		set (REDIS_LIBRARIES "hiredis.vc140.lib" "Win32_Interop.vc140.lib" "WSock32.lib" "Ws2_32.lib")
	endif()
	
	add_definitions(-DSC_PLATFORM_WIN -DHIREDIS_WIN)

	# copy runtime libraries
	set (GLIB_RUNTIME_PATH "${PACKAGES_PATH}/glib.redist.2.36.2.11/build/native/bin/v110/x64/Release/dynamic")
	set (INTL_RUNTIME_PATH "${PACKAGES_PATH}/libintl.redist.0.18.2.10/build/native/bin/v110/x64/Release/dynamic/cdecl")
    set (ZLIB_RUNTIME_PATH "${PACKAGES_PATH}/zlib.v120.windesktop.msvcstl.dyn.rt-dyn.1.2.8.8/lib/native/v120/windesktop/msvcstl/dyn/rt-dyn/x64/Release")
    set (FFI_RUNTIME_PATH "${PACKAGES_PATH}/libffi.redist.3.0.10.3/build/native/bin/x64/v110/dynamic/Release")
    set (CLANG_RUNTIME_PATH "${SC_MACHINE_THIRDPARTY_PATH}/clang")
	set (WIN_RUNTIME_LIBRARIES
			"${GLIB_RUNTIME_PATH}/glib-2-vs10.dll"
			"${GLIB_RUNTIME_PATH}/gmodule-2-vs10.dll"
            "${GLIB_RUNTIME_PATH}/gio-2-vs10.dll"
            "${GLIB_RUNTIME_PATH}/gobject-2-vs10.dll"
			"${INTL_RUNTIME_PATH}/libintl.dll"
            "${ZLIB_RUNTIME_PATH}/zlib.dll"
            "${FFI_RUNTIME_PATH}/libffi.dll"
            "${CLANG_RUNTIME_PATH}/libclang.dll"
            "${LIBCURL_RUNTIME_PATH}/libcurl.dll"
            "${LIBCURL_RUNTIME_PATH}/libeay32.dll"
            "${LIBCURL_RUNTIME_PATH}/ssleay32.dll"
			)
	file(COPY ${WIN_RUNTIME_LIBRARIES} DESTINATION "${SC_MACHINE_ROOT}/bin")

endif(${UNIX})

include_directories(${SC_MACHINE_ROOT} ${GLIB2_INCLUDE_DIRS})

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

