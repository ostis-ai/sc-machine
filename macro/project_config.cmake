macro(project_config _DIR _CONFIG _TARGET)
	configure_file("${_DIR}/${_CONFIG}" "${_DIR}/${_TARGET}")
endmacro()
