macro(sc_target_dependencies)
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
        message(FATAL_ERROR "OSTIS Platform isn't supported on Windows OS")
    endif(${UNIX})

    include("${CMAKE_MODULE_PATH}/codegen.cmake")
endmacro()

macro(sc_linux_target_dependencies)
    set(SC_CODEGEN_TOOL "${SC_BIN_PATH}/sc-code-generator")

    find_package(Boost 1.71 REQUIRED COMPONENTS filesystem system program_options)
    find_package(websocketpp REQUIRED 0.8.3)
    find_package(nlohmann_json)

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
