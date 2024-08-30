install(TARGETS 
    sc-machine sc-builder sc-machine-runner sc-server 
    sc-kpm-common
    sc-core sc-memory sc-agents-common sc-builder-lib sc-config
    EXPORT sc-machineExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# a target set for things we don't want to export to the consumers
install(TARGETS sc-server-lib EXPORT privateExport LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT sc-machineExport
    FILE sc-machineTargets.cmake
    NAMESPACE sc-machine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc-machine
)

install(EXPORT privateExport
    FILE privateTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc-machine
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/sc-machine-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${SC_MACHINE_ROOT}/cmake/sc-machine-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/sc-machine-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc-machine
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/sc-machine-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/sc-machine-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc-machine)

set(CPACK_PACKAGE_NAME                  sc-machine)
set(CPACK_PACKAGE_VENDOR                "OSTIS AI")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Software implementation of semantic network storage")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR         ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES            TRUE)

if (WIN32)
    set(CPACK_GENERATOR     ZIP)
else()
    set(CPACK_GENERATOR     TGZ)
endif()
include(CPack)
