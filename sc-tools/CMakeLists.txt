add_subdirectory(sc-config)
add_subdirectory(sc-builder)
add_subdirectory(sc-machine-runner)
add_subdirectory(sc-server)

add_custom_target(sc-tools)
add_dependencies(sc-tools
    sc-config
    sc-builder
)
