set(SC_CONFIG_SRC ${SC_TOOLS_SRC}/sc-config)
set(SC_BUILDER_SRC ${SC_TOOLS_SRC}/sc-builder)

add_subdirectory(sc-config)
add_subdirectory(sc-builder)
add_subdirectory(sc-machine-runner)
add_subdirectory(sc-server)

add_custom_target(sc-tools)
add_dependencies(sc-tools
    sc-config
    sc-builder
)
