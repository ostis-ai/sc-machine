macro(sc_codegen Target SrcPath)
	
	# fetch all include directories for the project target
	get_property(DIRECTORIES TARGET ${Target} PROPERTY INCLUDE_DIRECTORIES)

	# build the include directory flags
	foreach (DIRECTORY ${DIRECTORIES})
	    set(META_FLAGS ${meta_flags} "\\-I${DIRECTORY}")
	endforeach ()

	# include the system directories
	if (MSVC)
	    # assume ${VS_VERSION} is the version of Visual Studio being used to compile
	    set(META_FLAGS ${META_FLAGS} 
	        "\\-IC:/Program Files (x86)/Microsoft Visual Studio ${VS_VERSION}/VC/include"
	    )
	else ()
	    # you can figure it out for other compilers :)
	    message(FATAL_ERROR "System include directories not implemented for this compiler.")
	endif ()

	add_custom_command(
    	PRE_BUILD
    	TARGET ${Target}
		DEPENDS ${Headers}
		COMMAND call "${SC_PARSE_TOOL}"
		--target 	"${Target}"
		--source 	"${SrcPath}"
		--output 	"${CMAKE_CURRENT_BINARY_DIR}/GeneratedCode"
		--flags 	${META_FLAGS}
	)

endmacro(sc_codegen)