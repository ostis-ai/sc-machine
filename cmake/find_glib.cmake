macro(find_glib)
    if(NOT glib_CACHED)
        find_package(glib QUIET)

        if(NOT glib_FOUND)
            include(FindPkgConfig)
            find_package(PkgConfig REQUIRED)
            pkg_search_module(GLIB2 REQUIRED glib-2.0)
            pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

            if(NOT GLIB2_FOUND OR NOT GLIB2_MODULE_FOUND)
                message(FATAL_ERROR "glib2 not found!")
            endif()
            set(glib_INCLUDE_DIRS ${GLIB2_INCLUDE_DIRS} ${GLIB2_MODULE_INCLUDE_DIRS}
                CACHE STRING "Include directories for GLib"
            )
            set(glib_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS} 
                CACHE STRING "Libraries for GLib"
            )
            
            set(glib_CACHED TRUE CACHE BOOL "GLib found")
        endif()
    endif()
endmacro()
