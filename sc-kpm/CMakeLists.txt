set(SC_KPM_ROOT "${CMAKE_SOURCE_DIR}/sc-kpm")

add_subdirectory(common)

file(MAKE_DIRECTORY ${SC_EXTENSION_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSION_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${SC_MACHINE_ROOT}/bin/extensions")
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${SC_MACHINE_ROOT}/bin/extensions")
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

add_subdirectory(nl)
add_subdirectory(iot)
add_subdirectory(ui)
add_subdirectory(search)
add_subdirectory(merge)
add_subdirectory(utils)
add_subdirectory(scp)