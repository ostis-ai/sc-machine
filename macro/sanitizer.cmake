macro(sanitizer _NAME _INFO _MODE _CONFIG)
	set(${_NAME} ${_MODE} CACHE STRING ${_INFO})
	set_property(CACHE ${_NAME} PROPERTY STRINGS none address memory)

	if (NOT ${_NAME} STREQUAL "none")
		if (NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR NOT CMAKE_C_COMPILER_ID MATCHES ".*Clang")
			message("Sanitizers are supported only by Clang")
		else()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${_MODE} ${_CONFIG}")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${_MODE} ${_CONFIG}")
		endif()
	endif()
endmacro()

macro(address_sanitizer _NAME _INFO)
	sanitizer(${_NAME} ${_INFO} "address" "-O1 -fno-omit-frame-pointer")
endmacro()

macro(memory_sanitizer _NAME _INFO)
	sanitizer(${_NAME} ${_INFO} "memory" "-fPIE -pie -fno-omit-frame-pointer")
endmacro()

macro(leak_sanitizer _NAME _INFO)
	sanitizer(${_NAME} ${_INFO} "leak" "-fPIE -fno-omit-frame-pointer")
endmacro()

macro(no_sanitizer _NAME _INFO)
	sanitizer(${_NAME} ${_INFO} "none" "")
endmacro()

