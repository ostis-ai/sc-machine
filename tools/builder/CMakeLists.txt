set(SC_BUILDER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

list(REMOVE_ITEM SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.hpp"
		)

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

add_executable(sc-builder
		"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/builder.hpp"
		)

add_library(sc-builder-lib SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-builder-lib sc-memory ${BOOST_LIBS_LIST})
target_link_libraries(sc-builder sc-builder-lib)

install_targets("/bin" sc-builder sc-builder-lib)
sc_codegen(sc-builder-lib ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (${SC_BUILD_TESTS})
	add_subdirectory(tests)
endif ()

