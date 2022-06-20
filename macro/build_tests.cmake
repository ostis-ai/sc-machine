macro(build_tests _NAME _INFO _MODE _CMAKE_DIR)
	option(${_NAME} ${_INFO} ${_MODE})

	if (${_NAME})
		include(${_CMAKE_DIR}/tests.cmake)
	endif()
endmacro()
