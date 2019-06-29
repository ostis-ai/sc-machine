set (SC_BUILDER_DIR "${SC_TOOLS_DIR}/builder")

file(MAKE_DIRECTORY "${SC_BUILDER_DIR}/src/parser")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

if (${WIN32})
  set (BOOST_LIBS_LIST )

  elseif (${APPLE})
   set (BOOST_LIBS_LIST
         Boost::filesystem 
         Boost::program_options 
         Boost::system
      )
else()
  set (BOOST_LIBS_LIST
         boost_filesystem 
         boost_program_options 
         boost_system
      )
endif (${WIN32})
  
add_executable(sc-builder ${SOURCES})
target_include_directories(sc-builder 
  PRIVATE ${SC_MEMORY_SRC}
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )
target_link_libraries(sc-builder sc-core sc-memory ${BOOST_LIBS_LIST})

sc_codegen(sc-builder "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_dependencies(sc-builder sc-kpm)


add_subdirectory(tests)