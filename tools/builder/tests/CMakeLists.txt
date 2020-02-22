file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_executable(sc-builder-test ${SOURCES})

target_include_directories(sc-builder-test
  PRIVATE ${SC_MEMORY_SRC}
  )
target_link_libraries(sc-builder-test
  sc-memory
  sc-test-framework
  )

add_dependencies(sc-builder-test
  sc-builder
  sc-memory
  sc-test-framework
  )