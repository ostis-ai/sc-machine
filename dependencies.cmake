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
        message(SEND_ERROR "sc-machine isn't supported on Windows OS.")
    endif(${UNIX})
endmacro()

macro(sc_linux_target_dependencies)
    find_package(nlohmann_json 3.2.0 REQUIRED)

    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB2 REQUIRED glib-2.0)
    pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

    set(GLIB2_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})

    # for std::thread support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endmacro()
