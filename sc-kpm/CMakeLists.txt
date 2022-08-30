add_subdirectory(sc-common)

file(MAKE_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} "${SC_EXTENSIONS_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} "${SC_EXTENSIONS_DIRECTORY}")
endforeach(OUTPUT_CONFIG CMAKE_CONFIGURATION_TYPES)

add_subdirectory(sc-ui)
add_subdirectory(sc-search)
add_subdirectory(sc-utils)
add_subdirectory(sc-agents-common)

add_custom_target(sc-kpm)

add_dependencies(sc-kpm
    sc-kpm-common
    sc-ui
    sc-search
    sc-utils
)

if(${SC_BUILD_TESTS})
   include(${CMAKE_CURRENT_LIST_DIR}/tests/tests.cmake)
endif()
