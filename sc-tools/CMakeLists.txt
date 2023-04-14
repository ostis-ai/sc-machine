set(SC_CONFIG_UTILS_SRC ${SC_TOOLS_SRC}/sc-config-utils)
set(SC_BUILDER_SRC ${SC_TOOLS_SRC}/sc-builder)

set(SC_COMPONENT_MANAGER False CACHE BOOL "Sc-component-manager support")

add_subdirectory(codegen)
add_subdirectory(sc-config-utils)
add_subdirectory(sc-server)
add_subdirectory(sc-builder)

if(${SC_COMPONENT_MANAGER})
    add_subdirectory(sc-component-manager)
endif()

add_custom_target(sc-tools)

add_dependencies(sc-tools
    sc-config-utils
    sc-builder
)
