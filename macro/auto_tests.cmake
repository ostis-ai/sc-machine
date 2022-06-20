macro(auto_tests _NAME _INFO _MODE)
	option(${_NAME} ${_INFO} ${_MODE})

	if (${_NAME})
		add_definitions(-DSC_BUILD_AUTO_TESTS)
	endif()
endmacro()
