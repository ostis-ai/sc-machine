include_directories(${SC_MEMORY_SRC})

add_subdirectory(sc_fm_filesystem)
add_subdirectory(sc_fm_redis)

add_custom_target(sc-fm)
add_dependencies(sc-fm
  sc-fm-filesystem
  sc-fm-redis
)
