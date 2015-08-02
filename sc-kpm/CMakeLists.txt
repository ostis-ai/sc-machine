set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SC_MACHINE_ROOT}/bin/extensions")
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${SC_MACHINE_ROOT}/bin/extensions")
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${SC_MACHINE_ROOT}/bin/extensions")
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(SC_KPM_ROOT "${CMAKE_SOURCE_DIR}/sc-kpm")

add_subdirectory(common)
add_subdirectory(ui)
add_subdirectory(search)
add_subdirectory(merge)
add_subdirectory(utils)
