add_subdirectory(sc-common)
add_subdirectory(sc-agents-common)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} "${SC_EXTENSIONS_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} "${SC_EXTENSIONS_DIRECTORY}")
endforeach(OUTPUT_CONFIG)

add_subdirectory(sc-ui)
add_subdirectory(sc-search)
add_subdirectory(sc-utils)

add_custom_target(sc-kpm)

add_dependencies(sc-kpm
    sc-agents-common
    sc-kpm-common
)
