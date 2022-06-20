macro(logger _LOG _INFO _LOG_DIR _LOG_MODE _LOG_TYPE)
	add_definitions(
			-D${_LOG}_DIR="${_LOG_DIR}"
			-D${_LOG}_MODE="${_LOG_MODE}"
			-D${_LOG}_TYPE="${_LOG_TYPE}"
	)
endmacro()
