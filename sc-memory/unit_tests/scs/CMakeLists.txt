# scs tests
set(SC_MEMORY_WRAP_TEST "${SC_MACHINE_ROOT}/sc-memory/test/scs")

set(SOURCES_SCS_TEST
  "test_scs.cpp"
  )

file(GLOB UnitsGlob "units/*.cpp" "units/*.hpp")

add_executable(test_scs ${SOURCES_SCS_TEST} ${UnitsGlob})
target_include_directories(test_scs 
  PRIVATE ${SC_MEMORY_SRC})

target_link_libraries(test_scs
  sc-memory
  sc-test-framework)
