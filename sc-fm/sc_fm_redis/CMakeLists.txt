file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-fm-redis SHARED ${SOURCES} ${HEADERS})

# find dependencies
if (${UNIX})
	include(FindPkgConfig)
	pkg_check_modules (REDIS REQUIRED libhiredis)
	
endif(${UNIX})

include_directories("${SC_MEMORY_SRC}/sc-store" ${REDIS_INCLUDE_DIRS} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-fm-redis ${REDIS_LIBRARIES})
add_dependencies(sc-fm-redis sc-memory)

