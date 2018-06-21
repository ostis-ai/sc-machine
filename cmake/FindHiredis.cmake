# - Try to find the Lib library
# Once done this will define
#
#  REDIS_FOUND - System has Hiredis
#  REDIS_INCLUDE_DIRS - The Hiredis include directory
#  REDIS_LIBRARIES - The libraries needed to use Hiredis
#  REDIS_DEFINITIONS - Compiler switches required for using Hiredis


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#FIND_PACKAGE(PkgConfig)
#PKG_SEARCH_MODULE(PC_LIBHIREDIS libhiredis)

SET(REDIS_DEFINITIONS ${LIBHIREDIS_CFLAGS_OTHER})

FIND_PATH(REDIS_INCLUDE_DIRS hiredis/hiredis.h
   HINTS
   ${LIBHIREDIS_INCLUDEDIR}
   ${LIBHIREDIS_INCLUDE_DIRS}
)

FIND_LIBRARY(REDIS_LIBRARIES NAMES hiredis
   HINTS
   ${LIBHIREDIS_LIBDIR}
   ${LIBHIREDIS_LIBRARY_DIRS}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibHiredis DEFAULT_MSG REDIS_LIBRARIES REDIS_INCLUDE_DIRS)

#MARK_AS_ADVANCED(REDIS_INCLUDE_DIRS REDIS_LIBRARIES)