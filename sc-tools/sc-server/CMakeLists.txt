set(SC_SERVER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

list(REMOVE_ITEM SOURCES
		"${SC_SERVER_ROOT}/src/main.cpp"
		"${SC_SERVER_ROOT}/src/sc_server_factory.cpp"
		"${SC_SERVER_ROOT}/src/sc_server_factory.hpp"
		)

if(${WIN32})
	set(BOOST_LIBS_LIST)

elseif(${APPLE})
	set(BOOST_LIBS_LIST
			Boost::filesystem
			Boost::program_options
			Boost::system
			)
else()
	set(BOOST_LIBS_LIST
			boost_filesystem
			boost_program_options
			boost_system
			)
endif(${WIN32})

add_executable(sc-server
		"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/sc_server_factory.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/sc_server_factory.hpp"
		)

add_library(sc-server-lib SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${SC_SERVER_ROOT}/../sc-config-utils)
target_link_libraries(sc-server-lib sc-memory ${BOOST_LIBS_LIST})
target_link_libraries(sc-server sc-server-lib sc-config-utils)

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(sc-server)
endif ()

install_targets("/bin" sc-server sc-server-lib)
sc_codegen(sc-server-lib ${SC_SERVER_ROOT}/src)

if (${SC_BUILD_TESTS})
	include (tests/tests.cmake)
endif()
