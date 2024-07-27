install(TARGETS 
    sc-machine sc-builder sc-machine-runner sc-server sc-server-lib
    sc-kpm-common
    sc-core sc-memory sc-agents-common sc-builder-lib sc-config
    EXPORT sc-machineExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT sc-machineExport
    FILE sc_machineTargets.cmake
    NAMESPACE sc_machine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc_machine
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/sc_machine-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${SC_MACHINE_ROOT}/cmake/sc_machine-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/sc_machine-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc_machine
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/sc_machine-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/sc_machine-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sc_machine)

include(CPack)
