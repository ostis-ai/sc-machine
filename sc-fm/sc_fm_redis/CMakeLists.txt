file(GLOB_RECURSE SOURCES "*.c" "*.h")

add_library (sc-fm-redis SHARED ${SOURCES})

target_include_directories(sc-fm-redis 
  PRIVATE ${REDIS_INCLUDE_DIRS}
  PRIVATE ${GLIB2_INCLUDE_DIRS}
)

target_link_libraries(sc-fm-redis sc-core ${REDIS_LIBRARIES})
add_dependencies(sc-fm-redis sc-core)

