set(SC_CONFIG_UTILS_SRC ${SC_TOOLS_SRC}/sc-config-utils)
set(SC_BUILDER_SRC ${SC_TOOLS_SRC}/sc-builder)

add_subdirectory(codegen)
add_subdirectory(sc-config-utils)
add_subdirectory(sc-server)
add_subdirectory(sc-builder)

add_custom_target(sc-tools)

add_dependencies(sc-tools
    sc-config-utils
    sc-builder
)
