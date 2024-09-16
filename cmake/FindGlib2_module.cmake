# - Try to find GLib2Module
# Once done this will define
#
#  GLIB2_MODULE_FOUND - system has GLib2
#  GLIB2_MODULE_INCLUDE_DIRS - the GLib2 include directory
#  GLIB2_MODULE_LIBRARIES - Link these to use GLib2
#

IF (GLIB2_MODULE_LIBRARIES AND GLIB2_MODULE_INCLUDE_DIRS )
  # in cache already
  SET(GLIB2_MODULE_FOUND TRUE)

ELSE (GLIB2_MODULE_LIBRARIES AND GLIB2_MODULE_INCLUDE_DIRS )

  INCLUDE(FindPkgConfig)

  ## Glib
  IF ( GLIB2_MODULE_FIND_REQUIRED )
    SET( _pkgconfig_REQUIRED "REQUIRED" )
  ELSE ( GLIB2_MODULE_FIND_REQUIRED )
    SET( _pkgconfig_REQUIRED "" )
  ENDIF ( GLIB2_MODULE_FIND_REQUIRED )

  # Look for glib2 include dir and libraries w/o pkgconfig
  IF ( NOT GLIB2_MODULE_FOUND AND NOT PKG_CONFIG_FOUND )
    FIND_PATH(
      _glib2_module_include_DIR
    NAMES
      gmodule.h
    PATHS
      /opt/gnome/include
      /opt/local/include
      /sw/include
      /usr/include
      /usr/local/include
    PATH_SUFFIXES
      glib-2.0
    )

    #MESSAGE(STATUS "Glib headers: ${_glib2_include_DIR}")

    FIND_LIBRARY(
      _glib2_module_link_DIR
    NAMES
      gmodule-2.0
      gmodule
    PATHS
      /opt/gnome/lib
      /opt/local/lib
      /sw/lib
      /usr/lib
      /usr/local/lib
    )
    IF ( _glib2_module_include_DIR AND _glib2_module_link_DIR )
        SET ( _glib2_module_FOUND TRUE )
    ENDIF ( _glib2_module_include_DIR AND _glib2_module_link_DIR )


    IF ( _glib2_module_FOUND )
        SET ( GLIB2_MODULE_INCLUDE_DIRS ${_glib2_module_include_DIR})
        SET ( GLIB2_MODULE_LIBRARIES ${_glib2_module_link_DIR} )
        SET ( GLIB2_MODULE_FOUND TRUE )
    ELSE ( _glib2_module_FOUND )
        SET ( GLIB2_MODULE_FOUND FALSE )
    ENDIF ( _glib2_module_FOUND )

  ENDIF ( NOT GLIB2_MODULE_FOUND AND NOT PKG_CONFIG_FOUND )
  ##

  IF (GLIB2_MODULE_FOUND)
    IF (NOT GLIB2_MODULE_FIND_QUIETLY)
      MESSAGE (STATUS "Found GLib2 module: ${GLIB2_MODULE_LIBRARIES} ${GLIB2_MODULE_INCLUDE_DIRS}")
    ENDIF (NOT GLIB2_MODULE_FIND_QUIETLY)
  ELSE (GLIB2_MODULE_FOUND)
    IF (GLIB2_MODEUL_FIND_REQUIRED)
      MESSAGE (SEND_ERROR "Could not find GLib2 module")
    ENDIF (GLIB2_MODULE_FIND_REQUIRED)
  ENDIF (GLIB2_MODULE_FOUND)

  # show the GLIB2_MODULE_INCLUDE_DIRS and GLIB2_MODULE_LIBRARIES variables only in the advanced view
  MARK_AS_ADVANCED(GLIB2_MODULE_INCLUDE_DIRS GLIB2_MODULE_LIBRARIES)

ENDIF (GLIB2_MODULE_LIBRARIES AND GLIB2_MODULE_INCLUDE_DIRS)
