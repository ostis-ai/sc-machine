set(SC_KPM_ROOT "${SC_MACHINE_ROOT}/sc-kpm")

include_directories(${SC_MEMORY_SRC})

add_subdirectory(sc-common)

file(MAKE_DIRECTORY ${SC_EXTENSION_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSION_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${SC_BIN_PATH}/extensions")
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${SC_BIN_PATH}/extensions")
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

add_subdirectory(sc-http)
add_subdirectory(sc-ui)
add_subdirectory(sc-task)
add_subdirectory(sc-search)
add_subdirectory(sc-merge)
add_subdirectory(sc-utils)

add_custom_target(sc-kpm)

if (${SC_KPM_SCP})
    add_subdirectory(scp)
    add_dependencies(sc-kpm scp)
endif()

add_dependencies(sc-kpm
    sc-kpm-common
    sc-http
    sc-ui
    sc-search
    sc-task
    sc-merge
    sc-utils)
