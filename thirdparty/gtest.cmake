add_library(gtest STATIC
  "googletest/googletest/src/gtest-all.cc"
  "googletest/googlemock/src/gmock-all.cc"
  "googletest/googletest/src/gtest_main.cc"
)

target_include_directories(gtest 
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock/include
)
