file(MAKE_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} "${SC_EXTENSIONS_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} "${SC_EXTENSIONS_DIRECTORY}")
endforeach(OUTPUT_CONFIG CMAKE_CONFIGURATION_TYPES)

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
