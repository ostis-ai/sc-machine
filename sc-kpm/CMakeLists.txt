add_subdirectory(sc-common)
add_subdirectory(sc-agents-common)

add_subdirectory(sc-ui)
add_subdirectory(sc-search)
add_subdirectory(sc-utils)

add_custom_target(sc-kpm)
add_dependencies(sc-kpm
    sc-agents-common
    sc-kpm-common
)
