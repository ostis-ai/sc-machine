macro(FindGLIB)
    find_package(glib QUIET)

    if (NOT glib_FOUND)
        message("Trying to use system-installed glib")

        find_package(PkgConfig REQUIRED)
        pkg_search_module(GLIB2 REQUIRED glib-2.0)
        pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

        if (NOT GLIB2_FOUND OR NOT GLIB2_MODULE_FOUND)
            error("glib2 not found!")
        endif()

        set(glib_INCLUDE_DIRS ${GLIB2_INCLUDE_DIRS} ${GLIB2_MODULE_INCLUDE_DIRS})
        set(glib_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})
    endif()
endmacro()
