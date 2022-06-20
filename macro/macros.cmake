include(${CMAKE_CURRENT_LIST_DIR}/project_version.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/project_config.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/auto_tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/build_tests.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/code_coverage.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/clang_format.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/codegen.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sanitizer.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/compiler.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/logger.cmake)
