macro(sc_target_dependencies _PACKAGES_CONFIG _THIRDPARTY_PATH _BIN_PATH)
	if(${SC_BUILD_ARM64})
        check_c_compiler_flag("-arch arm64" IS_ARM64_SUPPORTED)
        if (${IS_ARM64_SUPPORTED})
            set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Used architecture" FORCE)
            message("-- Architecture: arm64")
        else()
            message(FATAL_ERROR "Arm64 platform is not supported by compiler")
        endif()
    endif()

    include(CTest)
    find_package(Java)

    if(${UNIX})
        sc_linux_target_dependencies()
    elseif(${WIN32})
        sc_win_target_dependencies(${_PACKAGES_CONFIG} ${_THIRDPARTY_PATH} ${_BIN_PATH})
    endif(${UNIX})

    include("${CMAKE_MODULE_PATH}/codegen.cmake")
endmacro()

macro(sc_linux_target_dependencies)
    set(SC_CODEGEN_TOOL "${SC_BIN_PATH}/sc-code-generator")

    find_package(Boost 1.71 REQUIRED COMPONENTS filesystem system program_options)
    find_package(websocketpp REQUIRED 0.8.3)

    if(${SC_FILE_MEMORY} MATCHES "Rocksdb")
	    find_package(RocksDB REQUIRED)
    endif()

    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB2 REQUIRED glib-2.0)
    pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

    set(GLIB2_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})

    if(NOT DEFINED LIBCLANG_LIBRARIES OR NOT DEFINED LIBCLANG_CXXFLAGS OR NOT DEFINED LIBCLANG_LIBDIR)
        find_package(LibClang REQUIRED)
    endif()

	pkg_check_modules(LIBCURL REQUIRED libcurl)

    # for std::thread support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endmacro()

macro(sc_win_target_dependencies _PACKAGES_CONFIG _THIRDPARTY_PATH _BIN_PATH)
    set(PACKAGES_PATH "${CMAKE_BINARY_DIR}/packages")
    set(SC_CODEGEN_TOOL "${SC_BIN_PATH}/sc-code-generator.exe")

    if (${MSVC_VERSION} EQUAL 1800)
        set(MSVC_SUFFIX "vc120")
    elseif(${MSVC_VERSION} EQUAL 1900)
        set(MSVC_SUFFIX "vc140")
    elseif(${MSVC_VERSION} GREATER_EQUAL 1911)
        set(MSVC_SUFFIX "vc141")
    endif()

    set(BOOST_VERSION "1.64.0.0")
    configure_file("${_PACKAGES_CONFIG}" "${CMAKE_BINARY_DIR}/packages.config")

    file(MAKE_DIRECTORY ${PACKAGES_PATH})
    execute_process(COMMAND "${_THIRDPARTY_PATH}/nuget.exe"
         install "${CMAKE_BINARY_DIR}/packages.config"
         WORKING_DIRECTORY ${PACKAGES_PATH}
    )

    set(GLIB_PATH "${PACKAGES_PATH}/glib.2.36.2.11/build/native")
    set(BOOST_PATH "${PACKAGES_PATH}/boost.${BOOST_VERSION}/lib/native")
    if (${SC_FILE_MEMORY} MATCHES "Rocksdb")
	    set(ROCKSDB_PATH "${SC_MACHINE_THIRDPARTY_PATH}/rocksdb")
    endif()

    set(LIBCLANG_INCLUDE_DIRS "${_THIRDPARTY_PATH}/clang/include")
    set(LIBCLANG_LIBRARY "libclang")

    set(GLIB2_INCLUDE_DIRS "${GLIB_PATH}/include" "${GLIB_PATH}/include/glib")

    set(LIBCURL_PATH "${_THIRDPARTY_PATH}/curl")
    set(LIBCURL_RUNTIME_PATH "${LIBCURL_PATH}/bin")
    set(LIBCURL_INCLUDE_DIRS "${LIBCURL_PATH}/include")
    set(LIBCURL_LIBRARIES "${LIBCURL_PATH}/lib/libcurl.lib" "WSock32.lib" "Ws2_32.lib")

    if(${SC_FILE_MEMORY} MATCHES "Rocksdb")
	    set(RocksDB_INCLUDE_DIR "${ROCKSDB_PATH}/include")
	    set(RocksDB_LIBRARIES "${ROCKSDB_PATH}/rocksdb-shared.lib")
    endif()

    include_directories("${BOOST_PATH}/include" "${LIBCURL_INCLUDE_DIRS}")

    link_directories(
        "${_THIRDPARTY_PATH}/clang/lib"
        "${GLIB_PATH}/lib/v110/x64/Release/dynamic/"
        "${PACKAGES_PATH}/boost_filesystem-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
        "${PACKAGES_PATH}/boost_system-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
        "${PACKAGES_PATH}/boost_program_options-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
	)

    set(GLIB2_LIBRARIES "gio-2.0.lib" "glib-2.0.lib" "gmodule-2.0.lib" "gobject-2.0.lib" "gthread-2.0.lib")

    add_definitions(-DSC_PLATFORM_WIN)

    # copy runtime libraries
    set(GLIB_RUNTIME_PATH "${PACKAGES_PATH}/glib.redist.2.36.2.11/build/native/bin/v110/x64/Release/dynamic")
    set(INTL_RUNTIME_PATH "${PACKAGES_PATH}/libintl.redist.0.18.2.10/build/native/bin/v110/x64/Release/dynamic/cdecl")
    set(ZLIB_RUNTIME_PATH "${PACKAGES_PATH}/zlib.v120.windesktop.msvcstl.dyn.rt-dyn.1.2.8.8/lib/native/v120/windesktop/msvcstl/dyn/rt-dyn/x64/Release")
    set(FFI_RUNTIME_PATH "${PACKAGES_PATH}/libffi.redist.3.0.10.3/build/native/bin/x64/v110/dynamic/Release")
    set(CLANG_RUNTIME_PATH "${_THIRDPARTY_PATH}/clang")
    set(WIN_RUNTIME_LIBRARIES
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
        "${ROCKSDB_PATH}/rocksdb-shared.dll"
    )
    file(COPY ${WIN_RUNTIME_LIBRARIES} DESTINATION "${_BIN_PATH}")
endmacro()
