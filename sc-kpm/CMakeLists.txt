set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SC_MACHINE_ROOT}/bin/extensions")
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(SC_KPM_ROOT "${CMAKE_SOURCE_DIR}/sc-kpm")

add_subdirectory(common)
add_subdirectory(ui)
add_subdirectory(search)
add_subdirectory(merge)
add_subdirectory(utils)
